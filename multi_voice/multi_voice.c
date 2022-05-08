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



//Individual voice variables
//saw
fix16 sawHz;
fix16 sawTune;
static filter_1p_lo sawLevSlew;
static fract32 sawLevel;
phasor sawOsc;

//square
fix16 sqrHz;
fix16 sqrTune;
static filter_1p_lo sqrLevSlew;
static fract32 sqrLevel;
phasor sqrOsc;

//sine
fix16 sinHz;
fix16 sinTune;
static filter_1p_lo sinLevSlew;
static fract32 sinLevel;
phasor sinOsc;

//triangle
fix16 triHz;
fix16 triTune;
static filter_1p_lo triLevSlew;
static fract32 triLevel;
phasor triOsc;


//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


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
  //saw
  param_setup( eParam_sawHz, 220 << 16 );
  param_setup( eParam_sawTune, FIX16_ONE );
  filter_1p_lo_init( &(sawLevSlew), 0 );
  param_setup( eParam_sawLevSlew, PARAM_SLEW_DEFAULT );
  param_setup( eParam_sawLevel, PARAM_AMP_MAX >> 2 );
  phasor_init( &sawOsc );

  //square
  param_setup( eParam_sqrHz, 220 << 16 );
  param_setup( eParam_sqrTune, FIX16_ONE );
  filter_1p_lo_init( &(sqrLevSlew), 0 );
  param_setup( eParam_sqrLevSlew, PARAM_SLEW_DEFAULT );
  param_setup( eParam_sqrLevel, PARAM_AMP_MAX >> 2 );
  phasor_init( &sqrOsc );

  //sine
  param_setup( eParam_sinHz, 220 << 16 );
  param_setup( eParam_sinTune, FIX16_ONE );
  filter_1p_lo_init( &(sinLevSlew), 0 );
  param_setup( eParam_sinLevSlew, PARAM_SLEW_DEFAULT );
  param_setup( eParam_sinLevel, PARAM_AMP_MAX >> 2 );
  phasor_init( &sinOsc );

  //triangle
  param_setup( eParam_triHz, 220 << 16 );
  param_setup( eParam_triTune, FIX16_ONE );
  filter_1p_lo_init( &(triLevSlew), 0 );
  param_setup( eParam_triLevSlew, PARAM_SLEW_DEFAULT );
  param_setup( eParam_triLevel, PARAM_AMP_MAX >> 2 );
  phasor_init( &triOsc );

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
  sqrLevel = filter_1p_lo_next( &(sqrLevSlew) );
  sinLevel = filter_1p_lo_next( &(sinLevSlew) );
  triLevel = filter_1p_lo_next( &(triLevSlew) );



  //defines sawVoice, AND initializes
  //NOTE: 15 is the Radix...
  fract32 sawVoice = shl_fr1x32(saw_polyblep(phasor_next(&sawOsc), sawOsc.freq), 15);

  //some of the rick's tricks oscs have fewer parameters...
  //but we are using osc_polyblep
  //
  fract32 sqrVoice = shl_fr1x32(square_polyblep(phasor_next(&sqrOsc), sqrOsc.freq), 15);
  fract32 sinVoice = shl_fr1x32(sine_polyblep(phasor_next(&sinOsc)), 15);
  fract32 triVoice =  shl_fr1x32(triangle_polyblep(phasor_next(&triOsc)), 15);


  //sends each voice to a separate out
  out[0] = mult_fr1x32x32(sawVoice, sawLevel);
  out[1] = mult_fr1x32x32(sqrVoice, sqrLevel);
  out[2] = mult_fr1x32x32(sinVoice, sinLevel);
  out[3] = mult_fr1x32x32(triVoice, triLevel);

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

  //saw params
  //level and slewLevel based on mix.c
  case eParam_sawHz :
    sawHz = v;
    sawOsc.freq = fix16_mul_fract(sawTune, sawHz);
    break;
  case eParam_sawTune :
    sawTune = v;
    sawOsc.freq = fix16_mul_fract(sawTune, sawHz);
    break;
  case eParam_sawLevel :
    filter_1p_lo_in( &(sawLevSlew), v);
    break;
  case eParam_sawLevSlew :
    filter_1p_lo_set_slew( &(sawLevSlew), v);
    break;
  //square params
  case eParam_sqrHz :
    sqrHz = v;
    sqrOsc.freq = fix16_mul_fract(sqrTune, sqrHz);
    break;
  case eParam_sqrTune :
    sqrTune = v;
    sqrOsc.freq = fix16_mul_fract(sqrTune, sqrHz);
    break;
  case eParam_sqrLevel :
    filter_1p_lo_in( &(sqrLevSlew), v);
    break;
  case eParam_sqrLevSlew :
    filter_1p_lo_set_slew( &(sqrLevSlew), v);
    break;
  //sine params
  case eParam_sinHz :
    sinHz = v;
    sinOsc.freq = fix16_mul_fract(sinTune, sinHz);
    break;
  case eParam_sinTune :
    sinTune = v;
    sinOsc.freq = fix16_mul_fract(sinTune, sinHz);
    break;
  case eParam_sinLevel :
    filter_1p_lo_in( &(sinLevSlew), v);
    break;
  case eParam_sinLevSlew :
    filter_1p_lo_set_slew( &(sinLevSlew), v);
    break;
  //triangle params
  case eParam_triHz :
    triHz = v;
    triOsc.freq = fix16_mul_fract(triTune, triHz);
    break;
  case eParam_triTune :
    triTune = v;
    triOsc.freq = fix16_mul_fract(triTune, triHz);
    break;
  case eParam_triLevel :
    filter_1p_lo_in( &(triLevSlew), v);
    break;
  case eParam_triLevSlew :
    filter_1p_lo_set_slew( &(triLevSlew), v);
    break;

  default:
    break;
  }
}
