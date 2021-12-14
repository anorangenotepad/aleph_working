#ifndef _ALEPH_MIX_PARAMS_H_
#define _ALEPH_MIX_PARAMS_H_

#include "param_common.h"

// define some constants here for the param descriptor code
// here, all the parameter ranges are pretty simple.
#define PARAM_AMP_MAX 	0x7fffffff
#define PARAM_CV_MAX 	0x7fffffff
#define PARAM_SLEW_MAX 	0x7fffffff

// something pretty fast, but noticeable
#define PARAM_SLEW_DEFAULT 	0x7ffecccc

//for the svf filter cutoff (myFilt) and rq (myFQ)
#define PARAM_CUT_MAX  0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC //~8K

#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0

// enumerate parameters
// the order defined here must be matched in the descriptor !
enum params {
  // cv slew
  eParam_cvSlew0,
  eParam_cvSlew1,
  eParam_cvSlew2,
  eParam_cvSlew3,
  // adc multiplier slew
  eParam_adcSlew0,
  eParam_adcSlew1,
  eParam_adcSlew2,
  eParam_adcSlew3,
  // cv values
  eParam_cv0,
  eParam_cv1,
  eParam_cv2,
  eParam_cv3,
  // adc multiplier values
  eParam_adc0,
  eParam_adc1,
  eParam_adc2,
  eParam_adc3,
  //my filter
  eParam_myFilt,
  eParam_myFQ,

  eParamNumParams
};

// this is only defined by the descriptor helper program.
extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
