#include "sys_stm8s.h"
#include "sys_adc.h"

volatile UINT16 u16_vcc;

void sys_adcInit(void)
{
	ADC_CR1 = 0x02;		//关闭ADC，开启连续转换
	ADC_CR2 = 0x00;		//关闭外部触发，数据左对齐，禁止扫描模式
	ADC_CR3 = 0x80;		//开启数据缓存
	sys_adcReadVcc();
}

UINT16 sys_adcValue(UINT8 ch)
{
	UINT8 const volatile *p = (UINT8*)&ADC_DB0RH;
	UINT8 tmpl;
	UINT16 tmph;
	UINT16 sum = 0;
	UINT8 i;
	ADC_CSR &= ~0x8F;
	ADC_CSR |= ch&0x0F;		//设置采样通道
	ADC_CR1 |= 0x01;		//开启ADC
	for(i=0; i<100; i++);	//等待ADC开启，至少7us
	ADC_CR1 |= 0x01;		//启动转换
	while(!(ADC_CSR&0x80));	//等待转换完成
	ADC_CSR &= ~0x80;		//清除标志
	ADC_CR1 &= ~0x01;		//关闭ADC
	for(i=0; i<10; i++)
	{
		tmph = p[i];		//左对齐先读高位
		tmpl = p[i+1];
		sum += (tmph<<2)|tmpl;
	}
	return sum/10;
}

UINT16 sys_adcReadVcc(void)
{
	UINT16 tmp = sys_adcValue(7);
	if(tmp != 0)
	{
		u16_vcc = 1249280L/tmp;	//1.22*1024*1000
	}
	else
	{
		u16_vcc = 0;
	}
	return u16_vcc;
}

