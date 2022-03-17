#include "timer1.h"
#include "timer4.h"


/*********************内部变量************************/
u32 gTimer1_Delay;
uint8_t second_01 = 0;
uint8_t sleeptime = 0;
void TIMER1_Init(void)
{
  TIM1_TimeBaseInit(4000 - 1, TIM1_COUNTERMODE_UP, 4000 - 1, 0); // 1分频，向上计数，每50us定时中断一次， 重复计数器值为0
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_Cmd(ENABLE);
}

void delay_ms(uint16_t ms)
{
  for (uint16_t i = 0; i < 32000; i++)
  {
    delay_tim4_1_2ms++;
    timer4_flag = 0;
    for (uint16_t j = 0; j < ms*300; j++)
    {
      delay_tim4_1_2ms++;
      timer4_flag = 1;
      delay_tim4_1_2ms--;
    }
  }
}

void TIMER1_Decrement(void)
{
  if (gTimer1_Delay != 0x00)
  {
    gTimer1_Delay--;
  }
}

void TIMER1_Delay50Us(u32 nTime)
{
  gTimer1_Delay = nTime;

  while (gTimer1_Delay != 0)
    ;
}

void TIMER1_Delay50Ms(u32 nTime)
{
  for (int i = 0; i < nTime; i++)
  {
    TIMER1_Delay50Us(1000);
  }
}