#ifndef _adc_H
#define _adc_H
#include "stm8s.h"

void MYAdc_Init(uint8_t channel);
uint16_t getADCValue(uint8_t channel);

#endif