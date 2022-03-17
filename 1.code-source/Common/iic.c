#include "stm8s.h"
#include "iic.h"

void SimIIC_Init(void)
{
	GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_FAST);
}

void I2C_Start(void)
{
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->DDR |= (uint8_t)GPIO_PIN_2;
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_2);
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
}

void I2C_Stop(void)
{
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_2);
	GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
}

void I2C_Ack(void)
{
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->DDR |= (uint8_t)GPIO_PIN_2;
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_2);
	GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
}

void I2C_NAck(void)
{
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->DDR |= (uint8_t)GPIO_PIN_2;
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
	GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
}
// 返回1 代表 非应答信号
// 返回0 代表 有应答信号
int I2C_Wait_Ack(void)
{
	int tempTime = 0;
	//  I2C_SDA_IN;

	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->DDR &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));

	GPIOD->ODR |= (uint8_t)GPIO_PIN_3;

	while (GPIO_ReadInputPin(GPIOD, GPIO_PIN_2))
	{
		tempTime++;
		if (tempTime > 250)
		{
			I2C_Stop();
			return 1;
		}
	}

	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);

	return 0;
}

void I2C_Send_Byte(int txd)
{
	int i = 0;
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	//GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
	GPIOD->DDR |= (uint8_t)GPIO_PIN_2;
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
	for (i = 0; i < 8; i++)
	{
		if ((txd & 0x80) > 0)
		{
			GPIOD->ODR |= (uint8_t)GPIO_PIN_2;
		}
		else
		{
			GPIOD->ODR &= (uint8_t)(~GPIO_PIN_2);
		}

		txd <<= 1;
		GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
		GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
	}
}
//ack=0 发送非应答信号 代表接收完成
//ack=1 发送应答信号 代表接收未完成 继续接收
int I2C_Read_Byte()
{
	int i = 0, receive = 0;
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->DDR &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR1 &= (uint8_t)(~(GPIO_PIN_2));
	GPIOD->CR2 &= (uint8_t)(~(GPIO_PIN_2));
	for (i = 0; i < 8; i++)
	{
		GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
		GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
		receive <<= 1;
		if (GPIO_ReadInputPin(GPIOD, GPIO_PIN_2))
			receive++;
	}
	return receive;
}
