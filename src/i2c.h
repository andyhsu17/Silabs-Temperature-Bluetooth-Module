/*
 * i2c.h
 *
 *  Created on: Feb 7, 2019
 *      Author: andyhsu
 */
#include "stdint.h"
#include <stdbool.h>
#ifndef SRC_I2C_H_
#define SRC_I2C_H_
#define Si7021_address 					0x40
#define read_7021_register_address 		0xE7
#define write_7021_register_address		0xE6
#define write_bit 						0
#define read_bit 						1
#define SCLport							gpioPortC
#define	SCLpin							11
#define SDAport							gpioPortC
#define SDApin							10
#define tempsensor_pin						10
#define tempsensor_port						gpioPortB
#define TempHoldMaster					0xE3
#define TempNoHoldMaster				0xF3

//***********************
// function declarations
//**************************
void i2c_init(void);
uint8_t I2C0_Read();
void I2C0_Write(uint8_t data);
uint16_t I2C0_ReadTemp();
float convertTemp(uint16_t val);
void I2C0_IRQHandler(void);
void temptoASCII(float tempC, char * array, bool Celsius);
float CtoF(float temp);
float FtoC(float temp);


#endif /* SRC_I2C_H_ */
