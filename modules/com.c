#include "com.h"
#include <string.h>
#include <stdlib.h>


/*---------------------------VARIABLES---------------------*/
osMessageQueueId_t mid_MsgQueue_COM;      

osThreadId_t tid_MsgQueueCOM;             
osThreadId_t tid_COM_Test;    

extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
int i=0;

char trama[24]="C0040000010084D440000000";
char rTrama[12];
char lineaR[43];
unsigned int numL;
USART_HandleTypeDef husart3;
 
char prueba;
char longitud;
char FEUsart=0xFE;
/*---------------------PROTOTIPO DE FUNCIONES---------------*/
void USART_callback(uint32_t event);
void USART_reset(void);
void Init_MsgQueue_PC(void);
void Init_Thread_COMPC(void);
void Thread_COMPC(void *argument);    
void Th_COMPC_Test(void *argument);
/*----------------------FUNCIONES---------------------------*/
void USART_callback(uint32_t event){
	uint32_t mask;
	mask= ARM_USART_EVENT_RECEIVE_COMPLETE |
				ARM_USART_EVENT_TRANSFER_COMPLETE |
				ARM_USART_EVENT_SEND_COMPLETE |
				ARM_USART_EVENT_TX_COMPLETE|
        ARM_USART_EVENT_RX_BREAK	;
	
	if(event & mask){
		osThreadFlagsSet(tid_MsgQueueCOM, 0x01);
	}

 if(event & ARM_USART_EVENT_RX_TIMEOUT){
		USARTdrv->Control(ARM_USART_ABORT_RECEIVE,115200);
		osThreadFlagsSet(tid_MsgQueueCOM, 0x01);
	}
	 if(event & ( ARM_USART_EVENT_TX_UNDERFLOW)){
		__breakpoint(0);
	}
}

void USART_reset(void){
	USARTdrv->Initialize(USART_callback);
	USARTdrv->PowerControl(ARM_POWER_FULL);
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
	ARM_USART_DATA_BITS_8 |
	ARM_USART_PARITY_NONE |
	ARM_USART_STOP_BITS_1 |
	ARM_USART_FLOW_CONTROL_NONE, 115200);
	//NVIC_EnableIRQ(USART3_IRQn);   
	USARTdrv->Control(ARM_USART_CONTROL_TX,1);
	USARTdrv->Control(ARM_USART_CONTROL_RX,1);
//	USARTdrv->Control(ARM_USART_CONTROL_BREAK,1);
}


void Init_MsgQueue_PC(void){
	mid_MsgQueue_COM = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COMPC_t), NULL);
}

osMessageQueueId_t getModCOMQueueID(void){
	return mid_MsgQueue_COM;
}
void Init_ThCOM(void){
	
	tid_MsgQueueCOM = osThreadNew(Thread_COMPC, NULL, NULL);
 
}
 
void Thread_COMPC(void *argument){
  MSGQUEUE_COMPC_t com;
  USART_reset();
 
	
  while (1) {
		
		//while(rTrama[i]!=FEUsart){
		

	  	USARTdrv->Receive(&rTrama,12);
		  
		osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
			//i++;
		
		//}
		
		sprintf(com.lineaRecibida,"%s",rTrama);
		memset(rTrama,0,sizeof(rTrama));
		osMessageQueuePut(mid_MsgQueue_COM, &com, NULL, osWaitForever);
	 

		

		
	//	sprintf(com.lineaRecibida,"%s",rTrama);
		//osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);

		//osMessageQueuePut(mid_MsgQueue_COM, &com, NULL, osWaitForever);

		osMessageQueueGet(mid_MsgQueue_COM, &com, NULL, osWaitForever);
		
		 if(com.send==1){
			USARTdrv->Send(&com.linea,strlen(com.linea));
	  	osThreadFlagsWait(0x01,osFlagsWaitAny,osWaitForever);
			 memset(com.linea,0,sizeof(com.linea));
			 com.send=1;
			 osMessageQueuePut(mid_MsgQueue_COM, &com, NULL, osWaitForever);
		 }
		
    osThreadYield();                                            
  }
}

/*---------------------------MODULO---------------------------------*/
void initModCOMPC(void){
	
	
	Init_MsgQueue_PC();
	Init_ThCOM();
}

/*----------------------------TEST----------------------------------*/
void Init_Th_COMPC_Test(void){
 
  tid_COM_Test = osThreadNew(Th_COMPC_Test , NULL, NULL);

}

void Th_COMPC_Test(void *argument){
	MSGQUEUE_COMPC_t com;
	int min=0;
	int seg=0;
	int horas=0;
  while (1) {
		osDelay(3000);
		horas=03;
		min=00;
		seg=25;
		sprintf(com.linea,"%02d:%02d:%02d--->%s\n\r",horas,min,seg,trama);
		osMessageQueuePut(mid_MsgQueue_COM, &com, 0U, 0U);

  }
}



