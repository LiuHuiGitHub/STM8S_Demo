#include "iostm8s103f3.h"
#include "drive_led.h"
#include "drive_led_Cfg.h"

#define LED_CS0 PC_ODR_ODR4
#define LED_CS1 PC_ODR_ODR3


/*Seg Port*/
//#define SEG_PORT        (PD_ODR<<1|PC_ODR>>6)


//与显示相关变量
CONST UINT8 ledCoding[] =
{  
	(UINT8)(~SEG_0),(UINT8)(~SEG_1),(UINT8)(~SEG_2),(UINT8)(~SEG_3),(UINT8)(~SEG_4),
	(UINT8)(~SEG_5),(UINT8)(~SEG_6),(UINT8)(~SEG_7),(UINT8)(~SEG_8),(UINT8)(~SEG_9),
	(UINT8)(~SEG_A),(UINT8)(~SEG_B),(UINT8)(~SEG_C),(UINT8)(~SEG_D),(UINT8)(~SEG_E),
	(UINT8)(~SEG_F),(UINT8)(~SEG_LINE),(UINT8)(~SEG_P),(UINT8)(~SEG_U),(UINT8)(~SEG_NULL),
	(UINT8)(~SEG_H),
};

UINT8 u8_ledRequestCounter = 0;
UINT8 u8_ledRequestCount = 0;
UINT8 u8_ledRequestData[2] = {0};

UINT8 u8_ledDisBuff[2] = {19,19};
UINT8 u8_ledDot = 0x00;
static UINT8 u8_ledIndex = 0;//从左到右为0,1,2

BOOL b_keyStart = FALSE;
BOOL b_keyTimeSet = FALSE;
BOOL b_keyTempSet = FALSE;

void drv_ledInit(void)
{
	LED_CS0 = SEG_OFF;
	LED_CS1 = SEG_OFF;
}

/* LED动态显示 */
void drv_ledHandler2ms(void)
{
	UINT8 coding;
	LED_CS0 = SEG_OFF;
	LED_CS1 = SEG_OFF;
	u8_ledIndex++;
	u8_ledIndex %= 4;
	
//	if (u8_ledDot & (1 << u8_ledIndex))//show dot
//	{
//        if(SEG_CLASS)
//        {
//            SEG_PORT |= SEG_PORT_H;
//        }
//        else
//        {
//            SEG_PORT &= ~SEG_PORT_H;
//        }
//	}
	if (u8_ledIndex == 0)
	{
		if(u8_ledRequestCount == 0)
		{
			coding = ledCoding[u8_ledDisBuff[0]];
		}
		else
		{
			if(u8_ledRequestCounter < 77)
			{
				coding = ledCoding[u8_ledRequestData[0]];
			}
			else if(u8_ledRequestCounter < 125)
			{
				coding = ledCoding[19];
			}
		}
		PA_ODR_ODR2 = coding>>6;
		PA_ODR_ODR1 = coding>>5;
		PD_ODR_ODR3 = coding>>4;
		PD_ODR_ODR2 = coding>>3;
		PC_ODR_ODR7 = coding>>2;
		PC_ODR_ODR6 = coding>>1;
		PC_ODR_ODR5 = coding>>0;
		LED_CS0 = SEG_ON;
	}
	else if(u8_ledIndex == 1)
	{
		if(u8_ledRequestCount == 0)
		{
			u8_ledRequestCounter = 0;
			coding = ledCoding[u8_ledDisBuff[1]];
		}
		else
		{
			u8_ledRequestCounter++;
			if(u8_ledRequestCounter < 77)
			{
				coding = ledCoding[u8_ledRequestData[1]];
			}
			else if(u8_ledRequestCounter < 125)
			{
				coding = ledCoding[19];
			}
			else
			{
				u8_ledRequestCounter = 0;
				u8_ledRequestCount--;
			}
		}
		PA_ODR_ODR2 = coding>>6;
		PA_ODR_ODR1 = coding>>5;
		PD_ODR_ODR3 = coding>>4;
		PD_ODR_ODR2 = coding>>3;
		PC_ODR_ODR7 = coding>>2;
		PC_ODR_ODR6 = coding>>1;
		PC_ODR_ODR5 = coding>>0;
		LED_CS1 = SEG_ON;
	}
	else if (u8_ledIndex == 2)
	{
		PA_ODR_ODR2 = 1;//h
		PA_ODR_ODR1 = 1;//g
		PD_ODR_ODR4 = 1;//f
		PD_ODR_ODR3 = 1;//e
		PD_ODR_ODR2 = 1;//d
		PC_ODR_ODR7 = 1;//c
		PC_ODR_ODR6 = 1;//b
		PC_ODR_ODR5 = 1;//a
		
		PD_DDR_DDR2 = 0;
		PD_CR1_C12 = 1;
		PD_CR2_C22 = 0;
		
		PD_DDR_DDR3 = 0;
		PD_CR1_C13 = 1;
		PD_CR2_C23 = 0;
		
		PC_DDR_DDR6 = 0;
		PC_CR1_C16 = 1;
		PC_CR2_C26 = 0;
	}
	else
	{
		if(PD_IDR_IDR2 == 0)
		{
			b_keyStart = TRUE;
		}
		else
		{
			b_keyStart = FALSE;
		}
		if(PD_IDR_IDR3 == 0)
		{
			b_keyTimeSet = TRUE;
		}
		else
		{
			b_keyTimeSet = FALSE;
		}
		if(PC_IDR_IDR6 == 0)
		{
			b_keyTempSet = TRUE;
		}
		else
		{
			b_keyTempSet = FALSE;
		}
		
		PD_DDR |= 0xFC;
		PD_CR1 |= 0xFC;
		PD_CR2 &= 0x03;
		
		PC_DDR |= 0xFF;
		PC_CR1 |= 0xFF;
		PC_CR2 &= (UINT8)(~0xFF);
	}
}

void drv_ledRequest(UINT8 count, UINT8 n)
{
	if(count == 0)
	{
		u8_ledRequestCounter = 0;
	}
	u8_ledRequestCount = count;
	u8_ledRequestData[0] = n / 10;
	u8_ledRequestData[1] = n % 10;
}

UINT8 drv_ledGetRequest(void)
{
	return u8_ledRequestCount;
}

