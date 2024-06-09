#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h" 
#include "stm32f4xx_hal.h"

#define SIZE_MSGQUEUE_JOYSTICK			3
#define REBOTE 0x00000002U

typedef struct {                               
  uint8_t tipo; //0: corta 1:larga
	uint8_t pulsacion; //1 bit activo por pulsacion
} MSGQUEUE_Joystick_t;
		//0x01: ARRIBA
		//0X02: DERECHA
		//0X03: ABAJO
		//0X04: IZQUIERDA
		//0X05: CENTRO


void initModJoystick(void);

osMessageQueueId_t getModJoystickQueueID(void);

void Init_Th_Joystick_Test(void);

#endif
