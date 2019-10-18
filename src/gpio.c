//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "i2c.h"
#include "leuart.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
void gpio_init(void){

	// Set LED ports to be standard output drive with default off (cleared)
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);


	/* map EFM32 pins as SCL and SDA of si7021 (WiredAnd, not push and pull) */
	GPIO_PinModeSet(SCLport, SCLpin, gpioModeDisabled, off);	// SCL
	GPIO_PinModeSet(SDAport, SDApin, gpioModeDisabled, off);	// SDA

	/*temperature sensor pinmode enable */
	GPIO_PinModeSet(tempsensor_port, tempsensor_pin, gpioModePushPull, off);
	GPIO_PinOutClear(tempsensor_port, tempsensor_pin);

	/* bluetooth leuart gpio enable */
//	GPIO_PinModeSet(LEUARTRXPORT, LEUARTRXPIN, gpioModePushPull, on);
	GPIO_PinModeSet(LEUARTRXPORT, LEUARTRXPIN, gpioModeInput, off);
	//GPIO_DriveStrengthSet(LEUARTRXPORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LEUARTTXPORT, LEUARTTXPIN, gpioModePushPull, on);
	GPIO_DriveStrengthSet(LEUARTTXPORT, gpioDriveStrengthWeakAlternateWeak);



}
