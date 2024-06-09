#include "Joystick.h"

/*--------------------------VARIABLES------------------------------*/
static GPIO_InitTypeDef GPIO_InitStruct;
osMessageQueueId_t mid_MsgQueueJoystick;
osTimerId_t tim_rebotes;                            
osTimerId_t tim_pulsLarga;                            
osThreadId_t tid_Th_Joystick;                        
osThreadId_t tid_Th_Joystick_Test;

uint8_t pulsUP, pulsDOWN, pulsRIGHT, pulsLEFT, pulsCENTER;
uint8_t up, right, down, left, center, upL, rightL, downL, leftL, centerL;

/*--------------------PROTOTIPOS DE FUNCIONES--------------------*/
void Init_MsgQueue_Joystick (void);
osMessageQueueId_t getModJoystickQueueID(void);
static void TimerRebotes_Callback (void const *arg);
void Init_Timer_Rebotes (void) ;
void Start_Timer_Rebotes(void);
static void TimerPulsLarga_Callback (void const *arg);
void Init_Timer_PulsLarga(void);
void Start_Timer_PulsLarga(void);
void Init_Thread_Joystick (void);
void Thread_Joystick (void *argument);                   
void initJoystick(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Th_Joystick_Test (void *argument) ;
/*-------------------------FUNCIONES------------------------------*/

//COLA
void Init_MsgQueue_Joystick (void) {
 
  mid_MsgQueueJoystick = osMessageQueueNew(SIZE_MSGQUEUE_JOYSTICK , sizeof(MSGQUEUE_Joystick_t), NULL);

}
osMessageQueueId_t getModJoystickQueueID(void){
	return mid_MsgQueueJoystick;
}
//TIMER PARA GESTION DE REBOTES
static void TimerRebotes_Callback (void const *arg) {
	  MSGQUEUE_Joystick_t msg;
		if(pulsUP==0 && pulsDOWN==0 && pulsRIGHT==0 && pulsLEFT==0 && pulsCENTER==0){ //flanco subida --> inincio de la pulsación
			Start_Timer_PulsLarga();
			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ //UP
				pulsUP=0x01;
			}else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){ //RIGHT
				pulsRIGHT=0x01;
			}else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){ //DOWN
				pulsDOWN=0x01;
			}else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){ //LEFT
				pulsLEFT=0x01;
			}else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ //CENTER
				pulsCENTER=0x01;
			}
		}else{ //flanco de bajada --> fin pulsación
			if(pulsUP==0x01){
				pulsUP=0x00;
				if(osTimerIsRunning(tim_pulsLarga)){ //pulsación menor a 1s		
					osTimerStop(tim_pulsLarga);
					msg.tipo=0x00;
					msg.pulsacion = 0x01;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}else{
					msg.tipo=0x01;
					msg.pulsacion = 0x01;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}
			}else if(pulsRIGHT==0x01){
				pulsRIGHT=0x00;
				if(osTimerIsRunning(tim_pulsLarga)){ //pulsación menor a 1s		
					osTimerStop(tim_pulsLarga);
					msg.tipo=0x00;
					msg.pulsacion = 0x02;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}else{
					msg.tipo=0x01;
					msg.pulsacion = 0x02;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}
			}else if(pulsDOWN==0x01){
				pulsDOWN=0x00;
				if(osTimerIsRunning(tim_pulsLarga)){ //pulsación menor a 1s		
					osTimerStop(tim_pulsLarga);
					msg.tipo=0x00;
					msg.pulsacion = 0x03;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}else{
					msg.tipo=0x01;
					msg.pulsacion = 0x03;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}
			}else if(pulsLEFT==0x01){
				pulsLEFT=0x00;
				if(osTimerIsRunning(tim_pulsLarga)){ //pulsación menor a 1s		
					osTimerStop(tim_pulsLarga);
					msg.tipo=0x00;
					msg.pulsacion = 0x04;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}else{
					msg.tipo=0x01;
					msg.pulsacion = 0x04;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}
			}else if(pulsCENTER==0x01){
				pulsCENTER=0x00;
				if(osTimerIsRunning(tim_pulsLarga)){ //pulsación menor a 1s		
					osTimerStop(tim_pulsLarga);
					msg.tipo=0x00;
					msg.pulsacion = 0x05;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}else{
					msg.tipo=0x01;
					msg.pulsacion = 0x05;                                        
					osMessageQueuePut(mid_MsgQueueJoystick, &msg, 0U, 0U);
				}
			}
		}
    
}
void Init_Timer_Rebotes (void) {
  tim_rebotes = osTimerNew((osTimerFunc_t)&TimerRebotes_Callback, osTimerOnce, NULL, NULL);
}
void Start_Timer_Rebotes(void){
	osTimerStart(tim_rebotes, 50); 
}
//TIMER PULSACION LARGA
static void TimerPulsLarga_Callback (void const *arg) {
	//no hace nada
}
void Init_Timer_PulsLarga(void) {
  tim_pulsLarga = osTimerNew((osTimerFunc_t)&TimerPulsLarga_Callback, osTimerOnce, NULL, NULL);
}
void Start_Timer_PulsLarga(void){
	osTimerStart(tim_pulsLarga, 1000); 
}	

//JOSTICK
void Init_Thread_Joystick (void) {
 
  tid_Th_Joystick = osThreadNew(Thread_Joystick, NULL, NULL);

}
 
void Thread_Joystick (void *argument) {

  while (1) {
		osThreadFlagsWait(REBOTE, osFlagsWaitAny, osWaitForever);
		Start_Timer_Rebotes();
    osThreadYield();                            // suspend thread
  }
}

void initJoystick(void){
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		
    //pulsador UP
    //configuración del pin GPIO:PB10 
    GPIO_InitStruct.Pin=GPIO_PIN_10;
    GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //pulsador RIGHT
    //configuración del pin GPIO:PB11 
    GPIO_InitStruct.Pin=GPIO_PIN_11;
    GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//pulsador DOWN
    //configuración del pin GPIO:PE12 
    GPIO_InitStruct.Pin=GPIO_PIN_12;
    GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		
		//pulsador LEFT
    //configuración del pin GPIO:PE14 
    GPIO_InitStruct.Pin=GPIO_PIN_14;
    GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=GPIO_PIN_14;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
		//pulsador CENTER
    //configuración del pin GPIO:PE15
    GPIO_InitStruct.Pin=GPIO_PIN_15;
    GPIO_InitStruct.Mode=GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  }
	

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
      if (GPIO_Pin==GPIO_PIN_10){ //UP
				osThreadFlagsSet(tid_Th_Joystick, REBOTE);
        
      }else if(GPIO_Pin==GPIO_PIN_11){ //RIGHT
				osThreadFlagsSet(tid_Th_Joystick, REBOTE);

      }else if(GPIO_Pin==GPIO_PIN_12){ //DOWN
				osThreadFlagsSet(tid_Th_Joystick, REBOTE);

      }else if(GPIO_Pin==GPIO_PIN_14){ //LEFT
				osThreadFlagsSet(tid_Th_Joystick, REBOTE);

      }else if(GPIO_Pin==GPIO_PIN_15){ //CENTER
				osThreadFlagsSet(tid_Th_Joystick, REBOTE);

      }
	}

	
/*---------------------------MODULO-----------------------------*/
void initModJoystick(void){
	pulsUP=pulsDOWN=pulsRIGHT=pulsLEFT=pulsCENTER=0x00;
	initJoystick();
	Init_Timer_PulsLarga();
	Init_Timer_Rebotes();
	Init_MsgQueue_Joystick();
	Init_Thread_Joystick();
} 
/*------------------------------TEST----------------------------*/
void Init_Th_Joystick_Test(void) {
	up=right=down=left=center=upL=rightL=downL=leftL=centerL=0x00;
  tid_Th_Joystick_Test = osThreadNew(Th_Joystick_Test, NULL, NULL);

}

void Th_Joystick_Test (void *argument) {
  MSGQUEUE_Joystick_t msg;
  osStatus_t status;

	
  while (1) {
    status = osMessageQueueGet(mid_MsgQueueJoystick, &msg, NULL, osWaitForever);   // wait for message
    if (status == osOK) {
      if(msg.tipo==0x01 && msg.pulsacion==0x01){ //larga up
				upL++;
			}else if(msg.tipo==0x00 && msg.pulsacion==0x01){ //corta up
				up++;
			}else if(msg.tipo==0x01 && msg.pulsacion==0x02){ //larga right
				rightL++;
			}else if(msg.tipo==0x00 && msg.pulsacion==0x02){ //corta right
				right++;
			}else if(msg.tipo==0x01 && msg.pulsacion==0x03){ //larga down
				downL++;
			}else if(msg.tipo==0x00 && msg.pulsacion==0x03){ //corta down
				down++;
			}else if(msg.tipo==0x01 && msg.pulsacion==0x04){ //larga left
				leftL++;
			}else if(msg.tipo==0x00 && msg.pulsacion==0x04){ //corta left
				left++;
			}else if(msg.tipo==0x01 && msg.pulsacion==0x05){ //larga center
				centerL++;
			}else if(msg.tipo==0x00 && msg.pulsacion==0x05){ //corta center
				center++;
			}
    }
  }
}
