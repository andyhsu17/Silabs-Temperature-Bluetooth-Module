/*
 * dma.c
 *
 *  Created on: Apr 9, 2019
 *      Author: andyhsu
 */
/**************************************************************************//**
 * @main_series1_PG1_EFR.c
 * @brief Demonstrates USART1 as SPI master.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
*  This code is originally Silicon Labs and copy righted by Silicon Labs� in 2018 and Silicon Labs� grants
 * permission to anyone to use the software for any purpose, including commercial applications, and to alter
 * it, and redistribute it freely subject that the origins is not miss represented, altered source version must
 * be plainly marked, and this notice cannot be altered or removed from any source distribution.
 *
 * Names of routines have been changed to confirm to the naming convention of the application developer.
 *
 *
	void dma_transmissionInit(char * transm_data, int totalBytes)
 * *
 ******************************************************************************/
#include "dma.h"
#include "em_ldma.h"
#include "em_core.h"
#include "main.h"

// DMA TX and RX Descriptor and Transfer Configurations
volatile LDMA_Descriptor_t ldmaTXDescriptor;
volatile LDMA_TransferCfg_t ldmaTXConfig;
volatile LDMA_Descriptor_t ldmaRXDescriptor;
volatile LDMA_TransferCfg_t ldmaRXConfig;
extern volatile char recdata[];


//******************************************************************************************************
// DMA Initialization for receive and transmit at irq priority of 3
// Inputs: none
// Outputs: none
//******************************************************************************************************
void dma_init()
{
	LDMA_Init_t build;
	build.ldmaInitCtrlNumFixed = STDNUM;					 // Round Robin Priority
	build.ldmaInitCtrlSyncPrsClrEn = STDNUM;		         /* No PRS Synctrig clear enable*/
	build.ldmaInitCtrlSyncPrsSetEn = STDNUM;              /* No PRS Synctrig set enable. */
	build.ldmaInitIrqPriority      = DMAPriority;    /* IRQ priority level 3.       */
	LDMA_Init(&build);
	NVIC_EnableIRQ(LDMA_IRQn);
	dma_receiveInit(recdata, STDBYTES);												// Initialize descriptor and configure structures
	LDMA_StartTransfer(RX_DMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);			// start transfer
}


//******************************************************************************************************
// Initializes the DMA descriptor and config structures for DMA memory to peripheral TX transmission
// Inputs: array of temperature characters in ascii, total number of bytes to transfer
// Outputs: none
//******************************************************************************************************
void dma_transmissionInit(char * transm_data, int totalBytes)
{
	ldmaTXDescriptor = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(transm_data,  // Source: transm_data array, Destination: LEUART0->TXDATA, Bytes to send: 7
				&(LEUART0->TXDATA),
				totalBytes);
	ldmaTXConfig = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_TXBL);              // Setting the transfer to trigger once there is room in the LEUART0->TXBL buffer
}

//******************************************************************************************************
// Initializes the DMA descriptor and config structures for DMA peripheral to memory RX transmission
// Inputs: array to store receive data, total number of bytes to receive
// Outputs: none
//******************************************************************************************************
void dma_receiveInit(char * recdata, int totalBytes)
{
	ldmaRXDescriptor = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(LEUART0->RXDATA),  // Source: LEUART0->RXDATA, Destination: recdata, Bytes to send: 4
					recdata,
					totalBytes);
	ldmaRXConfig = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_RXDATAV);              // Setting the transfer to trigger once there is room in the LEUART0->RXDATA buffer
}

//******************************************************************************************************
// DMA IRQ Handler
// inputs: DMA tx done interrupt flag
// outputs: none
//******************************************************************************************************
void LDMA_IRQHandler()
{
	CORE_ATOMIC_IRQ_DISABLE();
	while(LEUART0->SYNCBUSY);
	int flag = LDMA->IF & LDMA->IEN;
	LDMA->IFC = flag;
	// DMA transfer done, enable txc and disable TXDMA wakeup
	if(flag & DMA_TX_DONE)
	{
		LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU;	// disable TX dma wakeup so it can sleep
		while(LEUART0->SYNCBUSY);
		LEUART0->IEN |= LEUART_IEN_TXC;			// enable TXC to let you know the last byte has been sent
	}
    CORE_ATOMIC_IRQ_ENABLE();

}


