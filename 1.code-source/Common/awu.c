#include "stm8s.h"
#include "awu.h"

void MYAWU_Init(uint8_t APR_VALUE)

{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, ENABLE); //初始化AWU的时钟
    AWU_DeInit();
    /* Check parameter */
    assert_param(IS_AWU_TIMEBASE_OK(AWU_TIMEBASE_12S));
    /* Enable the AWU peripheral */
    AWU->CSR |= AWU_CSR_AWUEN;
    /* Set the TimeBase */
    AWU->TBR &= (uint8_t)(~AWU_TBR_AWUTB);
    AWU->TBR |= 12;
    /* Set the APR divider */
    AWU->APR &= (uint8_t)(~AWU_APR_APR);
    AWU->APR |= APR_VALUE;
    CLK_LSICmd(ENABLE);         //这个必须使能
    AWU_Cmd(ENABLE);
}
