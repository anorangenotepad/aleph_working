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

	//triangle0
	strcpy(desc[eParam_triHz0].label, "triHz0");
	desc[eParam_triHz0].type = eParamTypeNote;
	desc[eParam_triHz0].min = OSC_HZ_MIN;
	desc[eParam_triHz0].max = OSC_HZ_MAX;
	desc[eParam_triHz0].radix = OSC_HZ_RADIX;

	strcpy(desc[eParam_triTune0].label, "triTune0");
	desc[eParam_triTune0].type = eParamTypeFix;
	desc[eParam_triTune0].min = RATIO_MIN;
	desc[eParam_triTune0].max = RATIO_MAX;
	desc[eParam_triTune0].radix = RATIO_RADIX;

	strcpy(desc[eParam_triLevSlew0].label, "triLevSlew0");
	desc[eParam_triLevSlew0].type = eParamTypeIntegrator;
	desc[eParam_triLevSlew0].min = 0x00000000;
	desc[eParam_triLevSlew0].max = PARAM_SLEW_MAX;
	desc[eParam_triLevSlew0].radix = 16; //doesn't matter

	strcpy(desc[eParam_triLevel0].label, "triLevel0");
	desc[eParam_triLevel0].type = eParamTypeAmp;
	desc[eParam_triLevel0].min = 0x00000000;
	desc[eParam_triLevel0].max = PARAM_AMP_MAX;
	desc[eParam_triLevel0].radix = 16; // doesn't matter

	//triangle1
	strcpy(desc[eParam_triHz1].label, "triHz1");
	desc[eParam_triHz1].type = eParamTypeNote;
	desc[eParam_triHz1].min = OSC_HZ_MIN;
	desc[eParam_triHz1].max = OSC_HZ_MAX;
	desc[eParam_triHz1].radix = OSC_HZ_RADIX;

	strcpy(desc[eParam_triTune1].label, "triTune1");
	desc[eParam_triTune1].type = eParamTypeFix;
	desc[eParam_triTune1].min = RATIO_MIN;
	desc[eParam_triTune1].max = RATIO_MAX;
	desc[eParam_triTune1].radix = RATIO_RADIX;

	strcpy(desc[eParam_triLevSlew1].label, "triLevSlew1");
	desc[eParam_triLevSlew1].type = eParamTypeIntegrator;
	desc[eParam_triLevSlew1].min = 0x00000000;
	desc[eParam_triLevSlew1].max = PARAM_SLEW_MAX;
	desc[eParam_triLevSlew1].radix = 16; //doesn't matter

	strcpy(desc[eParam_triLevel1].label, "triLevel1");
	desc[eParam_triLevel1].type = eParamTypeAmp;
	desc[eParam_triLevel1].min = 0x00000000;
	desc[eParam_triLevel1].max = PARAM_AMP_MAX;
	desc[eParam_triLevel1].radix = 16; // doesn't matter

	//delay
	strcpy(desc[eParam_delay].label, "delay");
	desc[eParam_delay].type = eParamTypeFix; 
	desc[eParam_delay].min = 0x00000000; 
	desc[eParam_delay].max = PARAM_SECONDS_MAX; 
	desc[eParam_delay].radix = PARAM_SECONDS_RADIX;

	strcpy(desc[eParam_loop].label, "loop");
	desc[eParam_loop].type = eParamTypeFix; 
	desc[eParam_loop].min = 0x00000000; 
	desc[eParam_loop].max = PARAM_SECONDS_MAX; 
	desc[eParam_loop].radix = PARAM_SECONDS_RADIX;

	strcpy(desc[eParam_rMul].label, "rMul");
	desc[eParam_rMul].type = eParamTypeFix; 
	desc[eParam_rMul].min = 0x00010000; 
	desc[eParam_rMul].max = 0x00080000; 
	desc[eParam_rMul].radix = 32;

	strcpy(desc[eParam_rDiv].label, "rDiv");
	desc[eParam_rDiv].type = eParamTypeFix; 
	desc[eParam_rDiv].min = 0x00010000; 
	desc[eParam_rDiv].max = 0x00080000; 
	desc[eParam_rDiv].radix = 32;

	strcpy(desc[eParam_write].label, "write");
	desc[eParam_write].type = eParamTypeAmp; 
	desc[eParam_write].min = 0x00000000; 
	desc[eParam_write].max = FR32_MAX;
	desc[eParam_write].radix = 1;

	strcpy(desc[eParam_pre].label, "pre");
	desc[eParam_pre].type = eParamTypeAmp; 
	desc[eParam_pre].min = 0x00000000; 
	desc[eParam_pre].max = FR32_MAX;
	desc[eParam_pre].radix = 1;

	strcpy(desc[eParam_pos_write].label, "pos_write");
	desc[eParam_pos_write].type = eParamTypeFix; 
	desc[eParam_pos_write].min = 0x00000000; 
	desc[eParam_pos_write].max = PARAM_SECONDS_MAX; 
	desc[eParam_pos_write].radix = PARAM_SECONDS_RADIX;

	strcpy(desc[eParam_pos_read].label, "pos_read");
	desc[eParam_pos_read].type = eParamTypeFix; 
	desc[eParam_pos_read].min = 0x00000000; 
	desc[eParam_pos_read].max = PARAM_SECONDS_MAX; 
	desc[eParam_pos_read].radix = PARAM_SECONDS_RADIX;

	strcpy(desc[eParam_run_write].label, "run_write");
	desc[eParam_run_write].type = eParamTypeBool; 
	desc[eParam_run_write].min = 0;
	desc[eParam_run_write].max = 1; 
	desc[eParam_run_write].radix = 2; 

	strcpy(desc[eParam_run_read].label, "run_read");
	desc[eParam_run_read].type = eParamTypeBool; 
	desc[eParam_run_read].min = 0;
	desc[eParam_run_read].max = 1; 
	desc[eParam_run_read].radix = 2;

	// SVF PARAMS START FROM HERE
	// these come from mix_filt
	strcpy(desc[eParam_myFiltSlew].label, "myFiltSlew");
	desc[eParam_myFiltSlew].type = eParamTypeIntegrator;
	desc[eParam_myFiltSlew].min = 0x00000000;
	desc[eParam_myFiltSlew].max = PARAM_SLEW_MAX;
	desc[eParam_myFiltSlew].radix = 16; //doesn't matter

	strcpy(desc[eParam_myFQSlew].label, "myFQSlew");
	desc[eParam_myFQSlew].type = eParamTypeIntegrator;
	desc[eParam_myFQSlew].min = 0x00000000;
	desc[eParam_myFQSlew].max = PARAM_SLEW_MAX;
	desc[eParam_myFQSlew].radix = 16; //doesn't matter

	strcpy(desc[eParam_myFilt].label, "myFilt");
	desc[eParam_myFilt].type = eParamTypeSvfFreq;
	desc[eParam_myFilt].min = 0x00000000;
	desc[eParam_myFilt].max = PARAM_CUT_MAX;
	desc[eParam_myFilt].radix = 1;

	strcpy(desc[eParam_myFQ].label, "myFQ");
	desc[eParam_myFQ].type = eParamTypeFix;
	desc[eParam_myFQ].min = PARAM_RQ_MIN;
	desc[eParam_myFQ].max = PARAM_RQ_MAX;
	desc[eParam_myFQ].radix = 2;

	//fade (delay)
	strcpy(desc[eParam_Fade].label, "fade");
	desc[eParam_Fade].type = eParamTypeFix;
	desc[eParam_Fade].min = PARAM_FADE_MIN;
	desc[eParam_Fade].max = PARAM_FADE_MAX;
	desc[eParam_Fade].radix = PARAM_FADE_RADIX;

	//timescale (delay)

	strcpy(desc[eParam_Timescale].label, "timescale");
	desc[eParam_Timescale].type = eParamTypeFix;
	desc[eParam_Timescale].min = 0;
	desc[eParam_Timescale].max = 0x00040000;
	desc[eParam_Timescale].radix = 4;

}

// EOF
