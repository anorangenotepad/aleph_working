/* multi_voice.c
 * based on:
 * monosynth.c
   aleph-bfin

   Simple Subtractive ADSR Synth.
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"

// aleph-bfin
#include "bfin_core.h"
#include "cv.h"
#include "gpio.h"
#include "fract_math.h"
#include "libfixmath/fix16_fract.h"
#include <fract2float_conv.h>

// audio
#include "buffer.h"
#include "filter_ramp.h"
#include "filter_1p.h"
#include "filter_svf.h"
#include "biquad.h"
#include "ricks_tricks.h"
#include "osc_polyblep.h"

#include "delayFadeN.h"

#include "module.h"
#include "noise.h"



/// custom
#include "params.h"

#define TEST 0

//static variables
//see lines.c for notes
#define LINES_BUF_FRAMES 0x5FFF40

#define NLINES 1

// data structure of external memory
typedef struct _echo_voicesData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
  volatile fract32 audioBuffer[NLINES][LINES_BUF_FRAMES];
} echo_voicesData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
echo_voicesData* pDacsData;

//delay variables
//since there were two delays in original
//lines.c, the parameters were set up as arrays and
//initialized as such
//i.e. 
//fract32 out_del[NLINES] = {0, 0};
//where NLINES = 2

delayFadeN lines;

fract32 in_del = 0;
fract32 out_del = 0;

u8 fadeTargetRd = 0;
u8 fadeTargetWr = 0;

filter_ramp lpFadeRd;
filter_ramp lpFadeWr;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;



//---Individual voice variables
//----triangle(s)
fix16 triHz[2];
fix16 triTune[2];
static filter_1p_lo triLevSlew[2];
static fract32 triLevel[2];
phasor triOsc[2];

//---Output containers, etc.
//-----container variables for easier out processing
//-------containers for voices
static fract32 triHolder[2];
//--------audio output bus (for mix out of all voices)
static fract32 outBus = 0;

//--SVF filter (mix_filt.c)
//---need slews to smooth out parameter changes (otherwise, 'steppy')
static fract32 myFilt = 0;
static filter_1p_lo myFiltSlew;
static fract32 myFQ = 0;
static filter_1p_lo myFQSlew;
static fract32 filtLPWrapper = 0;
filter_svf my_svf;


//time scaler in 3.12 fixed-point
static volatile s16 globalTimescale;
s32 calc_ms(s16 ticks) {
  //see lines.c for notes
  s32 ret = mult_fr1x32(ticks, globalTimescale);
  ret = add_fr1x32(ret, shr_fr1x32(globalTimescale, 2));
  ret = shr_fr1x32(ret, 12);
  return ret;
}



//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


void module_init(void) {
  u8 i; //from lines...
  // init module/param descriptor
  pDacsData = (echo_voicesData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "echo_voices");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  delayFadeN_init(&(lines), pDacsData->audioBuffer[i], LINES_BUF_FRAMES);

  //SVF-related slew parameter set-up
  //see lines.c
  filter_1p_lo_init( &(myFiltSlew), 0x3fffffff );
  filter_1p_lo_init( &(myFQSlew), 0x3fffffff );

  filter_ramp_init(&(lpFadeRd));
  filter_ramp_init(&(lpFadeWr));

  memset((void*)pDacsData->audioBuffer[i], 0, LINES_BUF_FRAMES * sizeof(fract32));

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );


  //delay stuff from lines
  param_setup( eParam_Timescale, 1 << 16);
  param_setup( eParam_Fade, 0x100000);

  param_setup( eParam_loop,	10000 << 16 );
  param_setup( eParam_rMul,	0x10000 );
  param_setup( eParam_rDiv,	0x10000 );
  param_setup( eParam_write,	FRACT32_MAX );
  param_setup( eParam_pre,	0 );
  param_setup( eParam_pos_write,	0 );
  param_setup( eParam_pos_read,		0 );

  param_setup( eParam_delay, 1000 << 16);

  param_setup( eParam_run_read, 1);
  param_setup( eParam_run_write, 1);


  param_setup( eParam_cvSlew0, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew1, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew2, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal0, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

  //Individual voices
  
  //triangle0
  param_setup( eParam_triHz0, 220 << 16 );
  param_setup( eParam_triTune0, FIX16_ONE );
  filter_1p_lo_init( &(triLevSlew[0]), 0 );
  param_setup( eParam_triLevSlew0, PARAM_SLEW_DEFAULT );
  //param_setup( eParam_triLevel0, PARAM_AMP_MAX >> 2 );
  //want complete silence on start...
  //trigger with midi velocity (MIDINOTE)
  param_setup( eParam_triLevel0, 0x00000000);
  phasor_init( &triOsc[0]);

  //triangle1
  param_setup( eParam_triHz1, 220 << 16 );
  param_setup( eParam_triTune1, FIX16_ONE );
  filter_1p_lo_init( &(triLevSlew[1]), 0 );
  param_setup( eParam_triLevSlew1, PARAM_SLEW_DEFAULT );
  //param_setup( eParam_triLevel1, PARAM_AMP_MAX >> 2 );
  //want complete silence on start...
  //trigger with midi velocity (MIDINOTE)
  param_setup( eParam_triLevel1, 0x00000000);
  phasor_init( &triOsc[1] );

  //SVF filter-related (slew is set previously... towards top)
  param_setup(eParam_myFilt, PARAM_CUT_DEFAULT);
  param_setup(eParam_myFQ, PARAM_RQ_DEFAULT);
  filter_svf_init(&my_svf);

}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}

//// FIXME:
/* for now, stagger DAC channels across consecutive audio frames
   better method might be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - dac_update writes to 4x16 volatile buffer
*/

void module_process_frame(void) {
  static fract32 tmpDel, tmpSvf;
  
  // Update one of the CV outputs
  if(filter_1p_sync(&(cvSlew[cvChan]))) { ;; } else {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    cv_update(cvChan, cvVal[cvChan]);
  }

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }

  //'smoothes' level adjustments through lp filter
  triLevel[0] = filter_1p_lo_next( &(triLevSlew[0]) );
  triLevel[1] = filter_1p_lo_next( &(triLevSlew[1]) );



  //defines triVoice, AND initializes
  //NOTE: 15 is the Radix...
  //NOTE: variables set-up without array... can be tricky
  //      BUT think it might be better to do it this way
  fract32 triVoice0 =  shl_fr1x32(triangle_polyblep(phasor_next(&triOsc[0])), 15);
  fract32 triVoice1 =  shl_fr1x32(triangle_polyblep(phasor_next(&triOsc[1])), 15);


  //voice holdes and  outBus for mix output of both tri voices
  //--first zero 
  triHolder[0] = 0;
  triHolder[1] = 0;
  
  outBus = 0;

  //--then set up voices
  triHolder[0] = mult_fr1x32x32(triVoice0, triLevel[0]);
  triHolder[1] = mult_fr1x32x32(triVoice1, triLevel[1]);
  
  //---sends each voice to a separate out
  //---NOTE: you want to do it in this order
  //        (prevents audio-bleed for first two outs)
  out[0] = triHolder[0];
  out[1] = triHolder[1];
  
  //---set up out bus
  outBus = add_fr1x32(outBus, triHolder[0]);
  outBus = add_fr1x32(outBus, triHolder[1]);

  //---delay fits in here
  u8 i;
  tmpDel = 0;
  in_del = 0;
  out_del = 0;
  
  //----send to delay in
  //----INCOMPLETE!!
  in_del = add_fr1x32(in_del, outBus); 

  //----process
  lines.fadeRd = filter_ramp_next(&(lpFadeRd));
  if(fadeTargetRd == 0) {
    lines.fadeRd = FR32_MAX - lines.fadeRd;
  }

  tmpDel = delayFadeN_next( &(lines), in_del);

  //----get from delay out
  out_del = add_fr1x32(out_del, tmpDel);

  //SVF
  //----set cutoff and res with lp filter smoothing
  filter_svf_set_coeff( &(my_svf), filter_1p_lo_next(&(myFiltSlew))); 
  
  filter_svf_set_rq( &(my_svf), filter_1p_lo_next(&(myFQSlew)));

  //----send outBus through filter
  filtLPWrapper = filter_svf_lpf_next(&my_svf, out_del);

  //---sends mix out final two outs
  out[2] = filtLPWrapper;
  out[3] = filtLPWrapper;

}

//this function from lines param_set.c
static u8 start_fade_rd(u8 id) {
  u8 newTarget, oldTarget;
  //  u8 ret;

  if(lpFadeRd.sync) {
    // not fading right now, so pick different target and start crossfade
    oldTarget = fadeTargetRd;
    newTarget = !oldTarget;
    //copy all tap parameters to target
    buffer_tapN_copy( &(lines.tapRd[oldTarget]),
	              &(lines.tapRd[newTarget]) );
    fadeTargetRd = newTarget;
    //start the fade
    filter_ramp_start(&(lpFadeRd));
    return 1;
  } else {
    // fade is in progress
    return 0;
  }
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {
  
  case eParam_delay : //i think fade for fading between to delay lines...
    if( start_fade_rd(0) || start_fade_rd(1) ) {
      delayFadeN_set_delay_ms(&(lines),
		              calc_ms(trunc_fr1x32(v)),
			      fadeTargetRd);
    }
    break;

  case eParam_loop :
    delayFadeN_set_loop_ms(&(lines),
		           calc_ms(trunc_fr1x32(v)));
    break;

  case eParam_pos_write :
    delayFadeN_set_pos_write_ms(&(lines),
	                        calc_ms(trunc_fr1x32(v)));
    break;

  case eParam_pos_read :
    if (start_fade_rd ) {
      delayFadeN_set_pos_read_ms(&(lines),
		                 calc_ms(trunc_fr1x32(v)),
				 fadeTargetRd);
    }
    break;

  case eParam_run_write :
    delayFadeN_set_run_write(&(lines), v);
    break;
  case eParam_run_read :
    delayFadeN_set_run_read(&(lines), v);
    break;
  case eParam_rMul :
    delayFadeN_set_mul(&(lines), v >> 16);
    break;
  case eParam_rDiv :
    delayFadeN_set_div(&(lines), v >> 16);
    break;
  case eParam_write :
    delayFadeN_set_write(&(lines), v > 0);
    break;
  case eParam_pre :
    if(v == FR32_MAX) {
       //negative == full
       delayFadeN_set_pre(&(lines), -1);
    } else {
       delayFadeN_set_pre(&(lines), v);
    }
    break;

  //cv and other vanilla params
  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), v);
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), v);
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), v);
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v);
    break;

  case eParam_cvSlew0 :
    filter_1p_lo_set_slew(&(cvSlew[0]), v);
    break;
  case eParam_cvSlew1 :
    filter_1p_lo_set_slew(&(cvSlew[1]), v);
    break;
  case eParam_cvSlew2 :
    filter_1p_lo_set_slew(&(cvSlew[2]), v);
    break;
  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;

    //triangle0 params
  case eParam_triHz0 :
    triHz[0] = v;
    triOsc[0].freq = fix16_mul_fract(triTune[0], triHz[0]);
    break;
  case eParam_triTune0 :
    triTune[0] = v;
    triOsc[0].freq = fix16_mul_fract(triTune[0], triHz[0]);
    break;
  case eParam_triLevel0 :
    filter_1p_lo_in( &(triLevSlew[0]), v);
    break;
  case eParam_triLevSlew0 :
    filter_1p_lo_set_slew( &(triLevSlew[0]), v);
    break;

    //triangle1 params
  case eParam_triHz1 :
    triHz[1] = v;
    triOsc[1].freq = fix16_mul_fract(triTune[1], triHz[1]);
    break;
  case eParam_triTune1 :
    triTune[1] = v;
    triOsc[1].freq = fix16_mul_fract(triTune[1], triHz[1]);
    break;
  case eParam_triLevel1 :
    filter_1p_lo_in( &(triLevSlew[1]), v);
    break;
  case eParam_triLevSlew1 :
    filter_1p_lo_set_slew( &(triLevSlew[1]), v);
    break;

    //SVF filter controls
    //Basically, if you want slew (you do)
    //need to set these both up as hooks to 
    //controls, and then actually set_coeff, etc
    //in module_process_frame() based on these values...
  case eParam_myFiltSlew :
    filter_1p_lo_set_slew( &(myFiltSlew), v);
    break;
  case eParam_myFQSlew :
    filter_1p_lo_set_slew( &(myFQSlew), v);
    break;
  case eParam_myFilt :
    myFiltSlew.x = v;
    //WORKS! but no slew...
    //filter_svf_set_coeff(&my_svf, v);
    //DOES NOT WORK...
    //filter_svf_set_coeff(&my_svf, filter_1p_lo_in( &(myFiltSlew), v));
    break;
  case eParam_myFQ :
    myFQSlew.x = v << 14;
    //WORKS! but no slew...
    //filter_svf_set_rq(&my_svf, v << 14);
    //DOES NOT WORK...
    //filter_svf_set_rq(&my_svf, filter_1p_lo_in( &(myFQSlew), v << 14));
    break;
  
  //fade times
  case eParam_Fade :
    if (v > PARAM_FADE_MIN) { //max fade 5s
      filter_ramp_set_inc(&(lpFadeRd), v);
    }
    break;
  case eParam_Timescale :
    globalTimescale = v >> 6;
    break;

    //default case...
  default:
    break;
  }
}
