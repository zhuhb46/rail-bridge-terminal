#ifndef _ringBuffer_H
#define _ringBuffer_H
#define NULL 0
#include "stm8s.h"

void initRingbuffer(void);
int wirteRingbuffer(uint8_t *buffer, uint8_t len);
int readRingbuffer(uint8_t *buffer, uint8_t len);
uint16_t getRingbufferValidLen(void);
void releaseRingbuffer(void);

#endif /* RINGBUFFER_H_ */
