#ifndef __POT_H
#define __POT_H

#include "cmsis_os2.h" 
#include "stm32f4xx_hal.h"


#define SIZE_MSGQUEUE_P		3

typedef struct {                               
  float pot1;
	float pot2;
} MSGQUEUE_POT_t;


void initModPOT(void);

osMessageQueueId_t getModPOTQueueID(void);

void Init_POT_Test(void);

#endif
