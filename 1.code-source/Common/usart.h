#ifndef _usart_H //宏定义，定义文件名称
#define _usart_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm8s.h"

void USART1_Init(void);
void Send(uint8_t dat);
void UART1_sendhex(unsigned char dat);
void UART1_sendstr(unsigned char *dat);
void SendSe(uint8_t Data);

#endif