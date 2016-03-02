#include "iostm8s103f3.h"
#include "include.h"

static BOOL b_taskHandler1ms = FALSE;
static BOOL b_taskCycle10ms  = FALSE;
static BOOL b_taskCycle100ms = FALSE;

static UINT8 u8_taskCycleCount10ms  = 0;
static UINT8 u8_taskCycleCount100ms = 0;

static void sys_taskHandler1ms(void);
static void sys_taskCycle1ms(void);
static void sys_taskCycle10ms(void);
static void sys_taskCycle100ms(void);

void sys_taskGpioInit(void)
{
	PA_DDR |= BIT3;
	PA_CR1 |= BIT3;
	PA_CR2 &= ~BIT3;
}

void sys_taskInit(void)
{
	CLK_PCKENR1 |= 0x01;
	TIM4_PSCR = 0x07;				//TIM4 prescaler is 128, TIM4 clock is 125KHz
	TIM4_ARR = 125;					//TIM4 Handler 1ms
	TIM4_EGR = 0x01;				//enable update
	TIM4_IER = 0x01;				//enable update interrupt
	TIM4_CR1 = 0x01;				//enable TIM4
}

#pragma vector = TIM4_UPD_OVF_IRQ
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
	b_taskHandler1ms = TRUE;
	sys_taskHandler1ms();
	TIM4_SR &= ~0x01;
}

void sys_taskHandler1ms(void)
{
}

int main(void)
{
	CLK_CKDIVR = 0x00;				//cpu clock is HSI 16MHz
	sys_taskGpioInit();
	sys_eepromInit();
	sys_taskInit();
	sys_adcInit();
	sys_pwmInit();
	sys_iwdgInit();
	sys_uartInit();
	sys_tim2Init();
	enable_interrupt();
	while(1)
	{
		sys_iwdgReset();
		if(b_taskHandler1ms)
		{
			b_taskHandler1ms = FALSE;
			sys_taskCycle1ms();
			if(++u8_taskCycleCount10ms >= 10)
			{
				u8_taskCycleCount10ms = 0;
				b_taskCycle10ms = TRUE;
			}
		}
		if(b_taskCycle10ms)
		{
			b_taskCycle10ms = FALSE;
			sys_taskCycle10ms();
			if(++u8_taskCycleCount100ms >= 10)
			{
				u8_taskCycleCount100ms = 0;
				b_taskCycle100ms = TRUE;
			}
		}
		if(b_taskCycle100ms)
		{
			b_taskCycle100ms = FALSE;
			sys_taskCycle100ms();
		}
	}
}

static void sys_taskCycle1ms(void)
{
	UINT8 len;
	static UINT8 Buff[20];
	len = sys_uartReceiveData(Buff);
	if(len)
	{
		sys_uartTransmitData(Buff, len);
	}
	sys_pwmSet(sys_adcValue(3)>>2);
}

static void sys_taskCycle10ms(void)
{
}

static void sys_taskCycle100ms(void)
{
	static UINT8 count = 0;
	if(++count>=5)
	{
		PA_ODR_ODR3 = !PA_ODR_ODR3;
		count = 0;
	}
}
