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

/// copied from rawsc/osc.c to table_voice directory
//included as separate header file
#include "osc.h"


// data structure of external memory
typedef struct _table_voiceData {
	ModuleData super;
	ParamData mParamData[eParamNumParams];
} table_voiceData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
table_voiceData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;


//table (rawsc/osc.c)
const fract32 sine_table[1024] = {
#include "sine_table_inc.c"
};

//phase is phasor(?) 
//phi is ?
static u32 phase[NUM_OSCS];
static u32 phi[NUM_OSCS];
fract32 amp[NUM_OSCS];

//Individual voice variables

//sine
fix16 sinHz[NUM_OSCS];
fix16 sinTune[NUM_OSCS];
static filter_1p_lo sinLevSlew[NUM_OSCS];
static fract32 sinLevel[NUM_OSCS];
//phasor sinOsc;



//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
	gModuleData->paramData[id].value = v;
	module_set_param(id, v);
}


void module_init(void) {
	// init module/param descriptor
	pDacsData = (table_voiceData*)SDRAM_ADDRESS;

	gModuleData = &(pDacsData->super);
	strcpy(gModuleData->name, "table_voice");

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
	//sine
	//param_setup( eParam_sinHz0, 220 << 16 );
	param_setup( eParam_sinHz0, FIX16_ONE );
  param_setup( eParam_sinTune0, FIX16_ONE );

	//leave below params alone for now (should work)
	filter_1p_lo_init( &(sinLevSlew[0]), 0 );
	param_setup( eParam_sinLevSlew0, PARAM_SLEW_DEFAULT );
	param_setup( eParam_sinLevel0, PARAM_AMP_MAX >> 2 );
	//phasor_init( &sinOsc );

	//triangle (left for reference)
	//param_setup( eParam_triHz, 220 << 16 );
  //param_setup( eParam_triTune, FIX16_ONE );
	//filter_1p_lo_init( &(triLevSlew), 0 );
	//param_setup( eParam_triLevSlew, PARAM_SLEW_DEFAULT );
	//param_setup( eParam_triLevel, PARAM_AMP_MAX >> 2 );
	//phasor_init( &triOsc );

}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
	return eParamNumParams;
}


////osc.c functions
/* may be better to create an entry for osc.c and osc.h
	 in aleph/dsp, but just want to get working first
	 before try something more complex
	 */

void osc_set_phase(u16 idx, u32 val) {
	phase[idx] = val;
}

void osc_set_phi(u16 idx, u32 val) {
	phi[idx] = val;
}

//coarse (Hz) 
void osc_set_phi_upper(u16 idx, fract32 val) {
	u32 phi_ = (phi[idx] & 0x0000ffff) | (val & 0x7fff0000);
	phi[idx] = phi_;
}

//fine (Tune)
void osc_set_phi_lower(u16 idx, fract32 val) {
	u32 phi_ = (phi[idx] & 0x7fff0000) | ((val >> 15) & 0x0000ffff);
	phi[idx] = phi_;
}

//amp... trust me you need this
void osc_set_amp(u16 idx, fract32 val) {
  amp[idx] = val;
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
	sinLevel[0] = filter_1p_lo_next( &(sinLevSlew[0]) );

	//for reference
	//triLevel = filter_1p_lo_next( &(triLevSlew) );

	// -> from osc.c
	fract32 val;
  u32 idxA;
	u32 idxB;
	fract32 mulA;
	fract32 mulB;
	fract32 waveA;
	fract32 waveB;


	// phase is unsigned 32b
	// allow overflow
	phase[0] += phi[0];

	// lookup index
	// shift left for 10-bit index
	idxA = phase[0] >> 22;
	idxB = (idxA + 1) & 1023;

	// use bottom 22 bits for interpolation
	// shift back to [0, 7fffffff]
	mulB = (fract32) ((phase[0] & 0x3fffff) << 9);
	mulA = sub_fr1x32(0x7fffffff, mulB);

	waveA = shr_fr1x32(sine_table[idxA], 1);
	waveB = shr_fr1x32(sine_table[idxB], 1);

	// lookup, scale, and attenuate
  val = add_fr1x32(mult_fr1x32x32(waveA, mulA),	mult_fr1x32x32(waveB, mulB));

	// shift down to 24b
	val >>= 8;




	//for reference
	//fract32 triVoice =  shl_fr1x32(triangle_polyblep(phasor_next(&triOsc)), 15);


	//sends each voice to a separate out
	//val would be like sinVoice... sort of
	out[0] = mult_fr1x32x32(val, sinLevel[0]);
	out[1] = 0;
	out[2] = 0;
	out[3] = 0;


	//for reference
	//out[3] = mult_fr1x32x32(triVoice, triLevel);

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

			//sine params
		case eParam_sinHz0 : // --> upper 15 bit coarse
			sinHz[0] = v;
      osc_set_phi_upper(0, v);
			//osc_set_phi_upper(&(sinHz[0]), v);
			//sinOsc.freq = fix16_mul_fract(sinTune, sinHz);
			break;
		case eParam_sinTune0 : // --> lower 16 bits fine
			sinTune[0] = v;
      osc_set_phi_lower(0, v);
      //osc_set_phi_lower(&(sinTune[0]), v);
			//sinOsc.freq = fix16_mul_fract(sinTune, sinHz);
			break;
			//no need to change these two (sinLevel, sinLevSlew)  
		case eParam_sinLevel0 :
			filter_1p_lo_in( &(sinLevSlew[0]), v);
			break;
		case eParam_sinLevSlew0 :
			filter_1p_lo_set_slew( &(sinLevSlew[0]), v);
			break;
			/*triangle params //for reference
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
				break; */

		default:
			break;
	}
}
