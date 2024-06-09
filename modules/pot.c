#include "pot.h"
#include "math.h"

/*--------------------------VARIABLES----------------------------*/
#define RESOLUTION_12B 4096U
#define VREF 3.3f

osThreadId_t tid_Th_POT;
osMessageQueueId_t mid_MsgQueueP;         
osThreadId_t tid_POT_Test;
float pot1Act,pot2Act;
float pot1,pot2;
float p1Test,p2Test;

/*-------------------PROTOTIPOS DE FUNCIONES---------------------*/
void ADC1_pins_F429ZI_config(void);
void ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
void Init_MsgQueue_POT(void) ;
void Thread_POT(void *argument) ;
void Init_Thread_POT(void);
void Th_POT_Test(void *argument) ;
/*---------------------------FUNCIONES---------------------------*/
void ADC1_pins_F429ZI_config(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/*PC0     ------> ADC1_IN10//pot2
		PA3     ------> ADC1_IN3//pot1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		
}
void ADC_Init_Single_Conversion(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance){
	
   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc->Instance = ADC_Instance;
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc->Init.Resolution = ADC_RESOLUTION_12B;
  hadc->Init.ScanConvMode = DISABLE;
  hadc->Init.ContinuousConvMode = DISABLE;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc->Init.NbrOfConversion = 1;
  hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;

}

float ADC_getVoltage(ADC_HandleTypeDef *hadc, uint32_t Channel)	{
		ADC_ChannelConfTypeDef sConfig = {0};
		HAL_StatusTypeDef status;

		uint32_t raw = 0;
		float voltage = 0;
		 /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = Channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
  {
    return -1;
  }
		
		HAL_ADC_Start(hadc);
		
		do 
			status = HAL_ADC_PollForConversion(hadc, 0); //This funtions uses the HAL_GetTick(), then it only can be executed wehn the OS is running
		while(status != HAL_OK);
		
		raw = HAL_ADC_GetValue(hadc);
		
		voltage = raw*VREF/RESOLUTION_12B; 
		
		return voltage;

}

//COLA
void Init_MsgQueue_P(void) {
 
  mid_MsgQueueP = osMessageQueueNew(SIZE_MSGQUEUE_P, sizeof(MSGQUEUE_POT_t), NULL);

}

osMessageQueueId_t getModPOTQueueID(void){
	return mid_MsgQueueP;
}	
void Thread_POT (void *argument) {
	MSGQUEUE_POT_t pott;
	float value1,value2;
	ADC_HandleTypeDef adchandle; //handler definition
	ADC1_pins_F429ZI_config(); //specific PINS configuration
	ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration

	
	value1=ADC_getVoltage(&adchandle , 3 );
	pot1=value1*65000/3.2;
	
	value2=ADC_getVoltage(&adchandle , 10 ); //get values from channel 10->ADC123_IN10
	pot2=value2*95/3.2;
	
	if(pot1<1000){
		pott.pot1=1000;
	}else if(pot1>65000){
		pott.pot1=65000;
	}else{
		pott.pot1=(int)round(pot1);
	}
	
	if(pot2<5){
		pott.pot2=5;
	}else if(pot2>95){
		pott.pot2=95;
	}else{
		pott.pot2=(int)round(pot2);
	}
	
	osMessageQueuePut(mid_MsgQueueP, &pott, NULL, osWaitForever);
  while (1) {
		
		value1=ADC_getVoltage(&adchandle , 3 );
	  pot1=value1*65000/3.2;

		
		value2=ADC_getVoltage(&adchandle , 10 ); //get values from channel 10->ADC123_IN10
		pot2=value2*95/3.3;
		
		if(pot1<1000){
			pott.pot1=1000;
		}else if(pot1>65000){
			pott.pot1=65000;
		}else{
			pott.pot1=(int)round(pot1);
		}
		
		if(pot2<5){
			pott.pot2=5;
		}else if(pot2>95){
			pott.pot2=95;
		}else{
			pott.pot2=(int)round(pot2);
		}
		osMessageQueuePut(mid_MsgQueueP, &pott, NULL, osWaitForever);

	//osDelay(100);
	}
}

void Init_Thread_P (void) {
 
  tid_Th_POT = osThreadNew(Thread_POT, NULL, NULL);

}


/*---------------------------MODULO----------------------------------*/
void initModPOT(void){

	Init_Thread_P();
	Init_MsgQueue_P();
	
} 
	
	
	/*----------------------------TEST----------------------------------*/
void Th_POT_Test (void *argument) {
	MSGQUEUE_POT_t pot;
	osStatus_t status;
	
  while (1) {
   status = osMessageQueueGet(mid_MsgQueueP, &pot, NULL, osWaitForever);   // wait for message
    if (status == osOK) {
			p1Test=pot.pot1;
			p2Test=pot.pot2;
    }
  }
}

void Init_POT_Test(void) {
 
  tid_POT_Test = osThreadNew(Th_POT_Test, NULL, NULL);

}
