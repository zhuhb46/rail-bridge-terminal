#include "stm8s.h"
#include "ADXL345.h"
#include "iic.h"
#include "usart.h"

int Single_Read_ADXL345(int addr, int ADDRESS)
{
	int temp = 0;
	I2C_Start();
	I2C_Send_Byte(ADDRESS);
	I2C_Wait_Ack();
	I2C_Send_Byte(addr);
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte(ADDRESS + 1);
	I2C_Wait_Ack();
	temp = I2C_Read_Byte();
	I2C_NAck();
	I2C_Stop();
	return temp;
}

void Single_Write_ADXL345(int addr, int dt, int ADDRESS)
{
	I2C_Start();
	I2C_Send_Byte(ADDRESS);
	I2C_Wait_Ack();
	I2C_Send_Byte(addr);
	I2C_Wait_Ack();
	I2C_Send_Byte(dt);
	I2C_Wait_Ack();
	I2C_Stop();
}

void Multiple_read_ADXL345(int ADDRESS, unsigned char BUF[])
{
	int i;
	I2C_Start();			//?eê?D?o?
	I2C_Send_Byte(ADDRESS); //·￠?íéè±?μ??·+D′D?o?
	I2C_Wait_Ack();
	I2C_Send_Byte(0x32); //·￠?í′?′￠μ￥?aμ??·￡?′ó0x32?aê?
	I2C_Wait_Ack();
	I2C_Start();				//?eê?D?o?
	I2C_Send_Byte(ADDRESS + 1); //·￠?íéè±?μ??·+?áD?o?
	I2C_Wait_Ack();
	for (i = 0; i < 6; i++) //á?D??áè?6??μ??·êy?Y￡?′?′￠?DBUF
	{
		BUF[i] = I2C_Read_Byte(); //BUF[0]′?′￠0x32μ??·?Dμ?êy?Y
		if (i == 5)
		{
			I2C_NAck(); //×?oóò???êy?YDèòa??NOACK
		}
		else
		{
			I2C_Ack(); //??ó|ACK
		}
	}
	I2C_Stop(); //í￡?1D?o?
}

void Adxl_Init()
{

	Single_Write_ADXL345(0x31, 0x0B, 0xA6); //2aá?·??§,?y?o16g￡?13???￡ê?
	Single_Write_ADXL345(0x2C, 0x0F, 0xA6); //?ù?êéè?¨?a12.5 2???pdf13ò3
	Single_Write_ADXL345(0x2D, 0x08, 0xA6); //????μ??′?￡ê?   2???pdf24ò3
	Single_Write_ADXL345(0x2E, 0x80, 0xA6); //ê1?ü DATA_READY ?D??
	Single_Write_ADXL345(0x1E, 0x00, 0xA6); //X ??ò?á? ?ù?Y2aê?′??D?÷μ?×′ì?D′è?pdf29ò3
	Single_Write_ADXL345(0x1F, 0x00, 0xA6); //Y ??ò?á? ?ù?Y2aê?′??D?÷μ?×′ì?D′è?pdf29ò3
	Single_Write_ADXL345(0x20, 0x05, 0xA6); //Z ??ò?á? ?ù?Y2aê?′??D?÷μ?×′ì?D′è?pdf29ò3
}
