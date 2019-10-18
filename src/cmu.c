//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void cmu_init(void){

	// Enable the HFPERCLK for desired peripherals such as ADC
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFXO);
	CMU_ClockEnable(cmuClock_HFPER, true);

	// By default, LFRCO is enabled
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

	// Route LF clock to the LF clock tree
	if (Letimer0_EM == EM4) {
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// Use ULFRCO in EM3 for LETIMER0
	}
	else
	{
		CMU_OscillatorEnable(cmuOsc_ULFRCO, false, false);
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// Disable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);	// routing clock to LFA
	}

	/* HFXO start on entering em0/em1*/
	CMU_HFXOAutostartEnable(true, false, false);

	CMU_ClockEnable(cmuClock_LFA, true);
	CMU_ClockEnable(cmuClock_CORELE, true);

	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	/* I2c clock tree enable */
	CMU_ClockEnable(cmuClock_HFPER, true); 	// enable high freq peripheral clock
	CMU_ClockEnable(cmuClock_I2C0, true); 	// enable high I2C clock

	/*LEUART clock tree */
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	CMU_ClockEnable(cmuClock_LEUART0, true);

	// DMA Clock Tree
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO);
	CMU_ClockEnable(cmuClock_LDMA, true);

	//Cryotimer clock tree
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_CRYOTIMER, cmuSelect_ULFRCO);	// Use Cryotimer with ULFRCO
	CMU_ClockEnable(cmuClock_CRYOTIMER, true);


}

