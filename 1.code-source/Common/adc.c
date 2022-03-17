#include "adc.h"
void MYAdc_Init(uint8_t channel)
{
  // GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
  // ADC1_DeInit();
  // ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_2, ADC1_PRESSEL_FCPU_D2, ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL2, ENABLE);
  // ADC1_Cmd(ENABLE);

  GPIOC->CR2 &= ~(0x10);
  GPIOC->DDR &= ~(0x10);
  GPIOC->CR1 &= ~(0x10);
  GPIOC->CR2 &= ~(0x10);

  ADC1->CSR = 0x00;
  ADC1->CR1 = 0x00;
  ADC1->CR2 = 0x00;
  ADC1->CR3 = 0x00;
  ADC1->TDRH = 0x00;
  ADC1->TDRL = 0x00;
  ADC1->HTRH = 0xFF;
  ADC1->HTRL = 0x03;
  ADC1->LTRH = 0x00;
  ADC1->LTRL = 0x00;
  ADC1->AWCRH = 0x00;
  ADC1->AWCRL = 0x00;
  ADC1->CR2 &= ~0x08;
  ADC1->CR2 |= 0x08;
  ADC1->CR1 &= ~0x02;
  ADC1->CSR &= ~0x0F;
  ADC1->CSR |= 0x02;
  ADC1->CR1 &= ~0x70;
  ADC1->CR1 |= 0x00;
  ADC1->CR2 &= ~0x30;
  ADC1->CR2 &= ~0x40;
  ADC1->CR2 |= 0x00;
  ADC1->TDRL &= (uint8_t)(~(uint8_t)((uint8_t)0x01 << 0x02));
  ADC1->CR1 |= 0x01; //ADC1使能

  // ADC1->CR1 &= ~0x01;  //ADC1去使能
}

uint16_t getADCValue(uint8_t channel)
{

  uint8_t flagstatus = 0;
  uint16_t temph = 0;
  uint8_t templ = 0;
  ADC1->CR1 |= 0x01; //开始转换*/
  // while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET)
  //   ;
  while (flagstatus == 0)
  {
    flagstatus = (uint8_t)(ADC1->CSR & 0x80);
  }
  templ = ADC1->DRL;
  temph = ADC1->DRH;
  temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
  return ((uint16_t)temph);
}
