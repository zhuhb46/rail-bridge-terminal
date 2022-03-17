#include "timer4.h"

uint8_t delay_tim4_1_2ms = 0;
uint8_t timer4_flag = 1;

void TIMER4_Init(void)
{
    TIM4->CR1 = 0x00;
    TIM4->IER = 0x00;
    TIM4->CNTR = 0x00;
    TIM4->PSCR = 0x00;
    TIM4->ARR = 0xFF;
    TIM4->SR1 = 0x00;
    TIM4->PSCR = 0x07; //预分频 128
    TIM4->ARR = 0x40;  //转载值 64
    TIM4->SR1 = (uint8_t)(~0x01);
    TIM4->IER |= 0x01;
    TIM4->CR1 |= 0x01; //定时器4使能
    // TIM4->CR1 &= 0xFE;  //定时器4去使能
}
