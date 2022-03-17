#ifndef _ADXL345_H //宏定义，定义文件名称
#define _ADXL345_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm8s.h"
#define ADDRESS1 0xA6
#define ADDRESS2 0x3A

int Single_Read_ADXL345(int addr, int ADDRESS);
void Single_Write_ADXL345(int addr, int dt, int ADDRESS);
void Multiple_read_ADXL345(int ADDRESS, unsigned char BUF[]);
void Adxl_Init();

#endif