#ifndef _LoRa_H
#define _LoRa_H

#include "stm8s.h"

void LoRaSendData(uint8_t *ID, uint8_t Channel, uint8_t TempIdx, uint8_t idx, uint8_t *msg, uint8_t *Data, uint8_t DataLen);
void LoRaSendWakeup(uint8_t *ID, uint8_t Channel, uint8_t *myID, uint8_t myChannel, uint8_t *ChipID, uint16_t voltage, uint8_t errorcnt, uint16_t wakecnt);
void LoRaSendWork(uint8_t *ID, uint8_t Channel);
uint8_t LoRaInit(char *compare_ADDR, char *compare_CHEL, char *FeedBack, uint8_t *recvnct);
void print_LoRa_env(char *FeedBack, uint8_t *recvnct);
void USART_SendString(char *str);

#endif