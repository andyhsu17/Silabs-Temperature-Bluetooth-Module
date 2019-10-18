/*
 * dma.h
 *
 *  Created on: Apr 9, 2019
 *      Author: andyhsu
 */

#ifndef SRC_DMA_H_
#define SRC_DMA_H_
#include "em_ldma.h"

// Defines
#define BYTECOUNT 		1
#define LOOPCOUNT		7
#define TX_DMA_CHANNEL 	1
#define RX_DMA_CHANNEL 	0
#define DMA_TX_DONE		0x2
#define DMAPriority		3
#define STDNUM			0
#define STDBYTES		4

// Global Variables
extern volatile LDMA_Descriptor_t ldmaTXDescriptor;
extern volatile LDMA_TransferCfg_t ldmaTXConfig;
extern volatile LDMA_Descriptor_t ldmaRXDescriptor;
extern volatile LDMA_TransferCfg_t ldmaRXConfig;

// Function Declarations
void dma_init();
void LDMA_IRQHandler();
void dma_transmissionInit(char * transm_data, int totalBytes);
void dma_receiveInit(char * recdata, int totalBytes);

#endif /* SRC_DMA_H_ */
