#ifndef _iic_H //宏定义，定义文件名称
#define _iic_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm8s.h"

#define I2C_SCL_PIN GPIO_PIN_3
#define I2C_SDA_PIN GPIO_PIN_2

#define I2C_PORT GPIOD

#define I2C_SCL_L GPIO_WriteLow(I2C_PORT, I2C_SCL_PIN);
#define I2C_SCL_H GPIO_WriteHigh(I2C_PORT, I2C_SCL_PIN);

#define I2C_SDA_L GPIO_WriteLow(I2C_PORT, I2C_SDA_PIN);
#define I2C_SDA_H GPIO_WriteHigh(I2C_PORT, I2C_SDA_PIN);

#define I2C_SDA_OUT GPIO_Init(I2C_PORT, I2C_SDA_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
#define I2C_SDA_IN GPIO_Init(I2C_PORT, I2C_SDA_PIN, GPIO_MODE_IN_FL_NO_IT);

void SimIIC_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
int I2C_Wait_Ack(void);
void I2C_Send_Byte(int txd);
int I2C_Read_Byte();
void I2C_NAck(void);

#endif