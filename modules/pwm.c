#include "pwm.h"


/*--------------------VARIABLES---------------------*/
osThreadId_t tid_Th_zumbador;                       
TIM_HandleTypeDef tim1;
osTimerId_t tim_id_PWM;  
osThreadId_t tid_PWM_Test;
static uint16_t pulse=5;
osMessageQueueId_t mid_MsgQueuePWM;                // message queue id
osThreadId_t tid_ThPWM; 
TIM_OC_InitTypeDef TIM_Channel_InitStruct;


/*--------------------PROTOTIPOS DE FUNCIONES---------------------*/

void Thread_PWM(void *argument);                   
void initTimer_PWM(void);
void initPWM_out(void);                       
void Re_Init_Timer(void);
void init_Pulsador(void);
osThreadId_t getModPWMQueueID(void);
void Init_PWM_Test(void);
void PWM_Test(void *argument);
/*------------------------FUNCIONES------------------------------*/


void initTimer_PWM(void){
	
	__HAL_RCC_TIM1_CLK_ENABLE();

	tim1.Instance=TIM1;
	tim1.Init.Prescaler=1679;//33599:con APB2 y 168Mhz SystCoreClock
	tim1.Init.CounterMode=TIM_COUNTERMODE_UP;
	tim1.Init.Period=99; //4:1kHz
	tim1.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&tim1);
	TIM_Channel_InitStruct.OCMode = TIM_OCMODE_PWM1;
	TIM_Channel_InitStruct.Pulse =pulse;//pulse;//pulse/4(period)*100
	TIM_Channel_InitStruct.OCPolarity=TIM_OCPOLARITY_HIGH;
	TIM_Channel_InitStruct.OCFastMode= TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(&tim1,&TIM_Channel_InitStruct,TIM_CHANNEL_1);


}
void reInit_Timer(void){
	TIM_Channel_InitStruct.Pulse=pulse;
	HAL_TIM_PWM_ConfigChannel(&tim1,&TIM_Channel_InitStruct,TIM_CHANNEL_1);


}
void initPWM_out(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate=GPIO_AF1_TIM1;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);
	
}



void Init_MsgQueue_PWM(void) {
 
  mid_MsgQueuePWM = osMessageQueueNew(16, sizeof(MSGQUEUE_PWM_t), NULL);

}


osThreadId_t getModPWMQueueID(void){
   return mid_MsgQueuePWM;
}


void Init_ThPWM(void){
	
	tid_ThPWM = osThreadNew(Thread_PWM, NULL, NULL);
 
}
 
void Thread_PWM(void *argument){
  MSGQUEUE_PWM_t PWM;
	//PWM.start=1;
	//osMessageQueuePut(mid_MsgQueuePWM, &PWM, NULL, 0U);   // wait for message

	osDelay(1000);

  while (1) {
     // Insert thread code here...
    osMessageQueueGet(mid_MsgQueuePWM, &PWM, NULL, 0U);   // wait for message
    pulse=PWM.pulse;
		reInit_Timer();
		//initTimer_PWM();
    
	//osMessageQueueGet(mid_MsgQueuePWM, &PWM, NULL, 0U);   // wait for message
		if(PWM.start==1){
				HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
		}else{
				HAL_TIM_PWM_Stop(&tim1, TIM_CHANNEL_1);
		}
		osThreadYield();
  }
}

/*--------------------MÓDULO---------------------*/
void initModPWM(void){
	initTimer_PWM();
	initPWM_out();
	Init_MsgQueue_PWM();
	Init_ThPWM();
	
}

/*--------------------TEST---------------------*/


void Init_PWM_Test(void){
 
  tid_PWM_Test = osThreadNew(PWM_Test, NULL, NULL);

}
 
void PWM_Test(void *argument){
	MSGQUEUE_PWM_t pwm;

  while (1) {		
				osDelay(5000);

		    pwm.pulse=4;//80% ciclo de trabajo
		    pwm.start=1;
				osMessageQueuePut(mid_MsgQueuePWM, &pwm, 0U, 0U);
		    osDelay(10000);
	    	pwm.pulse=2;//40%
	    	pwm.start=1;
				osMessageQueuePut(mid_MsgQueuePWM, &pwm, 0U, 0U);
		    osDelay(10000);
		    pwm.pulse=5;//100%
		    pwm.start=1;
				osMessageQueuePut(mid_MsgQueuePWM, &pwm, 0U, 0U);
		    osDelay(10000);
     		pwm.start=0;//0%
        osMessageQueuePut(mid_MsgQueuePWM, &pwm, 0U, 0U);
		    osDelay(5000);
		    pwm.pulse=3;//60%
		    pwm.start=1;
				osMessageQueuePut(mid_MsgQueuePWM, &pwm, 0U, 0U);

		

  }
}


