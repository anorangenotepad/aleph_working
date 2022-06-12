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

  //INDIVIDUAL VOICE PARAMS START FROM HERE
  //based on aleph/modules_block/rawsc
  //changes based on rawsc/params.c
  //sine0

  //from rawsc, you REALLY need this...
  strcpy(desc[eParam_amp0].label, "amp0");
  desc[eParam_amp0].type = eParamTypeAmp;
  desc[eParam_amp0].min = 0;
  desc[eParam_amp0].max = 0x7fffffff;
  desc[eParam_amp0].radix = 16;
  
  //freqCoarse in rawsc
  strcpy(desc[eParam_sinHz0].label, "sinHz");
  //desc[eParam_sinHz].type = eParamTypeNote;
  desc[eParam_sinHz0].type = eParamTypeFix;
  desc[eParam_sinHz0].min = 0;
  desc[eParam_sinHz0].max = 0x7fffffff;
  desc[eParam_sinHz0].radix = 16;

  //freqFine
  strcpy(desc[eParam_sinTune0].label, "sinTune");
  desc[eParam_sinTune0].type = eParamTypeFix;
  desc[eParam_sinTune0].min = 0;
  desc[eParam_sinTune0].max = 0x7fffffff;
  desc[eParam_sinTune0].radix = 16;

  //no equivalent (slew for amp)
  strcpy(desc[eParam_sinLevSlew0].label, "sinLevSlew");
  desc[eParam_sinLevSlew0].type = eParamTypeIntegrator;
  desc[eParam_sinLevSlew0].min = 0x00000000;
  desc[eParam_sinLevSlew0].max = PARAM_SLEW_MAX;
  desc[eParam_sinLevSlew0].radix = 16; //doesn't matter

  //amp
  strcpy(desc[eParam_sinLevel0].label, "sinLevel");
  desc[eParam_sinLevel0].type = eParamTypeAmp;
  desc[eParam_sinLevel0].min = 0x00000000;
  desc[eParam_sinLevel0].max = PARAM_AMP_MAX;
  desc[eParam_sinLevel0].radix = 16; // doesn't matter

  
}

// EOF
