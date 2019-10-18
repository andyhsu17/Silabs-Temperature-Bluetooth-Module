/*
 * leuart.h
 *
 *  Created on: Feb 26, 2019
 *      Author: andyhsu
 */

#ifndef SRC_LEUART_H_
#define SRC_LEUART_H_
#include <stdio.h>
#include <stdint.h>
//********************************
 // defines
///*******************************
#define LEUARTRXPORT 		gpioPortD
#define	LEUARTRXPIN			11
#define	LEUARTTXPORT		gpioPortD
#define	LEUARTTXPIN			10
#define HM10BAUD			9600
#define STDFREQ				0
#define hash				0x23
#define question			0x3F
//*********************************
//************ functions************
// * *******************************
void leuart_init();
void LEUART0_IRQHandler();

//**********************************
 // global variables
 //**********************************
extern volatile unsigned int ind;


#endif /* SRC_LEUART_H_ */
