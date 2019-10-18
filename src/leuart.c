/*
 * leuart.c
 *
 *  Created on: Feb 26, 2019
 *      Author: andyhsu
 */


#include "leuart.h"
#include "em_leuart.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "em_core.h"
#include "main.h"
#include "sleep_routines.h"
#include "dma.h"

volatile unsigned int ind = 0;
extern volatile int scheduler_event;
//extern volatile char recdata[];
extern volatile LDMA_Descriptor_t ldmaRXDescriptor;
extern volatile LDMA_TransferCfg_t ldmaRXConfig;
//**********************************************************************************
// initialization routine for LEUART - startframe ? endframe #
// inputs: none
// outputs: none
//**********************************************************************************
void leuart_init()
{
	LEUART_Init_TypeDef build;
	build.baudrate = HM10BAUD;
	build.refFreq = STDFREQ;
	build.enable = leuartEnable;
	build.databits = leuartDatabits8;
	build.parity = leuartNoParity;
	build.stopbits = leuartStopbits1;

	LEUART_Init(LEUART0, &build);
	while(LEUART0->SYNCBUSY);													// wait for high and low frequency clock synchronization

	LEUART0->ROUTEPEN = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
	LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_TXLOC_LOC18 | LEUART_ROUTELOC0_RXLOC_LOC18;

	LEUART0->IFC = LEUART_IFC_SIGF;

	//LEUART0->CTRL |= LEUART_CTRL_LOOPBK;										// enable loopback for debug
	LEUART0->IEN = LEUART_IEN_SIGF;
	LEUART0->CMD = LEUART_CMD_RXBLOCKEN;										// block RXDATA from being transferred from RXDATA shift into RXDATA buffer
	while(LEUART0->SYNCBUSY);													// wait for high and low frequency clock synchronization

	LEUART0->CTRL = LEUART_CTRL_SFUBRX | LEUART_CTRL_RXDMAWU;					// on start frame interrupt, clears RXBLOCK automatically (must reenable after)
																						// also DMA controller wakeup on RX or TX of LEUART
	while(LEUART0->SYNCBUSY);													// wait for high and low frequency clock synchronization
	LEUART0->STARTFRAME = question;												// "?" set as startframe
	LEUART0->SIGFRAME = hash;													// "#" set as endframe
	LEUART_Enable(LEUART0, leuartEnable);
	NVIC_EnableIRQ(LEUART0_IRQn);
}

//********************************************************
// brief: leuart interrupt service routine
// inputs: scheduler event, RX DMA descriptor, RX DMA cfg structure
// outputs: new scheduler event
// ********************************************************
void LEUART0_IRQHandler()
{
	CORE_ATOMIC_IRQ_DISABLE();
	unsigned int flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = flag;
//	if(flag & LEUART_IF_STARTF)		// flag handle for received correct startframe, will disable RXBLOCK automatically
//	{
//		LEUART0->IEN |= LEUART_IEN_RXDATAV;
//	}
//	if(flag & LEUART_IF_RXDATAV)	// flag handle for RXDATA valid
//	{
//		recdata[ind] = LEUART0->RXDATA;
//		ind++;
//	}
	if(flag & LEUART_IF_SIGF)		// flag handle for sigframe (end of transmission)
	{
		//ind = 0;
		//LEUART0->IEN &= ~(LEUART_IEN_RXDATAV);			// disable RXDATAV and sigf

		LEUART0->CMD = LEUART_CMD_RXBLOCKEN;			// enable blockRXDATA again
		while(LEUART0->SYNCBUSY);
		LDMA_StartTransfer(RX_DMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);			// enable LDMA
		scheduler_event |= FLAG5;
	}


	// TXBL: 1 when ready for transmit for new byte
	// TXC:  1 after transmission is complete and txbuffer is empty
	if((flag & LEUART_IF_TXBL) || (flag & LEUART_IF_TXC))
	{
		LEUART0->IEN &= ~(LEUART_IEN_TXBL | LEUART_IEN_TXC);
		Sleep_UnBlock_Mode(EM3);
	}

	CORE_ATOMIC_IRQ_ENABLE();
}
