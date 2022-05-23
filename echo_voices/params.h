#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define PARAM_HZ_MIN OSC_FREQ_MIN
#define PARAM_HZ_MAX OSC_FREQ_MIN
#define PARAM_HZ_DEFAULT (OSC_FREQ_MIN *16)

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX FR32_MAX
#define PARAM_DAC_RADIX 16

#define PARAM_RATE_MIN 0x2000 // 1/8
#define PARAM_RATE_MAX 0x80000 //8
#define PARAM_RATE_RADIX 3

#define SMOOTH_FREQ_MIN 0x2000
#define SMOOTH_FREQ_MAX 0x400000
#define SMOOTH_FREQ_RADIX 7

#define SLEW_SECONDS_MIN 0x2000
#define SLEW_SECONDS_MAX 0x400000
#define SLEW_SECONDS_RADIX 7

//FADE STUFF
#define PARAM_FADE_MIN 0x2000
#define PARAM_FADE_MAX 0x2000000
#define PARAM_FADE_RADIX 12

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

//for the svf filter cutoff (myFilt) and rq (myFq)
#define PARAM_CUT_MAX 0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC //~8K

#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0

//TIME
#define PARAM_SECONDS_MAX 0x7FFFFFFF
#define PARAM_SECONDS_RADIX 32


// enumerate parameters
enum params {
  //global time scaler param
  eParam_Timescale,

  //xfade time
  eParam_Fade,

  //vanilla
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
  //00.Tri0
  eParam_triHz0,
  eParam_triTune0,
  eParam_triLevSlew0,
  eParam_triLevel0,
  //01.Tri1
  eParam_triHz1,
  eParam_triTune1,
  eParam_triLevSlew1,
  eParam_triLevel1,
  
  //SVF filter
  eParam_myFiltSlew,
  eParam_myFQSlew,
  eParam_myFilt,
  eParam_myFQ,

  // line (delay)
  eParam_pos_read,
  eParam_pos_write,
  eParam_run_read,
  eParam_run_write,
  eParam_loop,
  eParam_rMul,
  eParam_rDiv,
  eParam_pre,
  eParam_write,
  eParam_delay,
  
  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
