#include "sys_stm8s.h"
#include "sys_eeprom.h"
#include "string.h"

#define EEPROM_ADDR			0x4000

void sys_eepromInit(void)
{
}

void sys_eepromReadData(UINT16 addr, UINT8 *dat, UINT16 len)
{
	UINT8 *p = (UINT8*)(addr+EEPROM_ADDR);
	while(len--)
	{
		*dat++ = *p++;
	}
}

void sys_eepromWriteData(UINT16 addr, UINT8 *dat, UINT16 len)
{
	do
	{
		/*操作EEPROM，需要先进行解锁*/  
		FLASH_DUKR = 0xAE;			//注意这里不能断点调试，否则会造成内部不同步，FLASH解锁失败  
		FLASH_DUKR = 0x56;
//		FLASH_CR2 = 0x00;
//		FLASH_NCR2 = 0xFF;
	}
	while(!(FLASH_IAPSR & 0x08));	//检测对应的位是否解锁
	memcpy((UINT8*)(addr+EEPROM_ADDR), dat, len);
	FLASH_IAPSR = ~0x08;			//重新上锁
}