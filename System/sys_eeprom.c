#include "iostm8s103f3.h"
#include "sys_eeprom.h"
#include "string.h"

#define EEPROM_ADDR			0x4000

CONST UINT8 OptionBytes[10] = {0x00, 0xFF, 0x04, 0xFB, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
UINT8 *const pOptionByte = (UINT8 *)0x4801;

#define sys_eepromUnlock()		do\
								{\
									FLASH_DUKR = 0xAE;\
									FLASH_DUKR = 0x56;\
								}\
								while(!(FLASH_IAPSR & 0x08))

#define sys_eepromLock()		do\
								{\
								 	FLASH_IAPSR &= ~0x08;\
								}\
								while(0)

void sys_eepromInit(void)
{
    if(memcmp(pOptionByte, OptionBytes, 10))
    {
        sys_eepromUnlock();
        FLASH_CR2 = 0x80;
        FLASH_NCR2 = 0x7F;
        memcpy(pOptionByte, OptionBytes, 10);
        while(!(FLASH_IAPSR & 0x04));
        FLASH_IAPSR &= ~0x80;
    }
}

void sys_eepromEraseSector(UINT16 addr)
{
  	//..
}

void sys_eepromRead(UINT16 addr, UINT8 *dat, UINT16 len)
{
    UINT8 *p = (UINT8 *)(addr + EEPROM_ADDR);
    while(len--)
    {
        *dat++ = *p++;
    }
}

void sys_eepromWrite(UINT16 addr, UINT8 *dat, UINT16 len)
{
    sys_eepromUnlock();					//����Ӧ��λ�Ƿ����
    memcpy((UINT8 *)(addr + EEPROM_ADDR), dat, len);
    sys_eepromLock();					//��������
}