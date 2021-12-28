/* dacs_env.c
   aleph-bfin

   based on dacs.c (aleph)
   and 20 Objects Ardcore AC25_VCAREnvelope 
   by Darwin Grosse
   https://github.com/darwingrosse/ArdCore-Code/blob/master/20%20Objects/AC25_VCAREnvelope/AC25_VCAREnvelope.ino

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

// audio
#include "filter_1p.h"
#include "module.h"

/// custom
#include "params.h"

// data structure of external memory
typedef struct _dacs_envData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} dacs_envData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
dacs_envData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

static fract32 cvSwitch;

static fract32 currValue;
static u8 envState = 0;

//rise value is a wrapper 
//for riseSetting and fallSetting which
//can be adjuster through bees
static fract32 riseValue;
static fract32 fallValue;

static fract32 riseAdjusted;
static fract32 fallAdjusted;

//hooks for bees mapping/control
static fract32 riseSetting;
static fract32 fallSetting;
//adds a value specified with bees to both rise and fall
static fract32 envAdjust;


//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

void module_init(void) {
  // init module/param descriptor
  pDacsData = (dacs_envData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "dacs");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );

  param_setup( eParam_cvSlew0, 0 );
  param_setup( eParam_cvSlew1, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew2, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal0, 0 );
  param_setup( eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

  param_setup(eParam_cvSwitch, 0 );

  param_setup(eParam_riseSetting, 0);
  param_setup(eParam_fallSetting, 0);

  param_setup(eParam_envAdjust, 0);

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

//functions like loop() in ardcore/arduino
void module_process_frame(void) {

  //all code below adpated/copied from Ardcore envelope
  u8 doStart = 0;

  if (cvSwitch == 1)
  {
    cvSwitch = 0;
    doStart = 1;
  }

  if (doStart == 1)
  {
    doStart = 0;
    envState = 1;
  }

  if (envState == 1)
  {
    currValue = add_fr1x32(currValue, riseValue);
  }
  else if (envState == 2)
  {
    currValue = sub_fr1x32(currValue, fallValue);
  }

  if (currValue > PARAM_CV_VAL_DEFAULT)
  {
    currValue = PARAM_CV_VAL_DEFAULT;
    envState = 2;
  }

  if (currValue < 0)
  {
    currValue = 0;
    envState = 0;
  }

  //send value to CV out 0
  cv_update(cvChan, currValue);

  //adds a manually set value to both rise and fall
  riseAdjusted = add_fr1x32(envAdjust, riseSetting);
  fallAdjusted = add_fr1x32(envAdjust, fallSetting);

  riseValue = PARAM_CV_VAL_DEFAULT / riseAdjusted;
  fallValue = PARAM_CV_VAL_DEFAULT / fallAdjusted;

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

  //bool param (essentially fills the role of clock in ardcore) 
  case eParam_cvSwitch :
    cvSwitch = v;
    break;
  //rise and fall settings (to be controled with encoders)
  case eParam_riseSetting :
    riseSetting = v;
    break;
  case eParam_fallSetting :
    fallSetting = v;
    break;
  //adds value to rise and fall equally
  case eParam_envAdjust :
    envAdjust = v;
    break;

  default:
    break;
  }
}
