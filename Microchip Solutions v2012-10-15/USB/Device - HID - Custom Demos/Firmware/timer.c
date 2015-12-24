/*********************************************************************
* FileName:        timer.c
*********************************************************************/
#define USE_OR_MASKS
#include <p18cxxx.h>
#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "timers.h"
#include "stdio.h"

#include "timer.h"
#include "i2c_bma150.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
unsigned long count =0;

//------------------------------------------------------------------
//------------------------------------------------------------------
void timer0_init(void)
{
	//Fosc=16Mhz, internal source = Fosc/4, 
	OpenTimer0(TIMER_INT_ON | T0_16BIT | T0_SOURCE_INT | T0_PS_1_8);
	T0CONbits.T0CS = 0; //Timer 1Clock internal source
	INTCONbits.TMR0IF = 0; //clear interrupt flag
	INTCONbits.GIEH = 1; /* Enable global Interupt */
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void timer0_remove(void)
{
	CloseTimer0(); 
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void toggle_LED(void)
{
	mLED_1 = !mLED_1;
    //mLED_2 = !mLED_2;
}
