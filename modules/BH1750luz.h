#ifndef __BH1750luz_H
#define __BH1750luz_H

#include "cmsis_os2.h" 
#include "Driver_I2C.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

#define SIZE_MSGQUEUE_LZ			3

typedef struct {                               
//  uint8_t nLin;
//	char linea[30];
	uint16_t lx;
} MSGQUEUE_BH_t;


void initModBH(void);

osMessageQueueId_t getModLZQueueID(void);


#endif

