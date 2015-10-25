#include "sys_stm8s.h"
#include "sys_optionByte.h"
#include "string.h"

const UINT8 OptionByte[11] = {0x00,0x00,0xFF,0x81,0x7E,0x00,0xFF,0x00,0xFF,0x00,0xFF};
const UINT8 *pOptionByte = (UINT8*)0x4800;

void sys_optionByteInit(void)
{
	if(memcmp(pOptionByte, OptionByte, 11))
	{
		do
		{
			FLASH_DUKR = 0xAE;
			FLASH_DUKR = 0x56;
		}while(!(FLASH_IAPSR & 0x08));
		FLASH_CR2 = 0x80;
		FLASH_NCR2 = 0x7F;
		memcpy((void*)pOptionByte, OptionByte, 11);
		while(!(FLASH_IAPSR & 0x04));
		FLASH_IAPSR &= ~0x80;
	}
}
