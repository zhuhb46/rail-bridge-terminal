#include "stm8s.h"
#include "IWDG.h"

void IWDG_Init(void)

{
  IWDG->KR = IWDG_KEY_ENABLE;
  IWDG->KR = (uint8_t)0x55;
  IWDG->PR = (uint8_t)0x06;
  IWDG->RLR = 255;
  IWDG->KR = IWDG_KEY_REFRESH;

  // IWDG_Enable();
  // IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  // IWDG_SetPrescaler(IWDG_Prescaler_256);
  // IWDG_SetReload(255);
  // IWDG_ReloadCounter();
}
