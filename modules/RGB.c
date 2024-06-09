
#include "RGB.h"

/*--------------------------VARIABLES----------------------------*/

TIM_HandleTypeDef tim4;
TIM_OC_InitTypeDef TIM_Channel_InitStruct_1;

osMessageQueueId_t mid_MsgQueueRGB;                // message queue id
osThreadId_t tid_ThRGB; 
osThreadId_t tid_RGB_Test;
uint8_t green,blue,red;

/*--------------------PROTOTIPOS DE FUNCIONES---------------------*/

void Thread_RGB(void *argument);                   
void initTimer_PWM_RGB(void);
void Init_RGB(void);
osThreadId_t getModRGBQueueID(void);
void Init_RGB_Test(void);
void RGB_Test(void *argument);
void Init_MsgQueue_RGB(void);
void Init_ThRGB(void);

/*--------------------------FUNCIONES----------------------------*/


void initTimer_PWM_RGB(void){
	__HAL_RCC_TIM4_CLK_ENABLE();

	tim4.Instance=TIM4;
	tim4.Init.Prescaler=5249;
	tim4.Init.CounterMode=TIM_COUNTERMODE_UP;
	tim4.Init.Period=99; //8 (SYS/4(APB1 Preescaler) /999+1/Period+1
	tim4.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&tim4);
	
//the value of pulse should be less than period 
	//PULSE/PERIOD*100
	TIM_Channel_InitStruct_1.OCMode = TIM_OCMODE_PWM2; 
	TIM_Channel_InitStruct_1.Pulse =90;//47250;
	TIM_Channel_InitStruct_1.OCPolarity=TIM_OCPOLARITY_HIGH;
	TIM_Channel_InitStruct_1.OCFastMode= TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(&tim4,&TIM_Channel_InitStruct_1,TIM_CHANNEL_2);
				//HAL_TIM_PWM_Start(&tim4,TIM_CHANNEL_2);

	HAL_TIM_PWM_ConfigChannel(&tim4,&TIM_Channel_InitStruct_1,TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&tim4,&TIM_Channel_InitStruct_1,TIM_CHANNEL_4);


}





void initRGB(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_13; //ROJO RGB
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Alternate=GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_12; //VERDE
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Alternate=GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_15; //AZUL
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Alternate=GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	
}


void reInit_TimRGB(double pulse, uint32_t Channel){
   // __HAL_TIM_SET_COMPARE(&tim4, Channel, pulse);
	TIM_Channel_InitStruct_1.Pulse =pulse;
	HAL_TIM_PWM_ConfigChannel(&tim4,&TIM_Channel_InitStruct_1,Channel);
	HAL_TIM_PWM_Start(&tim4,Channel);


}


void Init_MsgQueue_RGB(void){
 
  mid_MsgQueueRGB = osMessageQueueNew(16, sizeof(MSGQUEUE_RGB_t), NULL);

}


osThreadId_t getModRGBQueueID(void){
   return mid_MsgQueueRGB;
}


void Init_ThRGB(void){
	
	tid_ThRGB = osThreadNew(Thread_RGB, NULL, NULL);
 
}
 
void Thread_RGB(void *argument){
  MSGQUEUE_RGB_t RGB;
	osDelay(1000);
	static double pulse;


  while (1) {
     // Insert thread code here...
    osMessageQueueGet(mid_MsgQueueRGB, &RGB, NULL, 0U);   // wait for message
    red=RGB.red;
		blue=RGB.blue;
		green=RGB.green;
		pulse=RGB.pulse;

		
		if(green){
			reInit_TimRGB(pulse,TIM_CHANNEL_1);
			//HAL_TIM_PWM_Start(&tim4,TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&tim4,TIM_CHANNEL_1);
		}
		//osMessageQueueGet(mid_MsgQueueRGB, &RGB, NULL, 0U); 

		if(red){
			reInit_TimRGB(pulse,TIM_CHANNEL_2);
			//HAL_TIM_PWM_Start(&tim4,TIM_CHANNEL_2);
		}else{
			HAL_TIM_PWM_Stop(&tim4,TIM_CHANNEL_2);
		}
		//osMessageQueueGet(mid_MsgQueueRGB, &RGB, NULL, 0U); 
		if(blue){
			reInit_TimRGB(pulse,TIM_CHANNEL_4);
			//HAL_TIM_PWM_Start(&tim4,TIM_CHANNEL_4);
		}else{
			HAL_TIM_PWM_Stop(&tim4,TIM_CHANNEL_4);

		}

		osThreadYield();
  }
}

/*--------------------MÓDULO---------------------*/
void initModRGB(void){
	initTimer_PWM_RGB();
	initRGB();
	Init_MsgQueue_RGB();
	Init_ThRGB();
	//Init_RGB_Test();
}

/*--------------------TEST---------------------*/
void Init_RGB_Test(void) {
 
  tid_RGB_Test = osThreadNew(RGB_Test, NULL, NULL);

}
 
void RGB_Test(void *argument) {
	  MSGQUEUE_RGB_t RGB;

  while (1) {
		
		osDelay(2000); 
		RGB.blue=1;
		RGB.green=0;
		RGB.red=0;
	  osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000); 
		RGB.blue=0;
		RGB.green=1;
		RGB.red=0;
	  osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000);
		RGB.blue=0;
		RGB.green=1;
		RGB.red=1;
		osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000);
		RGB.blue=0;
		RGB.green=0;
		RGB.red=1;
		osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000);
		RGB.blue=0;
		RGB.green=0;
		RGB.red=0;
		osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000);
		RGB.blue=1;
		RGB.green=1;
		RGB.red=1;
		osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		osDelay(2000);
		RGB.blue=1;
		RGB.green=1;
		RGB.red=0;
		osMessageQueuePut(mid_MsgQueueRGB, &RGB, 0U, 0U);
		
  }
}
