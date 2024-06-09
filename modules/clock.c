#include "clock.h"

/*--------------------VARIABLES---------------------*/
extern uint8_t horas;
extern uint8_t minutos;
extern uint8_t segundos;
osThreadId_t tid_Th_clock;
osTimerId_t tim_id_Clock;                         
osThreadId_t tid_Th_Clock_Test;

/*--------------------PROTOTIPOS DE FUNCIONES---------------------*/
void Init_Thread_Clock (void);
void Thread_Clock (void *argument);    
void Th_Clock_Test(void *argument); 
/*--------------------FUNCIONES---------------------*/
void Init_Thread_Clock (void) {
  tid_Th_clock = osThreadNew(Thread_Clock, NULL, NULL);

}
 
void Thread_Clock (void *argument) {

  while (1) {
		osDelay(1000);
		if(segundos<59){
			segundos++;
		}else{
			segundos=0;
			if(minutos<59){
				minutos++;
			}else{
				minutos=0;
				if(horas<23){
					horas++;
				}else{
					horas=0;
					
				}
			}			
	}
    osThreadYield();                           
  }
}

/*--------------------MÓDULO---------------------*/
void initModClock(void){
	Init_Thread_Clock();
}	

/*--------------------TEST---------------------*/
void Init_Th_Clock_Test(void) {
 
  tid_Th_Clock_Test = osThreadNew(Th_Clock_Test, NULL, NULL);

}
 
void Th_Clock_Test (void *argument) {
	
  while (1) {
		
		osDelay(20000); 
		
		horas=23;
		minutos=59;
		segundos=50;
		
  }
}
