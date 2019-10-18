#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_
//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdbool.h>
#include "em_core.h"

//***********************************************************************************
// enumerations
//***********************************************************************************

#define	EM0					0
#define EM1					1
#define EM2					2
#define	EM3					3
#define	EM4					4
#define	EM4s				5
#define	Max_Sleep_Modes		6
#define RECEIVE_SIZE		5

#define BUTTON0_CHANNEL         0             /**< Button 0 channel capsense */
#define BUTTON1_CHANNEL         3             /**< Button 1 channel capsense */

enum event
{
	FLAG1 = 0b00000001,		// startframe flag
	FLAG2 = 0b00000010,		// txdata flag
	FLAG3 = 0b00000100,		// comp0 flag
	FLAG4 = 0b00001000,		// comp1 flag
	FLAG5 = 0b00010000,		// sigframe flag (end of transmission)
	FLAG6 = 0b00100000,		// cap sense at location 0
	FLAG7 = 0b01000000,
	FLAG8 = 0b10000000,
};


//***********************************************************************************
// defined files
//***********************************************************************************

#define Letimer0_Period		3			// Letimer0 in seconds
#define Letimer0_On_Time	80			// Letimer0 On Time in milliseconds

#define	Letimer0_EM			EM2			// define first energy mode that it cannot enter for LETIMER0
#define STDLEN				7			// length of string of ascii
#define txdata				0x3B

//***********************************************************************************
// global variables
//***********************************************************************************
unsigned int lowest_energy_mode[Max_Sleep_Modes];

//const char extern volatile name[]; // "shnaEMAN+TA"
//uint8_t extern volatile pointer;
int extern volatile scheduler_event;
extern volatile char recdata[RECEIVE_SIZE];

//***********************************************************************************
// function prototypes
//***********************************************************************************
#endif
