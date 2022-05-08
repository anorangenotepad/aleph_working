#include <string.h>

#include "module.h"
#include "params.h" 

void param_desc_aux (ParamDesc* desc,int param_idx,
		     char* desc_string, ParamType type,
		     s32 min, s32 max, u8 radix) {
  strcpy(desc[param_idx].label,
	 desc_string);
  desc[param_idx].type = type;
  desc[param_idx].min = min;
  desc[param_idx].max = max;
  desc[param_idx].radix = radix;
}

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

  //param_desc_aux(desc, eParam_noteTrigger, "buzzTrigger",
		 //eParamTypeFix, 0x00000000, 0x00010000,
		 //32);

  // INDIVIDUAL OSC PARAMS START FROM HERE
  //saw
  strcpy(desc[eParam_sawHz].label, "sawHz");
  desc[eParam_sawHz].type = eParamTypeNote;
  desc[eParam_sawHz].min = OSC_HZ_MIN;
  desc[eParam_sawHz].max = OSC_HZ_MAX;
  desc[eParam_sawHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_sawTune].label, "sawTune");
  desc[eParam_sawTune].type = eParamTypeFix;
  desc[eParam_sawTune].min = RATIO_MIN;
  desc[eParam_sawTune].max = RATIO_MAX;
  desc[eParam_sawTune].radix = RATIO_RADIX;

  strcpy(desc[eParam_sawLevSlew].label, "sawLevSlew");
  desc[eParam_sawLevSlew].type = eParamTypeIntegrator;
  desc[eParam_sawLevSlew].min = 0x00000000;
  desc[eParam_sawLevSlew].max = PARAM_SLEW_MAX;
  desc[eParam_sawLevSlew].radix = 16; //doesn't matter

  strcpy(desc[eParam_sawLevel].label, "sawLevel");
  desc[eParam_sawLevel].type = eParamTypeAmp;
  desc[eParam_sawLevel].min = 0x00000000;
  desc[eParam_sawLevel].max = PARAM_AMP_MAX;
  desc[eParam_sawLevel].radix = 16; // doesn't matter
  
  //square
  strcpy(desc[eParam_sqrHz].label, "sqrHz");
  desc[eParam_sqrHz].type = eParamTypeNote;
  desc[eParam_sqrHz].min = OSC_HZ_MIN;
  desc[eParam_sqrHz].max = OSC_HZ_MAX;
  desc[eParam_sqrHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_sqrTune].label, "sqrTune");
  desc[eParam_sqrTune].type = eParamTypeFix;
  desc[eParam_sqrTune].min = RATIO_MIN;
  desc[eParam_sqrTune].max = RATIO_MAX;
  desc[eParam_sqrTune].radix = RATIO_RADIX;

  strcpy(desc[eParam_sqrLevSlew].label, "sqrLevSlew");
  desc[eParam_sqrLevSlew].type = eParamTypeIntegrator;
  desc[eParam_sqrLevSlew].min = 0x00000000;
  desc[eParam_sqrLevSlew].max = PARAM_SLEW_MAX;
  desc[eParam_sqrLevSlew].radix = 16; //doesn't matter

  strcpy(desc[eParam_sqrLevel].label, "sqrLevel");
  desc[eParam_sqrLevel].type = eParamTypeAmp;
  desc[eParam_sqrLevel].min = 0x00000000;
  desc[eParam_sqrLevel].max = PARAM_AMP_MAX;
  desc[eParam_sqrLevel].radix = 16; // doesn't matter
  
  //sine
  strcpy(desc[eParam_sinHz].label, "sinHz");
  desc[eParam_sinHz].type = eParamTypeNote;
  desc[eParam_sinHz].min = OSC_HZ_MIN;
  desc[eParam_sinHz].max = OSC_HZ_MAX;
  desc[eParam_sinHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_sinTune].label, "sinTune");
  desc[eParam_sinTune].type = eParamTypeFix;
  desc[eParam_sinTune].min = RATIO_MIN;
  desc[eParam_sinTune].max = RATIO_MAX;
  desc[eParam_sinTune].radix = RATIO_RADIX;

  strcpy(desc[eParam_sinLevSlew].label, "sinLevSlew");
  desc[eParam_sinLevSlew].type = eParamTypeIntegrator;
  desc[eParam_sinLevSlew].min = 0x00000000;
  desc[eParam_sinLevSlew].max = PARAM_SLEW_MAX;
  desc[eParam_sinLevSlew].radix = 16; //doesn't matter

  strcpy(desc[eParam_sinLevel].label, "sinLevel");
  desc[eParam_sinLevel].type = eParamTypeAmp;
  desc[eParam_sinLevel].min = 0x00000000;
  desc[eParam_sinLevel].max = PARAM_AMP_MAX;
  desc[eParam_sinLevel].radix = 16; // doesn't matter

  //triangle
  strcpy(desc[eParam_triHz].label, "triHz");
  desc[eParam_triHz].type = eParamTypeNote;
  desc[eParam_triHz].min = OSC_HZ_MIN;
  desc[eParam_triHz].max = OSC_HZ_MAX;
  desc[eParam_triHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_triTune].label, "triTune");
  desc[eParam_triTune].type = eParamTypeFix;
  desc[eParam_triTune].min = RATIO_MIN;
  desc[eParam_triTune].max = RATIO_MAX;
  desc[eParam_triTune].radix = RATIO_RADIX;

  strcpy(desc[eParam_triLevSlew].label, "triLevSlew");
  desc[eParam_triLevSlew].type = eParamTypeIntegrator;
  desc[eParam_triLevSlew].min = 0x00000000;
  desc[eParam_triLevSlew].max = PARAM_SLEW_MAX;
  desc[eParam_triLevSlew].radix = 16; //doesn't matter

  strcpy(desc[eParam_triLevel].label, "triLevel");
  desc[eParam_triLevel].type = eParamTypeAmp;
  desc[eParam_triLevel].min = 0x00000000;
  desc[eParam_triLevel].max = PARAM_AMP_MAX;
  desc[eParam_triLevel].radix = 16; // doesn't matter

}

// EOF
