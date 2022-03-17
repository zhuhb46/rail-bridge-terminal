#include "stm8s.h"
#include "LoRa.h"
#include "usart.h"
#include "string.h"
#include "timer1.h"
#include "timer4.h"

uint8_t wakeUp[] = "wake";
uint8_t work[] = "work";
uint32_t delay = 200;
/*
LoRaSendData：向协调器发送数据
@ID：目标协调器ID，如：ID[2] = {0x00，0x25};
@Channel:目标协调器信道，如：Channel = 0x25;
@TempIdx:由协调器分配的临时牌照
@idx：数据包编号
@msg：发送计数
@Data：待发送数据
@Datalen：数据包Data的长度
*/
void LoRaSendData(uint8_t *ID, uint8_t Channel, uint8_t TempIdx, uint8_t idx, uint8_t *msg, uint8_t *Data, uint8_t DataLen)
{
	Send(ID[0]);
	Send(ID[1]);
	Send(Channel);
	Send(0xAB);
	Send(0xAD);
	Send(TempIdx);
	Send(idx);
	Send(msg[0]);
	Send(msg[1]);
	int i = 0;
	for (i = 0; i < DataLen; i++)
	{
		Send(Data[i]);
	}
	Send(0xAE);
	Send(0xAE);
}

/*
LoRaSendWakeup：向协调器发送苏醒信息
@ID：目标协调器ID，如：ID[2] = {0x00，0x25};
@Channel:目标协调器信道，如：Channel = 0x25;
@myID:本机的ID，如：myID[2] = {0x00，0x20};
@myChannel：本机的信道，如：myChannel = 0x20;
@ChipID：本机的单片机chipid
@voltage：本机的电压值
*/
void LoRaSendWakeup(uint8_t *ID, uint8_t Channel, uint8_t *myID, uint8_t myChannel, uint8_t *ChipID, uint16_t voltage, uint8_t errorcnt, uint16_t wakecnt)
{
	Send(ID[0]);
	Send(ID[1]);
	Send(Channel);
	for (int i = 0; i < 4; i++)
	{
		Send(wakeUp[i]);
	}
	Send(myID[0]);
	Send(myID[1]);
	Send(myChannel);
	for (int i = 0; i < 9; i++)
	{
		Send(ChipID[i]);
	}
	Send(errorcnt);
	Send(wakecnt >> 8);
	Send(wakecnt & 0xff);
	Send(voltage >> 8);
	Send(voltage & 0xff);
}

void LoRaSendWork(uint8_t *ID, uint8_t Channel)
{
	Send(ID[0]);
	Send(ID[1]);
	Send(Channel);
	for (int i = 0; i < 4; i++)
	{
		Send(work[i]);
	}
}

void USART_SendString(char *str)
{
	unsigned int k = 0;
	do
	{
		Send(*(str + k));
		k++;
	} while (*(str + k) != '\0');
}

void print_LoRa_env(char *FeedBack, uint8_t *recvnct)
{
	memset(FeedBack, 0, 30);
	USART_SendString("AT+ADDR\r\n");
	*recvnct = 0;
	delay_ms(200);

	memset(FeedBack, 0, 30);
	USART_SendString("AT+CH\r\n");
	*recvnct = 0;
	delay_ms(200);

	memset(FeedBack, 0, 30);
	USART_SendString("AT+WMODE\r\n");
	*recvnct = 0;
	delay_ms(200);

	memset(FeedBack, 0, 30);
	USART_SendString("AT+CFGTF\r\n");
	*recvnct = 0;
	delay_ms(200);
}

uint8_t LoRaInit(char *compare_ADDR, char *compare_CHEL, char *FeedBack, uint8_t *recvnct)
{
	uint8_t i = 0;
	FeedBack[0] = 0;
	char compare_a[2] = "a";
	while (strncmp(FeedBack, compare_a, 1) != 0)
	{
		*recvnct = 0;
		USART_SendString("+++");
		delay_ms(200);
		if (i > 15)
		{
			return 0;
		}
		else
			i++;
	}
	memset(FeedBack, 0, 30);
	char compare_OK[5] = "+OK";
	while (strncmp(FeedBack, compare_OK, 3) != 0)
	{
		USART_SendString("a");
		*recvnct = 0;
		delay_ms(200);
		if (i > 15)
		{
			return 1;
		}
		else
			i++;
	}
	memset(FeedBack, 0, 30);
	// char compare_ADDR[25] = CPA_ADDR;
	while (strncmp(FeedBack, compare_ADDR, 10) != 0)
	{
		USART_SendString(compare_ADDR);
		*recvnct = 0;
		delay_ms(200);
		if (i > 5)
		{
			return 2;
		}
		else
			i++;
	}
	memset(FeedBack, 0, 30);
	// char compare_CHEL[25] = CPA_CHEL;
	while (strncmp(FeedBack, compare_CHEL, 8) != 0)
	{
		USART_SendString(compare_CHEL);
		*recvnct = 0;
		delay_ms(200);
		if (i > 5)
		{
			return 3;
		}
		else
			i++;
	}
	USART_SendString("AT+WMODE=FP\r\n");
	memset(FeedBack, 0, 30);
	delay_ms(200);
	return 88;
}
