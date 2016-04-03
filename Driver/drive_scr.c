#include "iostm8s103f3.h"
#include "drive_scr.h"

SCR_STATE e_scrState = SCR_OFF;
static UINT16 u16_vvvfScrCounter = 0;

#define OUTPUT_SCR0		PA_ODR_ODR4
#define OUTPUT_SCR1		PA_ODR_ODR4
#define OUTPUT_LED		PA_ODR_ODR3

#define LED_ON			0
#define LED_OFF			1

void drv_scrInit(void)
{
	OUTPUT_SCR0 = OFF;
	OUTPUT_SCR1 = OFF;
	OUTPUT_LED = LED_OFF;
}

UINT8 ttt = 15;

void drv_ledBreathing(void)
{
	static UINT8 dir = 0;
	static UINT8 counter = 0;
	static UINT8 count = 0;
	if(counter < 10)
	{
		counter++;
	}
	else
	{
		counter = 0;
	}
	if(++count >= 75)
	{
		count = 0;
		if(dir == 0)
		{
			if(--ttt == 0)
			{
				dir = 1;
			}
		}
		else
		{
			if(++ttt >= 10)
			{
				dir = 0;
			}
		}
	}
	
	if(counter >= ttt)
	{
		OUTPUT_LED = LED_ON;
	}
	else
	{
		OUTPUT_LED = LED_OFF;
	}
}

void drv_scrHandler2ms(void)
{
	if(e_scrState == SCR_FULL)
	{
		OUTPUT_SCR0 = ON;
		OUTPUT_SCR1 = ON;
		OUTPUT_LED = LED_ON;
	}
	else if(e_scrState == SCR_VVVF)
	{
		OUTPUT_SCR0 = ON;
		u16_vvvfScrCounter++;
		if(u16_vvvfScrCounter >= 500)
		{
			u16_vvvfScrCounter = 0;
			OUTPUT_SCR1 = ON;
		}
		else if(u16_vvvfScrCounter >= 400)
		{
			OUTPUT_SCR1 = OFF;
		}
		drv_ledBreathing();
	}
	else if(e_scrState == SCR_VVVF_OFF)
	{
		OUTPUT_SCR0 = ON;
		OUTPUT_SCR1 = OFF;
		drv_ledBreathing();
	}
	else
	{
		OUTPUT_SCR0 = OFF;
		OUTPUT_SCR1 = OFF;
		OUTPUT_LED = LED_OFF;
	}
}

void drv_scrRequest(SCR_STATE state)
{
	e_scrState = state;
}
