/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "main.h"
#include "gpio.h"
#include "cmu.h"
#include "letimer.h"
#include "i2c.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "leuart.h"
#include "em_leuart.h"
#include <string.h>
#include "dma.h"
#include "em_ldma.h"
#include "cryotimer.h"
#include "capsense.h"

//const char volatile name[] = {0x73, 0x68, 0x6e, 0x61, 0x45, 0x4d, 0x41, 0x4e, 0x2b, 0x54, 0x41}; // "shnaEMAN+TA"
//volatile const char fart[] = {0x74, 0x72, 0x61, 0x66, 0x45, 0x4d, 0x41, 0x4e, 0x2b, 0x54, 0x41}; //"trafEMAN+TA"
//uint8_t volatile pointer = strlen(name);
volatile int scheduler_event = 0;
volatile char recdata[RECEIVE_SIZE] = {0, 0, 0, 0, 0};
extern volatile LDMA_Descriptor_t ldmaTXDescriptor;
extern volatile LDMA_TransferCfg_t ldmaTXConfig;
static volatile bool tempOn = true;
static volatile bool touchEvent = false;

int main(void)
{


  /* initialized variables */
	bool setCelsius = true;
	char transm_data[STDLEN];


	Sleep_Block_Mode(EM3); 	// set lowest energy mode for the system
	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
	CHIP_Init();

	  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
	EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
	EMU_DCDCInit(&dcdcInit);
	em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
	EMU_EM23Init(&em23Init);
	CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Initialize clocks */
	cmu_init();

  /* Initialize GPIO */
	gpio_init();

  /* Initialize i2c*/
    i2c_init();
    I2C_Enable(I2C0, true);

  /* Initialize LETIMER0 */
    letimer0_init();
    LETIMER_Enable(LETIMER0, true);

  /* LEUART0 initialization routine */
    leuart_init();

  /* Initialize DMA */
    dma_init();

    CAPSENSE_Init();

    /* Initialize cryotimer */
    cryoInit();

  // enable interrupts after chip has been configured
    CORE_ATOMIC_IRQ_ENABLE();


  while (1)
  {
	  // when no interrupt, go to sleep
	  if(scheduler_event == 0)
	  {
	  Enter_Sleep();
	  }

	  // servicing event for transmission of byte via TXBL or TXC
//	  if(scheduler_event & FLAG2)
//	  {
//
//////////**** Code to rename bluetooth module ***********************************************
////		  if(pointer > (sizeof(name) - 1))			// ensure that if pointer decrement overflows, it doesn't transmit (shouldn't need this)
////		  		{
////			  	  	scheduler_event &= ~FLAG2;
////		  			goto NEXT;
////		  		}
////
////		  		LEUART0->TXDATA = name[pointer--];
////************************************************************************************
//
//		  	  	scheduler_event &= ~FLAG2;			// done servicing interrupt
//		  	  	while(LEUART0->SYNCBUSY);			// wait until synchronization is done
//		  		LEUART0->IEN |= (LEUART_IEN_TXBL | LEUART_IEN_TXC);
//	  }
// NEXT:

	 // comp0 to power up temp sensor
	  if(scheduler_event & FLAG3)
	  {
		  GPIO_PinModeSet(tempsensor_port, tempsensor_pin, gpioModePushPull, on);
		  scheduler_event &= ~FLAG3;			// done servicing interrupt
		  while(LEUART0->SYNCBUSY);				// wait for high and low frequency clock synchronization
	  }

	  // comp1 to read from temp sensor and transmit temperature to bluetooth
	  if(scheduler_event & FLAG4)
	  {
		  Sleep_Block_Mode(EM3);

		  /* map EFM32 pins as SCL and SDA of si7021 (WiredAnd, not push and pull) */
		  GPIO_PinModeSet(SCLport, SCLpin, gpioModeWiredAnd, on);	// SCL enable and wiredand
		  GPIO_PinModeSet(SDAport, SDApin, gpioModeWiredAnd, on);	// SDA enable and wiredand

		  i2c_reset();		// toggle scl lines 9 times

		  uint16_t val = I2C0_ReadTemp();
		  float temp = convertTemp(val);
		  if(!setCelsius)		// set fahrenheight
		  {
			  temp = CtoF(temp);
		  }
		  temptoASCII(temp, transm_data, setCelsius);
		 /* now disable everything */
		  GPIO_PinModeSet(SCLport, SCLpin, gpioModeDisabled, off);	// SCL disable
		  GPIO_PinModeSet(SDAport, SDApin, gpioModeDisabled, off);	// SDA disable
		  GPIO_PinModeSet(tempsensor_port, tempsensor_pin, gpioModePushPull, off);	// sensor assert to 0
		  Sleep_UnBlock_Mode(EM3);
		  /* Send to bluetooth module via LEUART */
//		  for(int i = 0; i < strlen(transm_data); i++)// send ascii temperature to BLE
//		  {
//		  LEUART0->TXDATA = transm_data[i];
//		  LEUART0->IEN |= LEUART_IEN_TXC;	// enable TXC interrupt
//		  Enter_Sleep();
//		  scheduler_event &= ~FLAG2;		// done servicing TXBL interrupt
//		  }
		  dma_transmissionInit(transm_data, strlen(transm_data));
		  LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;
		  while(LEUART0->SYNCBUSY);
		  Sleep_Block_Mode(EM3);
		  LDMA_StartTransfer(TX_DMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);			// no loop, destination and source increment positive
		  scheduler_event &= ~FLAG4;		// done servicing comp1 interrupt
	  }
	  // sigframe: end of transmission
	  if(scheduler_event & FLAG5)
	  {
		  if((recdata[1] == 'd' || recdata[1] == 'D'))
		  {
			  if((recdata[2] == 'f') || (recdata[2] == 'F')) setCelsius = false;
			  if((recdata[2] == 'c') || (recdata[2] == 'c')) setCelsius = true;
		  }
		  scheduler_event &= ~FLAG5;										// done servicing sigframe interrupt
	  }

	  // cryotimer interrupt every 1 second for capsense
	  if(scheduler_event & FLAG6)
	  {
		  // initialize capsense channels
		  CAPSENSE_Sense();
		  if (CAPSENSE_getPressed(BUTTON0_CHANNEL)	// if channel 0 is touched
		                 && !CAPSENSE_getPressed(BUTTON1_CHANNEL))
		  {
			  if(!touchEvent)	// if a touch event didnt just happen
			  {
				  touchEvent = true;	// set touch event to true (a touch just occurred)
				  if(tempOn)
				  {
					  tempOn = false;
					  LETIMER_Enable(LETIMER0, false);
					  GPIO_PinOutClear(tempsensor_port, tempsensor_pin);
					  while(LEUART0->SYNCBUSY);				// wait for high and low frequency clock synchronization
					  GPIO_PinOutClear(SCLport, SCLpin);	// SCL disable
					  GPIO_PinOutClear(SDAport, SDApin);	// SDA disable
				  }
				  else
				  {
					  tempOn = true;
					  LETIMER0->CNT = 0;
					  LETIMER_Enable(LETIMER0, true);
					  GPIO_PinOutSet(tempsensor_port, tempsensor_pin);
					  while(LEUART0->SYNCBUSY);				// wait for high and low frequency clock synchronization
					  GPIO_PinOutSet(SCLport, SCLpin);	// SCL disable
					  GPIO_PinOutSet(SDAport, SDApin);	// SDA disable
				  }
			  }

		  }
		  else	// channel 0 is not touched during the 1 sec time interval
		  {
			  touchEvent= false;	// a touch event didnt just happen
		  }
		  scheduler_event &= ~FLAG6;
	  }

  }
}

