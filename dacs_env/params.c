#include <string.h>

#include "module.h"
#include "params.h" 

extern void fill_param_desc(ParamDesc* desc) {
  strcpy(desc[eParam_cvVal0].label, "cv0");
  desc[eParam_cvVal0].type = eParamTypeFix;
  desc[eParam_cvVal0].min = 0x00000000;
  desc[eParam_cvVal0].max = PARAM_DAC_MAX;
  desc[eParam_cvVal0].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal1].label, "cv1");
  desc[eParam_cvVal1].type = eParamTypeFix;
  desc[eParam_cvVal1].min = 0x00000000;
  desc[eParam_cvVal1].max = PARAM_DAC_MAX;
  desc[eParam_cvVal1].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal2].label, "cv2");
  desc[eParam_cvVal2].type = eParamTypeFix;
  desc[eParam_cvVal2].min = 0x00000000;
  desc[eParam_cvVal2].max = PARAM_DAC_MAX;
  desc[eParam_cvVal2].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal3].label, "cv3");
  desc[eParam_cvVal3].type = eParamTypeFix;
  desc[eParam_cvVal3].min = 0x00000000;
  desc[eParam_cvVal3].max = PARAM_DAC_MAX;
  desc[eParam_cvVal3].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvSlew0].label, "cvSlew0");
  desc[eParam_cvSlew0].type = eParamTypeIntegrator;
  desc[eParam_cvSlew0].min = 0x00000000;
  desc[eParam_cvSlew0].max = 0x7fffffff;
  desc[eParam_cvSlew0].radix = 32;

  strcpy(desc[eParam_cvSlew1].label, "cvSlew1");
  desc[eParam_cvSlew1].type = eParamTypeIntegrator;
  desc[eParam_cvSlew1].min = 0x00000000;
  desc[eParam_cvSlew1].max = 0x7fffffff;
  desc[eParam_cvSlew1].radix = 32;

  strcpy(desc[eParam_cvSlew2].label, "cvSlew2");
  desc[eParam_cvSlew2].type = eParamTypeIntegrator;
  desc[eParam_cvSlew2].min = 0x00000000;
  desc[eParam_cvSlew2].max = 0x7fffffff;
  desc[eParam_cvSlew2].radix = 32;

  strcpy(desc[eParam_cvSlew3].label, "cvSlew3");
  desc[eParam_cvSlew3].type = eParamTypeIntegrator;
  desc[eParam_cvSlew3].min = 0x00000000;
  desc[eParam_cvSlew3].max = 0x7fffffff;
  desc[eParam_cvSlew3].radix = 32;

  //bool switch
  strcpy(desc[eParam_cvSwitch].label, "cvSwitch");
  desc[eParam_cvSwitch].type = eParamTypeBool;
  desc[eParam_cvSwitch].min = 0;
  desc[eParam_cvSwitch].max = 1;
  desc[eParam_cvSwitch].radix = 2;

  //rise and fall
  strcpy(desc[eParam_riseSetting].label, "riseSetting");
  desc[eParam_riseSetting].type = eParamTypeFix;
  desc[eParam_riseSetting].min = 0x00000000;
  desc[eParam_riseSetting].max = PARAM_CV_VAL_DEFAULT;
  desc[eParam_riseSetting].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_fallSetting].label, "fallSetting");
  desc[eParam_fallSetting].type = eParamTypeFix;
  desc[eParam_fallSetting].min = 0x00000000;
  desc[eParam_fallSetting].max = PARAM_CV_VAL_DEFAULT;
  desc[eParam_fallSetting].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_envAdjust].label, "envAdjust");
  desc[eParam_envAdjust].type = eParamTypeFix;
  desc[eParam_envAdjust].min = 0x00000000;
  desc[eParam_envAdjust].max = PARAM_CV_VAL_DEFAULT;
  desc[eParam_envAdjust].radix = PARAM_DAC_RADIX;

}

// EOF
