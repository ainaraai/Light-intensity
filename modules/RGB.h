#ifndef _RGB_H
#define _RGB_H

#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"

//#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
typedef struct {                                // object data type
 	uint8_t blue;
	uint8_t green;
	uint8_t red;
	double pulse;
}MSGQUEUE_RGB_t;


osThreadId_t getModRGBQueueID(void);
void initModRGB(void);


#endif
