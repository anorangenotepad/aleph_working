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

// audio
#include "filter_1p.h"
#include "filter_svf.h"
#include "biquad.h"
#include "ricks_tricks.h"
#include "osc_polyblep.h"

#include "module.h"


/// custom
#include "params.h"

// data structure of external memory
typedef struct _echo_voicesData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} echo_voicesData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
echo_voicesData* pDacsData;

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
static fract32 myFilt = 0;
static fract32 myFQ = 0;
static fract32 filtLPWrapper = 0;
filter_svf my_svf;






//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


void module_init(void) {
  // init module/param descriptor
  pDacsData = (echo_voicesData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "echo_voices");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );

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
  param_setup( eParam_triLevel0, PARAM_AMP_MAX >> 2 );
  phasor_init( &triOsc[0]);

  //triangle1
  param_setup( eParam_triHz1, 220 << 16 );
  param_setup( eParam_triTune1, FIX16_ONE );
  filter_1p_lo_init( &(triLevSlew[1]), 0 );
  param_setup( eParam_triLevSlew1, PARAM_SLEW_DEFAULT );
  param_setup( eParam_triLevel1, PARAM_AMP_MAX >> 2 );
  phasor_init( &triOsc[1] );

  //SVF filter-related
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



  //defines sawVoice, AND initializes
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

  //send outBus through filter
  filtLPWrapper = filter_svf_lpf_next(&my_svf, outBus);

  //---sends mix out final two outs
  out[2] = filtLPWrapper;
  out[3] = filtLPWrapper;

}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {

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
  case eParam_myFilt :
    filter_svf_set_coeff(&my_svf, v);
    break;
  case eParam_myFQ :
    filter_svf_set_rq(&my_svf, v << 14);
    break;

    //default case...
  default:
    break;
  }
}
