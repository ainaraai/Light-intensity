#ifndef _PWM_H
#define _PWM_H

#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"

#define PULSADO 0x00000002U


//#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
typedef struct {                                // object data type
 
  double pulse;
	uint8_t start;
} MSGQUEUE_PWM_t;
 



osMessageQueueId_t getModPWMQueueID(void);

void initModPWM(void);
void Init_PWM_Test(void);

osThreadId_t getModPWMThreadID(void);

#endif
