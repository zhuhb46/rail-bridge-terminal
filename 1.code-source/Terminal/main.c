
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "heads.h"
#include "string.h"

void SYSCLK_Init(void)
{
  CLK->CKDIVR |= CLK_PRESCALER_CPUDIV1;                // CPU 时钟分频 1，CPU时钟 = 外部时钟(即是外部晶振频率)
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,           // 时钟自动切换模式，注: 参数是自动切换，不然只使用这一个函数是不能切换成功的
                        CLK_SOURCE_HSE,                // 要切换的时钟(这里是外部时钟)
                        DISABLE,                       // 是否使能切换完成中断(这里失能)
                        CLK_CURRENTCLOCKSTATE_ENABLE); // 是否还使能当前时钟(这里选择 关掉当前时钟HSI)
  CLK->SWCR &= ~(CLK_SWCR_SWEN | CLK_SWCR_SWIF);       // 清除切换标志
}

void Get_ChipID(u8 *ChipID)
{
  u8 i = 0;
  for (i = 0; i < 12; i++)
    ChipID[i] = *(u8 *)(0X4865 + i);
}

void main(void)
{
  /******************************************************************/
  /* 系统初始化 */
  SYSCLK_Init(); //配置系统时钟，外部晶振16Mhz
  USART1_Init(); //配置串口115200
  TIMER1_Init();
  TIMER2_Init(); //配置定时器2用于发送数据
  TIMER4_Init(); //配置定时器4用于采集数据
  SimIIC_Init(); //配置IIC
  // MYAWU_Init(39); //配置闹钟
  MYAdc_Init(8); //配置ADC
  Adxl_Init();
  Addr_Init();
  asm("rim");

  /******************************************************************/
  /* GPIO初始化配置 */
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(GPIOA, GPIO_PIN_3); // LoRa开
  GPIO_WriteHigh(GPIOC, GPIO_PIN_5); // ADXL开

  /******************************************************************/
  /* 开启UART接收 */
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  workState = 0;

  /******************************************************************/
  /* 查询本机ID */
  for (uint8_t i = 0; i < 10; i++)
  {
    delay_ms(200);
  }
  Get_ChipID(myChipIdBuf);  //获取本机chipID
  AWU_Status_Flag = 20;     //初始化休眠时间为20s
  bool bSuccess = FALSE;    //判断是否成功完成传输
  uint16_t restartcnt = 48; //用于触发重启
  initRingbuffer();         //缓冲区初始化
  TIM4->CR1 |= 0x01;        //定时器4使能

  /******************************************************************/
  /* 初始化LoRa */
  uint8_t setLoRa = LoRaInit(CPA_ADDR, CPA_CHEL, FeedBack, &RecvCnt);
  print_LoRa_env(FeedBack, &RecvCnt);
  memset(FeedBack, 0, 30);
  USART_SendString("AT+ENTM\r\n");
  RecvCnt = 0;
  delay_ms(255);
  USART_SendString(FeedBack);
  workState = 1;
  while (setLoRa == 88)
  {
    GPIO_WriteLow(GPIOD, GPIO_PIN_4); // LED开
    for (uint8_t i = 0; i < 5; i++)
    {
      delay_ms(200);
    }
    GPIO_WriteHigh(GPIOD, GPIO_PIN_4); // LED关
    for (uint8_t i = 0; i < 5; i++)
    {
      delay_ms(200);
    }
    workState++;
    if (workState == 5)
      break;
  }
  GPIO_WriteLow(GPIOA, GPIO_PIN_3); // LoRa关

  /******************************************************************/

  /* 主循环 */
  IWDG_Init(); //配置看门狗
  myclock.SencondCount = 0;
  workTimeReport = 0;
  while (1)
  {
    workTimeReport = 0;               //开始计时
    IWDG_ReloadCounter();             //喂狗
    GPIO_WriteLow(GPIOD, GPIO_PIN_4); // LED开
    if (!bSuccess)                    //判断上次是否连接成功
    {
      IWDG_ReloadCounter();
      if (recv_recontime != 0) //配置下一次苏醒时间
      {
        AWU_Status_Flag = 10 * (recv_recontime & 0x0f);
      }
      else
        AWU_Status_Flag = 20; // default值
      restartcnt = 1200;      //无法连接情况下1200次苏醒即重启
    }

    /****************************************/
    /**工作周期**/
    if (wakeTime == 0)
    {
      CountError++;
      wakeCnt++;
      if (wakeCnt >= restartcnt) //触发重启
      {
        wakeCnt = 0;
        while (1)
        {
        }
      }
      IWDG_ReloadCounter();
      GPIO_WriteHigh(GPIOC, GPIO_PIN_5); // ADXL开
      bSuccess = FALSE;
      ADC1_Cmd(ENABLE);                          //开启ADC采集
      UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE); //初始化定时器但不打开接收中断
      Adxl_Init();                               //初始化传感器

      /****************************************/
      /**初始化数据**/
      int16_t averangeData[3] = {0};
      int16_t presentData[3] = {0};
      uint8_t averangeCnt = 0;
      dataCollectDone = FALSE;
      idxBufWrite = 0;
      idxBufRead = 0;
      IWDG_ReloadCounter();
      voltage = getADCValue(8); //采集电压
      initRingbuffer();         //缓冲区初始化
      idxHistory = 0;
      idxBufWrite = 0;
      getAddr = 0; //初始化getAddr使其进入握手模式
      recv_sleeptime = 0;
      ms_1 = 0;
      times = 0;
      UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
      TIM2->CR1 |= 0x01; //定时器2使能
      TIM4->CR1 |= 0x01; //定时器4使能
      /****************************************/
      /**开始采集数据**/
      int16_t beforeData[3] = {0}; //上一次数据
#define disDataLen 120
      uint8_t disDataBuf[disDataLen] = {0}; //存差分数据的数据包
      uint8_t disCnt = 2;                   //差分计算计数
      while (getAddr != 4 && getAddr != 5)
      {
        IWDG_ReloadCounter();
        disDataBuf[0] = 0xAF;
        disDataBuf[1] = 0xAF;
        if (isTimeToReadAdxl345) //采集中断触发
        {
          IWDG_ReloadCounter();
          isTimeToReadAdxl345 = FALSE;
          Multiple_read_ADXL345(ADDRESS1, &AverageBuf[averangeCnt][0]);                                 //采集单点数据
          presentData[0] = ((AverageBuf[averangeCnt][1] & 0xFFFF) << 8) + (AverageBuf[averangeCnt][0]); //数据类型转换
          presentData[1] = ((AverageBuf[averangeCnt][3] & 0xFFFF) << 8) + (AverageBuf[averangeCnt][2]);
          presentData[2] = ((AverageBuf[averangeCnt][5] & 0xFFFF) << 8) + (AverageBuf[averangeCnt][4]);
          if (averangeCnt == 0)
          {
            IWDG_ReloadCounter();
            averangeData[0] = presentData[0];
            averangeData[1] = presentData[1];
            averangeData[2] = presentData[2];
          }
          else
          {
            IWDG_ReloadCounter();
            averangeData[0] = ((averangeData[0] >> 1) + (presentData[0] >> 1));
            averangeData[1] = ((averangeData[1] >> 1) + (presentData[1] >> 1));
            averangeData[2] = ((averangeData[2] >> 1) + (presentData[2] >> 1));
          }
          averangeCnt++;
          if (averangeCnt == 7) //多次平均，采集完7次即存储数据
          {
            if (disCnt == 2)
            {
              /* 存head数据 */
              disDataBuf[2] = (averangeData[0] & 0xFF);
              disDataBuf[3] = ((averangeData[0] >> 8) & 0xFF);
              disDataBuf[4] = (averangeData[1] & 0xFF);
              disDataBuf[5] = ((averangeData[1] >> 8) & 0xFF);
              disDataBuf[6] = (averangeData[2] & 0xFF);
              disDataBuf[7] = ((averangeData[2] >> 8) & 0xFF);
              disCnt = 8;
            }
            else
            {
              for (uint8_t i = 0; i < 3; i++)
              {
                disDataBuf[disCnt] = ((averangeData[i] - beforeData[i]) / 4);
                disCnt++;
              }
            }
            if (disCnt >= disDataLen)
            {
              /* 存满一个数据包 */
              disCnt = 2;
              wirteRingbuffer(disDataBuf, disDataLen); //数据存入缓冲区
              IWDG_ReloadCounter();
              ringDataLen = getRingbufferValidLen(); //判断缓冲区数据长度
              if (ringDataLen >= disDataLen)         //数据长度满足一个数据包，及可供发送使用
              {
                GPIO_WriteHigh(GPIOA, GPIO_PIN_3); // LoRa开
                dataCollectDone = TRUE;
              }
            }
            averangeCnt = 0;
            memcpy(beforeData, averangeData, sizeof(averangeData));
          }
          IWDG_ReloadCounter();
        }
      }

      /****************************************/
      /**去使能设备**/
      ADC1_Cmd(DISABLE);
      TIM2->CR1 &= 0xFE;   //定时器2去使能
      TIM4->CR1 &= 0xFE;   //定时器4去使能
      releaseRingbuffer(); //缓冲区释放
      IWDG_ReloadCounter();
      //      UART1_sendstr(ADXL_Stop);
      if (idxHistory >= 13) //数据发送完成，配置休眠周期
      {
        IWDG_ReloadCounter();
        if ((recv_sleeptime != 0) && (recv_sleeptime != 0x6B))
        {
          AWU_Status_Flag = recv_sleeptime * 20 - workTimeReport;
        }
        else
          AWU_Status_Flag = 1800 - workTimeReport;
        restartcnt = 24;
        if (wakeCnt >= restartcnt)
          wakeCnt = 0;
        CountError = 0;
        bSuccess = TRUE;
        dataCollectDone = FALSE;
      }
    }
    /****************************************/
    /**去使能外设**/
    IWDG_ReloadCounter();
    UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);
    GPIO_WriteLow(GPIOA, GPIO_PIN_3);  // LoRa关
    GPIO_WriteLow(GPIOC, GPIO_PIN_5);  // ADXL关
    GPIO_WriteHigh(GPIOD, GPIO_PIN_4); // LED关
    IWDG_ReloadCounter();
    /******************************************************************/
    /**进入休眠**/
    TIM2->CR1 &= 0xFE; //定时器2去使能
    while (myclock.SencondCount < AWU_Status_Flag)
    {
      IWDG_ReloadCounter();
      wfi();
    }
    myclock.SencondCount = 0;
    workTimeReport = 0;
    /******************************************************************/
    /**开机初始化**/
    SYSCLK_Init();
    USART1_Init();
    TIMER2_Init();
    TIMER4_Init();
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval : None
 */
void assert_failed(u8 *file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
