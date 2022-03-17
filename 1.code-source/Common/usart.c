#include "stm8s.h"
#include "usart.h"

uint8_t HexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void USART1_Init(void)
{
	(void)UART1->SR;
	(void)UART1->DR;
	UART1->BRR2 = 0x00; /* Set UART1_BRR2 to reset value 0x00 */
	UART1->BRR1 = 0x00; /* Set UART1_BRR1 to reset value 0x00 */
	UART1->CR1 = 0x00;	/* Set UART1_CR1 to reset value 0x00 */
	UART1->CR2 = 0x00;	/* Set UART1_CR2 to reset value 0x00 */
	UART1->CR3 = 0x00;	/* Set UART1_CR3 to reset value 0x00 */
	UART1->CR4 = 0x00;	/* Set UART1_CR4 to reset value 0x00 */
	UART1->CR5 = 0x00;	/* Set UART1_CR5 to reset value 0x00 */
	UART1->GTR = 0x00;
	UART1->PSCR = 0x00;
	uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;
	UART1->CR1 &= (uint8_t)(~((uint8_t)0x10));
	UART1->CR1 |= (uint8_t)0x00;
	UART1->CR3 &= (uint8_t)(~((uint8_t)0x30));
	UART1->CR3 |= (uint8_t)0x00;
	UART1->CR1 &= (uint8_t)(~(((uint8_t)0x04) | ((uint8_t)0x02)));
	UART1->CR1 |= (uint8_t)0x00;
	UART1->BRR1 &= (uint8_t)(~((uint8_t)0xFF));
	UART1->BRR2 &= (uint8_t)(~((uint8_t)0xF0));
	UART1->BRR2 &= (uint8_t)(~((uint8_t)0x0F));
	BaudRate_Mantissa = ((uint32_t)CLK_GetClockFreq() / ((uint32_t)115200 << 4));
	BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / ((uint32_t)115200 << 4));
	UART1->BRR2 |= (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F);
	UART1->BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0);
	UART1->BRR1 |= (uint8_t)BaudRate_Mantissa;
	UART1->CR2 &= (uint8_t) ~(((uint8_t)0x08) | ((uint8_t)0x04));
	UART1->CR3 &= (uint8_t) ~(((uint8_t)0x04) | ((uint8_t)0x02) | ((uint8_t)0x01));
	UART1->CR3 |= (uint8_t)((uint8_t)0x80 & (uint8_t)(((uint8_t)0x04) |
													  ((uint8_t)0x02) | ((uint8_t)0x01)));
	UART1->CR2 |= (uint8_t)((uint8_t)0x08);
	UART1->CR2 |= (uint8_t)((uint8_t)0x04);
	UART1->CR3 &= (uint8_t)(~((uint8_t)0x08));
	uint8_t uartreg = 0, itpos = 0x00;
	uartreg = (uint8_t)((uint16_t)0x0205 >> 0x08);
	itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)0x0205 & (uint8_t)0x0F));
	if (uartreg == 0x01)
	{
		UART1->CR1 &= (uint8_t)(~itpos);
	}
	else if (uartreg == 0x02)
	{
		UART1->CR2 &= (uint8_t)(~itpos);
	}
	else
	{
		UART1->CR4 &= (uint8_t)(~itpos);
	}
	// UART1_DeInit();
	// UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
	// UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
	enableInterrupts();
}
void Send(uint8_t Data)
{
	while ((UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET))
		;
	UART1->DR = Data;
}

void UART1_sendhex(unsigned char dat)
{
	Send(HexTable[dat >> 4]);
	Send(HexTable[dat & 0x0f]);
}
void UART1_sendstr(unsigned char *dat)
{
	while (*dat != '\0')
	{
		Send(*dat);
		dat++;
	}
}

void SendSe(uint8_t Data)
{
	uint8_t j = Data;
	Data = ((Data & 0x55) << 1);
	j = ((j & 0xAA) >> 1);
	Data = Data | j;
	while ((UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET))
		;
	UART1_SendData8(~Data);
}