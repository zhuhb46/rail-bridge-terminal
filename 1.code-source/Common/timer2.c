#include "timer2.h"

void TIMER2_Init(void)
{
    TIM2->CR1 = 0x00;
    TIM2->IER = 0x00;
    TIM2->SR2 = 0x00;
    TIM2->CCER1 = 0x00;
    TIM2->CCER2 = 0x00;
    TIM2->CCER1 = 0x00;
    TIM2->CCER2 = 0x00;
    TIM2->CCMR1 = 0x00;
    TIM2->CCMR2 = 0x00;
    TIM2->CCMR3 = 0x00;
    TIM2->CNTRH = 0x00;
    TIM2->CNTRL = 0x00;
    TIM2->PSCR = 0x00;
    TIM2->ARRH = 0xFF;
    TIM2->ARRL = 0xFF;
    TIM2->CCR1H = 0x00;
    TIM2->CCR1L = 0x00;
    TIM2->CCR2H = 0x00;
    TIM2->CCR2L = 0x00;
    TIM2->CCR3H = 0x00;
    TIM2->CCR3L = 0x00;
    TIM2->SR1 = 0x00;

    TIM2->PSCR = 0x07;
    TIM2->ARRH = 0x00;
    TIM2->ARRL = 0x7c;

    TIM2->SR1 = 0xFE;
    TIM2->SR2 = 0xFF;

    TIM2->IER |= 0x01;

    //TIM2->CR1 |= 0x01;  //定时器2使能
    TIM2->CR1 &= 0xFE; //定时器2去使能
}
