#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX FR32_MAX
#define PARAM_DAC_RADIX 16
#define OSC_HZ_MIN 0x00010000      // 1 hz
#define OSC_HZ_MAX 0x40000000    // 16384 hz
#define OSC_HZ_RADIX 15

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

//not used...
#define PARAM_AMP_0 (FRACT32_MAX)
#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

//these are used, and come from mix/params.h
#define PARAM_AMP_MAX  0x7fffffff
#define PARAM_CV_MAX   0x7fffffff
#define PARAM_SLEW_MAX 0x7fffffff

#define PARAM_SLEW_DEFAULT 0x7ffecccc

//this was part of original monosynth
#define PARAM_CV_SLEW_DEFAULT 0x77000000

// cv output
#define PARAM_CV_VAL_DEFAULT PARAM_AMP_6

// enumerate parameters
enum params {
  eParam_cvVal0,
  eParam_cvVal1,
  eParam_cvVal2,
  eParam_cvVal3,
  eParam_cvSlew0,
  eParam_cvSlew1,
  eParam_cvSlew2,
  eParam_cvSlew3,

  //eParam_noteTrigger,
  //
  //Params for individual voices grouped 
  //together
  //
  //00.Saw
  eParam_sawHz,
  eParam_sawTune,
  eParam_sawLevSlew,
  eParam_sawLevel,
  //01.Square
  eParam_sqrHz,
  eParam_sqrTune,
  eParam_sqrLevSlew,
  eParam_sqrLevel,
  //02.sine
  eParam_sinHz,
  eParam_sinTune,
  eParam_sinLevSlew,
  eParam_sinLevel,
  //03.Tri
  eParam_triHz,
  eParam_triTune,
  eParam_triLevSlew,
  eParam_triLevel,

  
  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
