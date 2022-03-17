//******************************************************************************
// name:         timer1.h
// introduce:    定时器驱动的头文件
// author:       甜甜的大香瓜
// changetime:   2016.05.10
// email:        897503845@qq.com
//******************************************************************************
#ifndef __TIMER1_H
#define __TIMER1_H

/*********************头文件************************/
#include "stm8s.h"

/*********************外部函数声明************************/
void TIMER1_Init(void);
void delay_ms(uint16_t ms);
void TIMER1_Decrement(void);
void TIMER1_Delay50Us(u32 nTime);
void TIMER1_Delay50Ms(u32 nTime);

#endif