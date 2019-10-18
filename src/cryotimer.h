/*
 * cryotimer.h
 *
 *  Created on: Apr 25, 2019
 *      Author: andyhsu
 */
#include <stdint.h>
#ifndef SRC_CRYOTIMER_H_
#define SRC_CRYOTIMER_H_
// defines

// function declarations
void cryoInit();
void CRYOTIMER_IRQHandler();

// global variables
extern volatile int scheduler_event;

#endif /* SRC_CRYOTIMER_H_ */
