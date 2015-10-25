#include "sys_stm8s.h"
#include "sys_uart.h"
#include "string.h"

#define HSIClockFreq		16000000L

#define UART1_FLAG_TXE		0x80	/*!< Transmit Data Register Empty flag */
#define UART1_FLAG_RXNE		0x20	/*!< Read Data Register Not Empty flag */

#define boud		9600
#define UART_TX_BUFF_LEN		10
volatile UINT8 Tx_Buff[UART_TX_BUFF_LEN];
volatile UINT8 TxIndex = 0;
UINT8 TxEnd = 0;

void sys_uartInit(void)
{
    UINT16 baud_div=0;
	CLK_PCKENR1 |= BIT2;		//enable uart1 clock
	
	PD_DDR |= BIT5;
	PD_CR1 |= BIT5;
	PD_CR2 |= BIT5;
	
	PD_DDR &= ~BIT6;
	PD_CR1 |= BIT6;
	PD_CR2 &= ~BIT6;
	
    UART1_CR1 = (0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0); 
    /*1位起始位 8位数据位 结束位由CR3设置 不使用奇偶校验 不使能奇偶校验中断*/
    UART1_CR2 = (0<<7)|(0<<6)|(1<<5)|(0<<4)|(1<<3)|(1<<2); 
    /*使能发送和接收 接收中断使能 禁止发送完成中断*/
    UART1_CR3 = (0<<6)|(0<<4)|(0<<3); /*设置1位停止位 不使能SCLK*/        
    UART1_CR5 = (0<<2)|(0<<1);     
    /*使用智能卡模式需要设置的，基本上保持默认就行了 */ 
    
    /*设置波特率*/
    baud_div =HSIClockFreq/boud;  /*求出分频因子*/
    UART1_BRR2 = baud_div & 0x0f;
    UART1_BRR2 |= ((baud_div & 0xf000) >> 8);
    UART1_BRR1 = ((baud_div & 0x0ff0) >> 4);    /*先给BRR2赋值 最后再设置BRR1*/
    
	(void)UART1_SR;
	UART1_DR = 0x00;
    UART1_CR1 |= (0<<5);         /*使能UART*/
}

void UART1_SendByte(UINT8 data)
{
    UART1_DR=data;
   /* Loop until the end of transmission */
   while (!(UART1_SR & UART1_FLAG_TXE));
}

void UART1_SendString(UINT8* Data,UINT16 len)
{
  UINT16 i=0;
  for(;i<len;i++)
    UART1_SendByte(Data[i]);
}

__interrupt void UART1_TX_IRQHandler(void)
{
	if(++TxIndex < TxEnd)
	{
		(void)UART1_SR;
		UART1_DR = Tx_Buff[TxIndex];
	}
	else
	{
		UART1_CR2 &= ~0x40;			//禁用发送中断
		TxIndex = TxEnd;
	}
}

void sys_uartTransmitData(UINT8 *Data, UINT8 len)
{
	while(TxIndex != TxEnd);
	if(len > UART_TX_BUFF_LEN)
	{
		len = UART_TX_BUFF_LEN;
	}
	TxIndex = 0;
	TxEnd = len;
	memcpy((void*)Tx_Buff, Data, len);
	UART1_CR2 |= 0x40;			//禁用发送中断
	UART1_DR = Tx_Buff[0];
}

__interrupt void UART1_RX_IRQHandler(void)
{
	static UINT8 Res = 0;
	(void)Res;
	if(UART1_SR & 0x01)
	{
		UART1_SR &= ~0x01;
	}
	else if(UART1_SR & UART1_FLAG_RXNE)
	{
		Res =(UINT8)UART1_DR;
	}
}
