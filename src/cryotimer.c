/*
 * cryotimer.c
 *
 *  Created on: Apr 25, 2019
 *      Author: andyhsu
 */

#include "cryotimer.h"
#include <stdbool.h>
#include <em_cryotimer.h>
#include "gpio.h"

// Global variables
extern volatile int scheduler_event;

//******************************************************************************************************
// Initialization of cryotimer struct and interrupts every one second
// inputs: none
// outputs: none
//******************************************************************************************************
void cryoInit()
{
	// struct initialization to interrupt every one second, prescale divider is 1, ULFRCO oscillator
	CRYOTIMER_Init_TypeDef build;
	build.enable = true;
	build.debugRun = false;
	build.em4Wakeup = true;
	build.osc = cryotimerOscULFRCO,
	build.presc = cryotimerPresc_1;
    build.period = cryotimerPeriod_1k;
    CRYOTIMER_Init(&build);

    // Enable interrupts
	NVIC_EnableIRQ(CRYOTIMER_IRQn);
    CRYOTIMER->IFC = CRYOTIMER_IFC_PERIOD;
    CRYOTIMER->IEN = CRYOTIMER_IEN_PERIOD;
	CRYOTIMER_Enable(true);
}

//******************************************************************************************************
// brief: cryotimer interrupt handler routine interrupts every one second
// inputs: scheduler event, cryotimer IF
// outputs: new scheudler event
//******************************************************************************************************
void CRYOTIMER_IRQHandler()
{
	CORE_ATOMIC_IRQ_DISABLE();
	int flag = CRYOTIMER->IF & CRYOTIMER->IEN;
	CRYOTIMER->IFC = flag;
	if(flag & CRYOTIMER_IF_PERIOD)
	{
		scheduler_event |= FLAG6;
	}
	CORE_ATOMIC_IRQ_ENABLE();

}
