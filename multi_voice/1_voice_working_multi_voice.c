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
//#include "filter_svf.h"
#include "biquad.h"
#include "ricks_tricks.h"
#include "osc_polyblep.h"
//#include "env_adsr.h"

#include "module.h"

/// custom
#include "params.h"

// data structure of external memory
typedef struct _multi_voiceData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} multi_voiceData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
multi_voiceData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

//hpf dcBlocker;
//hpf dcBlocker2;
//hpf dcBlocker3;
phasor noteOsc;

//env_adsr noteEnv;
//env_adsr filterEnv;

//Individual voice variables
fix16 sawHz;
fix16 sawTune;
static filter_1p_lo sawLevSlew;
static fract32 sawLevel;



//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

//filter_svf my_svf;
//filter_svf my_svf2;
//filter_svf my_svf3;

void module_init(void) {
  // init module/param descriptor
  pDacsData = (multi_voiceData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "multi_voice");

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
  param_setup( eParam_sawHz, 220 << 16 );
  param_setup( eParam_sawTune, FIX16_ONE );
  filter_1p_lo_init( &(sawLevSlew), 0);
  param_setup( eParam_sawLevSlew, PARAM_SLEW_DEFAULT);
  param_setup( eParam_sawLevel, PARAM_AMP_MAX >> 2);
  //param_setup( eParam_noteTrigger, 0);

  //env_adsr_init(&noteEnv);
  //noteEnv.attackTime = FR32_MAX;
  /* noteEnv.decayTime = SLEW_1S; */
  //noteEnv.sustainLevel = FR32_MAX;

  //env_adsr_init(&filterEnv);
  //filterEnv.attackTime = SLEW_1S;
  //filterEnv.decayTime = SLEW_1S;
  //filterEnv.sustainLevel = FR32_MAX - FR32_MAX / 3 ;
  phasor_init (&noteOsc);

  /*
  #ifndef ARCH_BFIN
  int i;
  fract32 slewTest=FR32_MAX;
  FILE* myFile;
  myFile=fopen("testOutput.csv", "w");
  for(i=FR32_MIN;
      i < FR32_MAX - (FR32_MAX >> 10);
      i+=(FR32_MAX >> 10)) {
    fprintf(myFile, "%d %d\n",
	    i,
	    soft_clip_asym(FR32_MAX - (FR32_MAX / 4), 0, i));
  }
  fclose(myFile);
  myFile=fopen("float2frac_test.csv", "w");
  for(i=FR32_MIN;
      i < FR32_MAX - (FR32_MAX >> 10);
      i+=(FR32_MAX >> 10)) {
    fprintf(myFile, "%d %d\n",
	    i,
	    soft_clip_asym(FR32_MAX - (FR32_MAX / 4), 0, i));
  }

  #endif
  */

  /*
  filter_svf_init(&my_svf);
  filter_svf_set_coeff(&my_svf, FR32_MAX / 12);
  filter_svf_set_rq(&my_svf, 0);

  filter_svf_init(&my_svf2);
  filter_svf_set_coeff(&my_svf2, FR32_MAX / 12);
  filter_svf_set_rq(&my_svf2, 0);

  filter_svf_init(&my_svf3);
  filter_svf_set_coeff(&my_svf3, FR32_MAX / 12);
  filter_svf_set_rq(&my_svf3, 0);

  hpf_init(&dcBlocker);
  hpf_init(&dcBlocker2);
  hpf_init(&dcBlocker3);
  */
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
  sawLevel = filter_1p_lo_next( &(sawLevSlew) );

  //defines sawVoice, AND initializes
  //NOTE: 15 is the Radix...
  fract32 sawVoice = shl_fr1x32(saw_polyblep(phasor_next(&noteOsc), noteOsc.freq),
			       15);

  //some of the rick's tricks oscs have fewer parameters...
  //but we are using osc_polyblep
  //
  //fract32 squareVoice = shl_fr1x32(square_polyblep(phasor_next(&noteOsc), noteOsc.freq), 15);
  //fract32 sineVoice = shl_fr1x32(sine_polyblep(phasor_next(&noteOsc)), 15);
  //fract32 triVoiceNote =  shl_fr1x32(triangle_polyblep(phasor_next(&noteOsc)), 15);


  //fract32 myEnv = env_adsr_next(&noteEnv);
  //fract32 filterEnvVal = env_adsr_next(&filterEnv);
  //out[0] = mult_fr1x32x32(sawVoice,myEnv);
  //i had some trouble triggering the note, plus triggered
  //note is not how eurorack oscs work...
  //this is what you can do to just have it ring out w/out envelope
  out[0] = mult_fr1x32x32(sawVoice, sawLevel);
  //out[1] = squareVoice;
  //out[2] = sinVoice;
  //out[3] = triVoice;


  //filter_svf_set_coeff(&my_svf, shr_fr1x32(filterEnvVal, 0));
  //filter_svf_set_coeff(&my_svf2, shr_fr1x32(filterEnvVal, 0));
  //filter_svf_set_coeff(&my_svf3, shr_fr1x32(filterEnvVal, 0));

  //out[1] = dc_block(&dcBlocker, filter_svf_lpf_next(&my_svf, out[0]));
  //out[2] = dc_block(&dcBlocker2, filter_svf_softclip_lpf_next(&my_svf2, out[0]));
  //out[3] = dc_block(&dcBlocker3, filter_svf_softclip_asym_lpf_next(&my_svf3, out[0]));
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

  /*
  case eParam_noteTrigger :
    if (v > 0) {
      env_adsr_press(&noteEnv);
      env_adsr_press(&filterEnv);
    }
    else {
      env_adsr_release(&noteEnv);
      env_adsr_release(&filterEnv);
    }
    break;
    */

  case eParam_sawHz :
    sawHz = v;
    noteOsc.freq = fix16_mul_fract(sawTune, sawHz);
    break;
  case eParam_sawTune :
    sawTune = v;
    noteOsc.freq = fix16_mul_fract(sawTune, sawHz);
    break;
 case eParam_sawLevel :
    filter_1p_lo_in( &(sawLevSlew), v);
    break;
 case eParam_sawLevSlew :
    filter_1p_lo_set_slew( &(sawLevSlew), v);
    break;

  default:
    break;
  }
}
