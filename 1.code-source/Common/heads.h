#ifndef _heads_H
#define _heads_H

#include "stm8s.h"
#include "usart.h"
#include "timer1.h"
#include "timer2.h"
#include "timer4.h"
#include "ADXL345.h"
#include "iic.h"
#include "adc.h"
#include "LoRa.h"
#include "IWDG.h"
#include "awu.h"
#include "global.h"
#include "stm8s_it.h"
#include "ringBuffer.h"

#define LED_GPIO_PORT (GPIOD)
#define LED_GPIO_PINS (GPIO_PIN_4)
#define DataLen 120
#define DEFAULTGROUP 5 //默认的组别
//组别编号 ： 0   1   2   3   4   5  ········
//目标地址 ： 40  41  42  43  44  45 ········
//本地地址 ： 29  30
#define TOTALGROUP 11 //默认的组别总数
#define MYADDR 34
#define CPA_ADDR "AT+ADDR=34\r\n"
#define CPA_CHEL "AT+CH=34\r\n"

extern uint16_t AWU_Status_Flag;
extern uint8_t sleeptime;
extern u32 gTimer1_Delay;
extern int cnt; //数据包计数
extern bool isTimeToReadAdxl345;
extern uint8_t getAddr;
extern uint8_t CountError;
extern uint8_t AverageBuf[8][6]; //计算平均数组
extern uint8_t idxBufWrite;
extern uint8_t idxBufRead;
extern uint8_t idxHistory;
extern bool dataCollectDone;
extern bool dataReady[];
extern u16 voltage;
extern uint8_t myChipIdBuf[12];
extern int ms_1;
extern uint8_t times;
extern uint16_t wakeTime; // RTC苏醒次数
extern uint8_t myringbuffer[120];
extern uint16_t ringDataLen;
extern bool callBack;
extern uint8_t recv_sleeptime;
extern uint8_t recv_recontime;
extern uint16_t workTimeReport;
extern uint16_t wakeCnt;
extern uint8_t workState;
extern char FeedBack[30];
extern uint8_t RecvCnt;

typedef struct _timer1_clock
{
    uint8_t SencondClock;
    uint8_t minuteClock;
    uint8_t hourClock;
    uint32_t SencondCount;
} timer1_clock;
extern timer1_clock myclock;

#endif