#ifndef _COMPC_H
#define _COMPC_H

#include "cmsis_os2.h"                        
#include <stdio.h>
#include "Driver_USART.h"
#include "stm32f4xx_hal.h"


#define MSGQUEUE_OBJECTS 6       

typedef struct {  
	char linea[13];
	char lineaRecibida[13];
	uint8_t send;
} MSGQUEUE_COMPC_t;

void Init_Th_COMPC_Test(void);
void initModCOMPC(void);

osMessageQueueId_t getModCOMQueueID(void);

#endif
