#include "sys_stm8s.h"
#include "sys_adc.h"

volatile UINT16 u16_vcc;

void sys_adcInit(void)
{
	ADC_CR1 = 0x02;		//�ر�ADC����������ת��
	ADC_CR2 = 0x00;		//�ر��ⲿ��������������룬��ֹɨ��ģʽ
	ADC_CR3 = 0x80;		//�������ݻ���
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
	ADC_CSR |= ch&0x0F;		//���ò���ͨ��
	ADC_CR1 |= 0x01;		//����ADC
	for(i=0; i<100; i++);	//�ȴ�ADC����������7us
	ADC_CR1 |= 0x01;		//����ת��
	while(!(ADC_CSR&0x80));	//�ȴ�ת�����
	ADC_CSR &= ~0x80;		//�����־
	ADC_CR1 &= ~0x01;		//�ر�ADC
	for(i=0; i<10; i++)
	{
		tmph = p[i];		//������ȶ���λ
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

