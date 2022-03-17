#include "stm8s_it.h"
#include "heads.h"

uint8_t myAddr[2] = {0x00, MYADDR}; //本机LoRa地址
uint8_t myChannel = MYADDR;         //本机LoRa信道

uint8_t preamble[4] = "work";              // 77 6F 72 6B
uint8_t ackbuf[4] = "ackb";                // 61 63 6B 62
uint8_t timesOutCnt = 4;                   //重连尝试次数
uint8_t concentraterAddr[TOTALGROUP][2];   //目标协调器地址
uint8_t concentraterChannel[TOTALGROUP];   //目标协调器信道
uint8_t concentraterchoice = DEFAULTGROUP; //默认协调器
//组别编号 ： 0   1   2   3   4   5  ········
//目标地址 ： 40  41  42  43  44  45 ········
//本地地址 ： 29  30

void Addr_Init(void)
{
  for (int i = 0; i < TOTALGROUP; i++)
  {
    /* 配置目标协调器 */
    concentraterAddr[i][0] = 0x00;
    concentraterAddr[i][1] = 40 + i;
    concentraterChannel[i] = 40 + i;
  }
}

uint16_t AWU_Status_Flag = 0; //休眠时长
int getnum = 0;               //串口接收字节计数
uint8_t getAddr = 0;          //串口状态标志
uint8_t buf[25] = {0};        //串口数据缓冲区
uint16_t wakeTime = 0;        //苏醒计时用于开机
uint16_t wakeCnt = 0;         //苏醒计数用于重启
uint8_t shortAddr[3] = {0};   //短地址
uint8_t myMAC[9] = {0};
uint8_t CoCtrl[4] = {0};
bool isTimeToReadAdxl345 = FALSE;
uint8_t cntSampling = 0; //数据采集计数
uint8_t CountError = 0;  //失败计数

uint8_t myChipIdBuf[12] = {0}; //本机id存储
uint8_t myTempIdx = 0;         //本机被分配的牌照
uint8_t idxHistory = 0;        //已发送成功的数据包计数

int cnt = 0xFF;
int ms_1 = 0;
uint8_t times = 0;
uint8_t AverageBuf[8][6] = {0};
uint8_t idxBufWrite = 0;
uint8_t idxBufRead = 0;
bool dataCollectDone;
bool callBack = FALSE;
bool TimeOut = FALSE;
uint16_t voltage;
uint16_t ringDataLen = 0;
uint8_t msg[2] = {0};
uint8_t sendDatabuf[DataLen] = {0};
uint8_t recv_sleeptime = 0;
uint8_t recv_recontime = 0;
uint16_t workTimeReport = 0;
timer1_clock myclock;

uint8_t workState = 0;
char FeedBack[30] = {0};
uint8_t RecvCnt = 0;

#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8AF62Ax) || defined(STM8AF52Ax) || defined(STM8S903)
/**
 * @brief UART1 TX Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
 * @brief UART1 RX Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  buf[getnum] = UART1_ReceiveData8(); //获取串口数据

  if (workState == 0)
  {
    if (RecvCnt >= 30)
      RecvCnt = 0;
    FeedBack[RecvCnt] = UART1_ReceiveData8(); //获取串口数据
    RecvCnt++;
  }

  bool bNextState = FALSE;
  switch (getAddr)
  {
  case 0: //握手work
  {
    if ((getnum <= 20) && (getnum > 15)) //配置数据段
    {
      if (20 == getnum) //接收完成
      {
        myTempIdx = buf[16];      //存储本机被分配的牌照
        recv_sleeptime = buf[17]; //存储休眠时长
        recv_recontime = buf[18]; //失败后再次尝试的时长
        bNextState = TRUE;        //更新状态机
        callBack = TRUE;          //准备发送数据
        ms_1 = 0;
        getnum = 0;               //初始化串口
      }
      else
        getnum++;
    }
    else if ((getnum <= 15) && (getnum > 3)) //判断是否为本机数据
    {
      if (getnum <= 11)
      {
        if (myChipIdBuf[getnum - 4] == buf[getnum])
        {
          getnum++;
        }
        else
        {
          getnum = 0;
        }
      }
      else
        getnum++;
    }
    else if (getnum <= 3) //判断数据包类型
    {
      if (preamble[getnum] == buf[getnum])
      {
        getnum++;
      }
      else
      {
        getnum = 0;
      }
    }
  }
  break;

  case 1:
  {
    if (16 == getnum) //数据接收完毕
    {
      idxHistory = buf[getnum]; //记录协调器成功接受的数据包编号
      idxHistory++;             //发送下一个数据包
      if (idxHistory >= 14)     //数据包发送完成，进入下一状态
      {
        bNextState = TRUE;
      }
      callBack = TRUE;
      getnum = 0;
      buf[0] = 0;
      times = 0;
      ms_1 = 0;
    }
    else if ((getnum <= 15) && (getnum > 3)) //判断是否为本机数据
    {
      if (getnum <= 11)
      {
        if (myChipIdBuf[getnum - 4] == buf[getnum])
        {
          getnum++;
        }
        else
        {
          getnum = 0;
        }
      }
      else
        getnum++;
    }
    else if (getnum <= 3) //判断数据类型
    {
      if (ackbuf[getnum] == buf[getnum])
      {
        getnum++;
      }
      else
      {
        getnum = 0;
      }
    }
  }
  break;

  default:
    getAddr = 5;
  }

  if (bNextState) //更新状态机
  {
    getAddr++;
    getnum = 0;
    buf[0] = 0;
    times = 0;
    ms_1 = 0;
    TimeOut = FALSE;
  }

  UART1_ClearITPendingBit(UART1_IT_RXNE);
}
#endif /*STM8S208 or STM8S207 or STM8S103 or STM8S903 or STM8AF62Ax or STM8AF52Ax */

/**
 * @brief Timer2 Update/Overflow/Break Interrupt routine.
 * @param  None
 * @retval None
 */

INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  ms_1++;
  if ((ms_1 == 400) && (dataCollectDone == TRUE))
  {
    switch (getAddr)
    {
    case 0: //向协调器发送苏醒信息
      LoRaSendWakeup(concentraterAddr[concentraterchoice], concentraterChannel[concentraterchoice], myAddr, myChannel, myChipIdBuf, voltage, CountError, wakeCnt);
      timesOutCnt = 1;
      break;

    case 1: //向协调器发送数据
      timesOutCnt = 3;
      if (TimeOut == TRUE) //协调器成功未接收，重发历史数据
      {
        msg[1]++;
        LoRaSendData(concentraterAddr[concentraterchoice], concentraterChannel[concentraterchoice], myTempIdx, idxHistory, msg, sendDatabuf, DataLen);
        callBack = FALSE;
        TimeOut = FALSE;
        ms_1 = 4200;
      }
      if (callBack == TRUE) //向协调器发送新数据
      {
        ringDataLen = getRingbufferValidLen(); //判断数据库是否满足发送长度
        if (ringDataLen > 119)
        {
          msg[1]++;
          readRingbuffer(sendDatabuf, 120);
          LoRaSendData(concentraterAddr[concentraterchoice], concentraterChannel[concentraterchoice], myTempIdx, idxHistory, msg, sendDatabuf, DataLen);
          callBack = FALSE;
          TimeOut = FALSE;
          ms_1 = 4200;
        }
        else
        {
          ms_1 = 0;
        }
      }
      break;

    default:
      getAddr = 5;
    }
  }
  else if (ms_1 > 5000) //等待超时，更新状态信息
  {
    TimeOut = TRUE;
    ms_1 = 0;
    getnum = 0;
    buf[0] = 0;

    if (times >= timesOutCnt) //如果多次尝试失败，则切换协调器
    {
      getAddr = 5;
      times = 0;
      // if (CountError > 180)
      // {
      //  if (concentraterchoice >= (TOTALGROUP - 1))
      //  {
      //    concentraterchoice = 0;
      //  }
      //  else
      //    concentraterchoice++;
      // }
    }
    times++;
  }
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
}

/**
 * @brief Auto Wake Up Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  AWU_GetFlagStatus();
  if (wakeTime == AWU_Status_Flag) //苏醒控制
    wakeTime = 0;
  else
    wakeTime++;
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  if (delay_tim4_1_2ms < 200)
    delay_tim4_1_2ms++;
  else
    timer4_flag = 0;
  cntSampling++;
  if (cntSampling > SAMPLE_INTERVAL) //传感器数据采集标志更新
  {
    cntSampling = 0;
    isTimeToReadAdxl345 = TRUE;
  }
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

/**
 * @brief Timer1 Update/Overflow/Trigger/Break Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  workTimeReport++;
  myclock.SencondCount++;
  myclock.SencondClock++;
  if (myclock.SencondClock == 60)
  {
    myclock.SencondClock = 0;
    myclock.minuteClock++;
    if (myclock.minuteClock == 60)
    {
      myclock.minuteClock = 0;
      myclock.hourClock++;
      if (myclock.hourClock == 24)
      {
        myclock.hourClock = 0;
      }
    }
  }

  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/