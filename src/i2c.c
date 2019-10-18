/*
 * i2c.c
 *
 *  Created on: Feb 7, 2019
 *      Author: andyhsu
 */
#include "i2c.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_device.h"
#include "main.h"
#include "em_gpio.h"
#include "sleep_routines.h"
#include "stdint.h"
#include "gpio.h"
#include <stdlib.h>
#include <stdbool.h>
//*************************************************************************************************
// Function to convert celsius to fahrenheight
// input: celius temperature in float
// output: fahrenheight temperature in float
//************************************************************************************************
float CtoF(float temp)
{
	return (temp * 1.8) + 32;
}

//*************************************************************************************************
// Function to convert farenheight to celsius
// input: fahrenheight temperature in float
// output: celsius in float
//************************************************************************************************
float FtoC(float temp)
{
	return (temp - 32) / 1.8;
}
//*************************************************************************************************
// Function to convert temperature value to ASCII string for UART transmission to bluetooth module
// input: temperature in float and pointer to array of char
// output: void
//************************************************************************************************
void temptoASCII(float tempC, char * array, bool Celsius)
{
	if(tempC < 0){
		char temp = '-';
		array[0] = temp;
		tempC *= -1;
	}
	else{
		array[0] = '+';
	}
	unsigned int working;
	unsigned int temporary_int;
	temporary_int = (int)(tempC * 10);
	if(temporary_int >= 1000){
	working = temporary_int / 1000;
	working += 0x30;
	array[1] = (char)working;
	}
	else{
		array[1] = ' ';
	}
	if(temporary_int >= 100){
	temporary_int %= 1000;
	working = temporary_int / 100;
	working += 0x30;
	array[2] = (char)working;
	}
	else{
		array[2] = ' ';
	}
	if(temporary_int >= 10){
		temporary_int %= 100;
		working = temporary_int / 10;
		working += 0x30;
		array[3] = (char)working;
	}
	array[4] = '.';
	temporary_int %= 10;
	temporary_int += 0x30;
	array[5] = (char)temporary_int;
	if(Celsius == true) array[6] = 'C';
	else array[6] = 'F';
	return;
}




//********************************************************************
// Function to read I2C bus sequence with No Hold Master configuration
// inputs: temperature sensor port, temp sensor pin, si7021 address
// outputs: data returned by i2c query from si7021
//*********************************************************************
uint16_t I2C0_ReadTemp()
{
	I2C0->CMD = I2C_CMD_ABORT;
	I2C0->CMD = I2C_CMD_CLEARPC;
	GPIO_PinOutSet(tempsensor_port, tempsensor_pin);

	I2C0->CMD = I2C_CMD_START;						// send start bit
	I2C0->TXDATA = (Si7021_address << 1) | write_bit; // write slave address to slave, or with write command
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear interrupt flag register

	I2C0->TXDATA = TempHoldMaster;				// Temperature measure command no hold
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear acknowledge interrupt

	I2C0->CMD = I2C_CMD_START;						// send start repeat bit
	I2C0->TXDATA = (Si7021_address << 1) | read_bit;	// give device address to slave telling it to read
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear interrupt flag register

	while((I2C0->IF & I2C_IF_RXDATAV) == 0); // wait until status is receive valid (clock stretch)

	uint16_t tempdata1 = I2C0->RXDATA;				// read 8 most significant bits
	tempdata1 = tempdata1 << 8;						// shift left 8 bits

	I2C0->CMD = I2C_CMD_ACK;						// send ack bit
	while((I2C0->IF & I2C_IF_RXDATAV) == 0); // wait until status is receive valid (clock stretch)

	uint16_t tempdata2 = I2C0->RXDATA;				// receive 8 least significant bits
	I2C0->CMD = I2C_CMD_NACK;
	I2C0->CMD = I2C_CMD_STOP;
	tempdata1 = tempdata1 | tempdata2;
	return tempdata1;
}

//*******************************************************
// brief: function to convert buffer value to degrees C
// input: uint16_t ADC val returned by si7021 adc
// output: float degrees in temperature celsius
//*******************************************************
float convertTemp(uint16_t val)
{
	float tempC = (175.72 * val / 65536) - 46.85;
	return tempC;
}

//********************************************
// read function to read from I2c sensor
// input: si7021 port and pin, si7021 address
// output: receive data from si7021
//**********************************************
uint8_t I2C0_Read()
{
	I2C0->CMD = I2C_CMD_ABORT;
	I2C0->CMD = I2C_CMD_CLEARPC;
	GPIO_PinOutSet(tempsensor_port, tempsensor_pin);

		// 0 - write
		// 1 - read
		/* Read from slave*/
	I2C0->CMD = I2C_CMD_START;						// send start bit
	I2C0->TXDATA = (Si7021_address << 1) | write_bit; // write slave address to slave, or with write command
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear interrupt flag register

	I2C0->TXDATA = read_7021_register_address; 		// send read device register address;
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear interrupt flag register
	I2C0->CMD = I2C_CMD_START;						// send start repeat bit
	I2C0->TXDATA = (Si7021_address << 1) | read_bit;	// give device address to slave telling it to read
	while((I2C0->IF & I2C_IF_ACK) == 0); 			// wait for acknowledge from slave
	I2C0->IFC = I2C_IF_ACK;							// clear interrupt flag register
	while((I2C0->IF & I2C_IF_RXDATAV) == 0);
	I2C0->CMD = I2C_CMD_NACK;						// send NAK
	I2C0->CMD = I2C_CMD_STOP;						// send stop bit
	return I2C0->RXDATA;
}

//******************************************************************************
// brief: write for configuration of 12 bit resolution configuration
// input: uint8_t data to write to si7021, si7021 adress
// output: none
//******************************************************************************
void I2C0_Write(uint8_t data)
{
		GPIO_PinOutSet(tempsensor_port, tempsensor_pin);
		I2C0->CMD = I2C_CMD_ABORT;
	    I2C0->CMD = I2C_CMD_CLEARPC;

	/* write to register*/
		I2C0->CMD = I2C_CMD_START;					// send start bit
		I2C0->TXDATA = (Si7021_address << 1) | write_bit; 	// write slave address to slave, or with write command
		while((I2C0->IF & I2C_IF_ACK) == 0); 		// wait for acknowledge from slave
		I2C0->IFC = I2C_IF_ACK;						// clear acknowledge interrupt bit
		I2C0->TXDATA = write_7021_register_address;	// send write device register address
		while((I2C0->IF & I2C_IF_ACK) == 0); 		// wait for acknowledge from slave
		I2C0->IFC = I2C_IF_ACK;						// clear interrupt flag register
		I2C0->TXDATA = data;		// send register 1 command for 12 bit resolution
		I2C0->CMD = I2C_CMD_ACK;					// send NAK
		I2C0->CMD = I2C_CMD_STOP;					// send stop bit
}


//********************************************************
// brief: initialization routine for i2c routing SCL and SDA, etc
// inputs: none
// outputs: none
//*********************************************************
void i2c_init(void)
{
	I2C_Init_TypeDef build;
	build.enable = true;
	build.freq = I2C_FREQ_FAST_MAX;
	build.master = true;
	build.refFreq = 0;
	build.clhr = i2cClockHLRAsymetric;

	/* Deassert control line assert the other */
	for(int i = 0; i < 9; i++)
	{
		GPIO_PinOutClear(SCLport, SCLpin);
		GPIO_PinOutSet(SCLport, SCLpin);
	}
	if(I2C0->STATE & I2C_STATE_BUSY)
	{
		I2C0->CMD = I2C_CMD_ABORT;
	}
		/* Route I2C pins to I2C Peripheral */
	I2C0->ROUTELOC0 = I2C_ROUTELOC0_SCLLOC_LOC15 | I2C_ROUTELOC0_SDALOC_LOC15;	// route location from I2C0 peripheral and GPIO pins
	I2C0->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;						// enable SCL and SDA
	NVIC_ClearPendingIRQ(I2C0_IRQn);
//	/* clock low time out  */
//	I2C0->CTRL = _I2C_CTRL_CLTO_1024PCC;						// set to 1024 prescaled clock cycles
//	I2C0->IFC = I2C_IFC_CLTO;									// clear clock low timeout interrupt
//	I2C0->IEN = I2C_IEN_CLTO;									// enable clock low timeout interrupt
//	/* bit idle time out */
//	I2C0->CTRL |= _I2C_CTRL_BITO_160PCC;						// enable 160 prescaled clock cycles
//	I2C0->IFC = I2C_IFC_BITO;									// clear flag before enabling interrupts
//	I2C0->IEN |= I2C_IEN_BITO;									// enable interrupts for bus idle timeout
	/* interrupt set up */
	I2C0->IFC = I2C_IFC_ACK | I2C_IFC_NACK;						// clear all interrupts
	I2C0->IEN |= I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_RXDATAV;	// enable interrupts from acknowledge bit and NACK
	Sleep_Block_Mode(EM3);										// make sure only stays in EM0 or EM1
	for(int i = 0; i < 1000000; i++);							// delay to stabilize power lines
		/* initialization */
	I2C_Init(I2C0, &build);
	//NVIC_EnableIRQ(I2C0_IRQn);								// enable i2c interrupts
}

//******************************************************************************
// brief: write for configuration of 12 bit resolution configuration
// input: none
// output: none
//******************************************************************************
void I2C0_IRQHandler(void)
{
	uint32_t flag = I2C0->IF;				// read flag
	I2C0->IFC = flag;						// clear immediately
}

