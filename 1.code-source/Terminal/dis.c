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
                /*
                数据空间：disDataBuf[]
                原始数据：AB AD 00 00 00 11 AF AF FB FF 1D 00 0F 01 00 00 00 01 00 00 FF 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 FF 00 00 00 00 00 00 FF 00 00 01 00 00 00 00 00 FF 00 00 00 00 00 FF 00 00 01 00 00 00 00 00 00 00 00 00 FF 00 00 01 01 00 FF 00 00 00 00 00 00 00 00 00 00 00 00 00 AE AE
                数据拆分：
                AB AD
                00
                00 00 11

                
                AF AF
                FB FF 1D 00 0F 01
                00 00 00
                01 00 00
                FF 01 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                00 00 00
                01 00 00
                00 00 00
                00 00 00
                00 00 00
                FF 00 00
                00 00 00
                00 FF 00
                00 01 00
                00 00 00
                00 FF 00
                00 00 00
                00 FF 00
                00 01 00
                00 00 00
                00 00 00
                00 00 FF
                00 00 01
                01 00 FF
                00 00 00
                00 00 00
                00 00 00
                00 00 00

                00 AE AE




                */
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