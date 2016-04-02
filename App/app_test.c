#include "include.h"
#include "app_config.h"

static BOOL b_keyStartOld = FALSE;
static BOOL b_keyTimeSetOld = FALSE;
static BOOL b_keyTempSetOld = FALSE;

BOOL b_outputTemp = FALSE;
BOOL b_outputPump = FALSE;

enum
{
	NTC_NORMAL,
	NTC_OVER_TEMP,
	NTC_OPEN_CIRCUIT,
	NTC_SHORT_CIRCUIT,
}e_ntcState = NTC_NORMAL;

enum
{
	MODE_POWER_ON,
	MODE_STANDBY,
	MODE_WORK_HIGH,
	MODE_WORK_LOW,
	MODE_TIME_SET,
	MODE_TEMP_SET,
	MODE_AGING_TEST,
}e_workMode = MODE_STANDBY, e_workModeOld = MODE_STANDBY;

void app_testInit(void)
{
}

void app_testHandler100ms(void)
{
	static UINT8 u8_powerOnDelay = 20;
	static UINT8 u8_keyEnterSetCounter = 30;
	static UINT8 u8_keyLongEnterCounter = 0;
	static UINT8 u8_keyEnterTimeCounter = 0;
	UINT8 temp;
	if(u8_powerOnDelay)						//等待温度读取完成
	{
		u8_powerOnDelay--;
		if(b_keyTimeSet && b_keyTempSet)
		{
			drv_ledRequest(3, SoftWareVersion);
			e_workMode = MODE_AGING_TEST;
		}
		return;
	}
	temp = hwa_ntcGetTemp();
	if(e_workMode != MODE_TIME_SET
		&& e_workMode != MODE_TEMP_SET
		&& e_workMode != MODE_AGING_TEST
	)
	{
		if(temp == 0)
		{
			e_ntcState = NTC_SHORT_CIRCUIT;
		}
		else if(temp == 99)
		{
			e_ntcState = NTC_OPEN_CIRCUIT;
		}
		else if(temp >= 50)
		{
			e_ntcState = NTC_OVER_TEMP;
		}
		else
		{
			e_ntcState = NTC_NORMAL;
		}
	}
	
	switch(e_ntcState)
	{
		case NTC_NORMAL:
			switch(e_workMode)
			{					
				case MODE_STANDBY:
					u8_ledDisBuff[0] = 0;	//O
					u8_ledDisBuff[1] = 15;	//F
					if(b_keyStartOld != b_keyStart && b_keyStart)
					{
						drv_ledRequest(3, s_System.Temp);
						e_workMode = MODE_WORK_HIGH;
					}
					if(b_keyTimeSetOld != b_keyTimeSet && b_keyTimeSet)
					{
						e_workMode = MODE_TIME_SET;
						e_workModeOld = MODE_STANDBY;
					}
					if(b_keyTempSetOld != b_keyTempSet && b_keyTempSet)
					{
						e_workMode = MODE_TEMP_SET;
						e_workModeOld = MODE_STANDBY;
					}
					break;
					
				case MODE_WORK_HIGH:
					u8_ledDisBuff[0] = temp/10;
					u8_ledDisBuff[1] = temp%10;
					if(temp > s_System.Temp + 2)
					{
						e_workMode = MODE_WORK_LOW;
					}
					if(b_keyStartOld != b_keyStart && b_keyStart)
					{
						e_workMode = MODE_STANDBY;
					}
					break;
						
				case MODE_WORK_LOW:
					u8_ledDisBuff[0] = temp/10;
					u8_ledDisBuff[1] = temp%10;
					if(temp < s_System.Temp - 2)
					{
						e_workMode = MODE_WORK_HIGH;
					}
					if(b_keyStartOld != b_keyStart && b_keyStart)
					{
						e_workMode = MODE_STANDBY;
					}
					break;
					
				case MODE_TIME_SET:
					if(b_keyTimeSetOld != b_keyTimeSet && b_keyTimeSet)
					{
						u8_keyEnterSetCounter = 30;
						s_System.Time+=5;
						if(s_System.Time > 90)
						{
							s_System.Time = 10;
						}
						drv_ledRequest(3, s_System.Time);
					}
					else if(b_keyTimeSet)
					{
						u8_keyEnterSetCounter = 30;
						if(u8_keyLongEnterCounter < 20)
						{
							u8_keyLongEnterCounter++;
						}
						else
						{
							drv_ledRequest(0, s_System.Time);
							if(u8_keyEnterTimeCounter < 5)
							{
								u8_keyEnterTimeCounter++;
							}
							else
							{
								u8_keyEnterTimeCounter = 0;
								s_System.Time+=5;
								if(s_System.Time > 90)
								{
									s_System.Time = 10;
								}
							}
						}
					}
					else
					{
						u8_keyLongEnterCounter = 0;
						u8_keyEnterTimeCounter = 0;
					}
					
					if(u8_keyEnterSetCounter)
					{
						u8_keyEnterSetCounter--;
						if(u8_keyEnterSetCounter == 0)
						{
							if(s_System.Time >= 30)
							{
								app_configWrite();
								drv_ledRequest(0, s_System.Time);
							}
							e_workMode = e_workModeOld;
						}
					}
					break;
					
				case MODE_TEMP_SET:
					u8_ledDisBuff[0] = s_System.Temp/10;
					u8_ledDisBuff[1] = s_System.Temp%10;
					break;
				
				case MODE_AGING_TEST:
					u8_ledDisBuff[0] = temp/10;	
					u8_ledDisBuff[1] = temp%10;
					if(b_keyStartOld != b_keyStart && b_keyStart)
					{
						e_workMode = MODE_STANDBY;
					}
					break;
					
				default:
					e_workMode = MODE_STANDBY;
					break;
			}
			break;
			
		case NTC_SHORT_CIRCUIT:
			u8_ledDisBuff[0] = 14;	//E
			u8_ledDisBuff[1] = 1;	//1
			break;
			
		case NTC_OPEN_CIRCUIT:
			u8_ledDisBuff[0] = 14;	//E
			u8_ledDisBuff[1] = 2;	//2
			break;
			
		case NTC_OVER_TEMP:
			u8_ledDisBuff[0] = 20;	//H
			u8_ledDisBuff[1] = 20;	//H
			break;
	}
	b_keyStartOld = b_keyStart;
	b_keyTimeSetOld = b_keyTimeSet;
	b_keyTempSetOld = b_keyTempSet;
}

