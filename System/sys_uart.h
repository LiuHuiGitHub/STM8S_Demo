#ifndef __SYS_UART_H__
#define __SYS_UART_H__

#include "typedef.h"

void sys_uartInit(void);
void UART1_SendByte(UINT8 data);
void UART1_SendString(UINT8* Data,UINT16 len);
void sys_uartTransmitData(UINT8 *Data, UINT8 len);

#endif
