#include "iostm8s103f3.h"
#include "sys_iwdg.h"


void sys_iwdgInit(void)
{
    CLK_ICKR |= BIT3;			//enable LSI
    while(!(CLK_ICKR & BIT4));		//wait LSI ready
    IWDG_KR = 0xCC;
    IWDG_KR = 0x55;				//
    IWDG_PR = 0x06;				//256
    IWDG_RLR = 250;				//250*256/128000 = 0.5s
    IWDG_KR = 0xAA;
}

void sys_iwdgReset(void)
{
    IWDG_KR = 0xAA;
}
