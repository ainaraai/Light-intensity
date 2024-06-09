#include "principal.h"
#include <string.h>
#include <stdlib.h>

/*--------------------------VARIABLES----------------------------*/



//float tempMedida,tempMedidat,tempP;
//float tempReferienciat,tempReferiencia=25;

uint8_t porcentaje,porcentajeA;//,porcentajet;


osThreadId_t tid_Th_PRINC;
osTimerId_t tim_id;

/*--------------------------VARIABLES INTERNAS----------------------------*/
uint8_t horas,horasP,h;
uint8_t minutos,minP,m;
uint8_t segundos,segP,s;
uint8_t segundosPrevios;
static float diff;
static uint16_t Lref,Lmed,LrefP;
static uint8_t intensidad;
static float inten;
//COM


char lon;
char lineaReciTT[12];
uint8_t decimal;
char okay;
char FE=0xFE;
char lineaU;
uint8_t num;
char bufftram[350];
static uint8_t cont;

#define REPOSO 	0x01
#define MANUAL 	0x09
#define AUTO 0x03
#define PROGRAMACION 	0x10

#define HORAS 	0x0B
#define MINUTOS 	0x06
#define SEGUNDOS 0x07
#define REF 	0x08

#define CIEN 	0x05
#define OCHENTA 	0x04
#define CUARENTA 0x02
#define CERO 	0x00

#define NOVENTA 	90
#define DIF 	10
#define LMAX 	66000

uint8_t estado;
uint8_t seleccionado;
uint16_t cicloTrabajo;
	//TIM_OC_InitTypeDef TIM_Channel_InitStruct;

//BUFFER 
char bufferCirc[20][43];
uint8_t buffE=0;//posicion escritura buffer
uint8_t buffL=0;//posicion lectura buffer
char tramaLect[35];
uint8_t autoEsc=0;
uint8_t stop=0;
char tramaASCII[43];

/*-------------------PROTOTIPOS DE FUNCIONES---------------------*/
void bufferRellena(void);
void bufferPopSiguiente(void);
void bufferLee(void);
void bufferVacia(void);
void configLCD(void);
void Thread_PRINC(void *argument);
void Init_Th_Principal(void);
/*---------------------------FUNCIONES---------------------------*/

void bufferVacia(void){
	memset(bufferCirc, 0, sizeof(bufferCirc));
	buffE=0;
	buffL=0;
}

void bufferRellena(void){
	if(buffE<21){
		strcpy(bufferCirc[buffE],tramaASCII);
		buffE++;
	}else{
		buffE=0;
		strcpy(bufferCirc[buffE],tramaASCII);
		buffE++;

	}
}


void bufferLee(void){
	if(buffL<21 && bufferCirc[buffL]!=0){
		strcpy(tramaLect,bufferCirc[buffL]);
		buffL++;
		stop=0;
	}else if(bufferCirc[buffL]==0){
		stop=1;
	}else{
		stop=0;
		buffL=0;
		strcpy(tramaLect,bufferCirc[buffL]);
		buffL++;

	}
}

// LCD
void configLCD(void){
	MSGQUEUE_LCD_t localObject;

	switch(estado){
		case REPOSO:
			sprintf(localObject.linea, "     SBM 2024  ");
			localObject.nLin=1;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);

			sprintf(localObject.linea, "      %02i:%02i:%02i", horas, minutos,segundos);
			localObject.nLin=2;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);

		break;
		case MANUAL:
			sprintf(localObject.linea, "   M -PWM %d%%-", intensidad);
			localObject.nLin=1;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
			
			sprintf(localObject.linea, "    %02i:%02i:%02i", horas, minutos,segundos);
			localObject.nLin=2;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
		break;
		
		case AUTO:
			sprintf(localObject.linea, "   A -PWM %d%%-", intensidad);
			localObject.nLin=1;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
			
			sprintf(localObject.linea, "    %02i:%02i:%02i", horas, minutos,segundos);
			localObject.nLin=2;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
			sprintf(tramaASCII,"%02d:%02d:%02d--Lmed:%d.%03d--Lref:%d.%03d--PWM:%d%%",horas,minutos,segundos,Lmed/1000,Lmed%1000,Lref/1000,Lref%1000,intensidad);
			bufferRellena();
		printf("%s",tramaASCII);
		break;
//		case ACTIVO:
//			diff=tempReferiencia-tempMedida;
//			sprintf(localObject.linea, "   ACT---%02i:%02i:%02i---", horas, minutos, segundos);
//			localObject.nLin=1;
//			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
//		  

//		if(porcentaje==100){
//			sprintf(localObject.linea,"Tm:%0.1f`-Tr:%0.1f`-d:%i", tempMedida, tempReferiencia,porcentaje);
//			}else{
//			sprintf(localObject.linea,"Tm:%0.1f`-Tr:%0.1f`-d:%i%%", tempMedida, tempReferiencia,porcentaje);
//			}
//			localObject.nLin=2;
//			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
//		  sprintf(tramaASCII,"%d:%d:%d-Tm:%0.1f`-Tr:%0.1f`-d%d%%",horas,minutos,segundos,tempMedida,tempReferiencia,porcentaje);
//		  if(porcentajeA!=porcentaje){
//				porcentajeA=porcentaje;
//				bufferRellena();
//			}

//		break;
//		case TEST:
//			diff=tempReferienciat-tempMedidat;
//			sprintf(localObject.linea, "   TEST---%02i:%02i:%02i---", horas, minutos, segundos);
//			localObject.nLin=1;
//			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);

//	  	sprintf(localObject.linea, "Tm:%0.1f`-Tr:%0.1f`-D:%i%%", tempMedidat, tempReferienciat,porcentaje);
//			localObject.nLin=2;
//			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);

//		break;
		case PROGRAMACION:
			sprintf(localObject.linea, "  PROGRAMACION");
		//	sprintf(localObject.linea, "      ---P&D---");

			localObject.nLin=1;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
		
		  sprintf(localObject.linea, "H: %02i:%02i:%02i - Lr%d", horasP, minP, segP, LrefP);
			localObject.nLin=2;
			osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);

		break;
	}
}
void Init_Th_Principal(void){
 
  tid_Th_PRINC = osThreadNew(Thread_PRINC, NULL, NULL);

}

void Thread_PRINC(void *argument){
	osStatus_t status;
	MSGQUEUE_PWM_t msgPWM;
	MSGQUEUE_BH_t msgLuz;	
	MSGQUEUE_COMPC_t msgComPC;
	MSGQUEUE_POT_t msgPot;
  MSGQUEUE_RGB_t msgRGB;
	MSGQUEUE_Joystick_t msgJoystick;
  while (1) {
		if(segundosPrevios!=segundos){
		segundosPrevios=segundos;
		configLCD();
		}
		
		
		//Gestion Joystick
		status = osMessageQueueGet(getModJoystickQueueID(), &msgJoystick, NULL, 100);   // wait for message nueva pulsacion
		if(status==osOK){
			if(msgJoystick.tipo==0x01 && msgJoystick.pulsacion==0x05){ //pulsacion centro larga cambio de estado
				switch(estado){
						case REPOSO:
							estado=MANUAL;
						break;
						case MANUAL:
							bufferVacia();
							estado=AUTO;
						break;
						case AUTO:
							//tempMedida=tempMedidat;
						  //tempP=tempReferiencia=tempReferienciat;
						  LrefP=Lref;
							horasP=horas;
							minP=minutos;
							segP=segundos;
						
							estado=PROGRAMACION;	
						break;
						case PROGRAMACION:
							estado=REPOSO;
						  cont=0;
						break;
					}
			}else if(msgJoystick.tipo==0x00 && msgJoystick.pulsacion==0x01){//arriba corta
				
				
				if(estado==PROGRAMACION){
					switch(seleccionado){
								case HORAS:
									if(horasP<23){
										horasP++;
									}else{
										horasP=0;
									}
								break;
								case MINUTOS:
									if(minP<59){
										minP++;
									}else{
										minP=0;
									}
								break;
								case SEGUNDOS:
									if(segP<59){
										segP++;
									}else{
										segP=0;
									}
								break;
								case REF:
									if(LrefP<65000){
										LrefP=LrefP+1000;
									}else{
										LrefP=1000;
									}
								break;
						}
					}
				}else if(msgJoystick.tipo==0x00 && msgJoystick.pulsacion==0x02){//corta dch
					if(estado==PROGRAMACION){
						switch(seleccionado){
								case HORAS:
									seleccionado=MINUTOS;
								break;
								case MINUTOS:
									seleccionado=SEGUNDOS;
								break;
								case SEGUNDOS:
									seleccionado=REF;
								break;
								case REF:
									seleccionado=HORAS;
								break;
							}
					
					}

				}else if(msgJoystick.tipo==0x00 && msgJoystick.pulsacion==0x03){//corta abajo
					if(estado==PROGRAMACION){
						switch(seleccionado){
								case HORAS:
									if(horasP==0){
										horasP=23;
									}else{
										horasP--;
									}
								break;
								case MINUTOS:
									if(minP==0){
										minP=59;
									}else{
										minP--;
									}
								break;
								case SEGUNDOS:
									if(segP==0){
										segP=59;
									}else{
										segP--;
									}
								break;
								case REF:
									if(LrefP<=1000){
										LrefP=65000;
									}else{
										LrefP=LrefP-1000;
									}
								break;
								}
					
					
					}

					
				}else if(msgJoystick.tipo==0x00 && msgJoystick.pulsacion==0x04){//corta izq
					if(estado==PROGRAMACION){
							switch(seleccionado){
								case HORAS:
									seleccionado=REF;
								break;
								case MINUTOS:
									seleccionado=HORAS;
								break;
								case SEGUNDOS:
									seleccionado=MINUTOS;
								break;
								case REF:
									seleccionado=SEGUNDOS;
								break;
							}
						}
					
					
				}else if(msgJoystick.tipo==0x00 && msgJoystick.pulsacion==0x05){//centro corta
					if(estado==PROGRAMACION){
					    horas=horasP;
							minutos=minP;
							segundos=segP;
						  Lref=LrefP;
					}

				}
		}


		
		//Luz BH1750
		status = osMessageQueueGet(getModLZQueueID(), &msgLuz, NULL, 100);   // wait for message nuevo volumen
		if(status==osOK){//mensaje sacado de la cola, no timeout
			Lmed=msgLuz.lx;
		}
	
		
		//Mensajes a envr RGB/PWM
		if(estado==REPOSO){
			  msgPWM.start=0;
			  osMessageQueuePut(getModPWMQueueID(), &msgPWM, 0U, 0U);

				msgRGB.red=0;
				msgRGB.blue=0;
				msgRGB.green=1;
			if(cont==0){
				msgRGB.pulse=NOVENTA;
				cont++;
			}else if(cont<9){
				msgRGB.pulse=msgRGB.pulse-DIF;

				cont++;		
			}else if(cont>8 && cont<17){
				cont++;
				msgRGB.pulse=msgRGB.pulse+DIF;

				if(cont==17){
					cont=0;
				}
			}
			 osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);

		}
		if(estado==MANUAL){
				
				//Potenciometros
				status = osMessageQueueGet(getModPOTQueueID(), &msgPot, NULL, 100);   // wait for message nuevo volumen
				if(status==osOK){//mensaje sacado de la cola, no timeout
					Lref=msgPot.pot1;
					intensidad=msgPot.pot2;
				}
				
				msgRGB.red=0;
				msgRGB.blue=0;
				msgRGB.green=0;
			  osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);
				if(Lmed>Lref){
					 diff=Lmed-Lref;
				}else{
					diff=Lref-Lmed;
				}
				if((diff)>5000) {
					msgRGB.red=1;
					msgRGB.pulse=NOVENTA;
					osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);
				}else{
					msgRGB.red=0;
					osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);				
				}
				msgPWM.start=1;
			  msgPWM.pulse=intensidad;
			  osMessageQueuePut(getModPWMQueueID(), &msgPWM, 0U, 0U);

		  
		}
		if(estado==AUTO){
				msgRGB.red=0;
				msgRGB.blue=0;
				msgRGB.green=0;
			  osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);
				if(Lmed>Lref){
					 diff=Lmed-Lref;
				}else{
					diff=Lref-Lmed;
				}
				if((diff)>5000) {
					msgRGB.blue=1;
					msgRGB.pulse=NOVENTA;
					osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);
				}else{
					msgRGB.blue=0;
					osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);				
				}
				inten=((float)Lref/LMAX)*100;
				intensidad=(int)inten;
				msgPWM.pulse=intensidad;
			  osMessageQueuePut(getModPWMQueueID(), &msgPWM, 0U, 0U);
			
		}
			
		//Linea de comandos PC
		if(estado==PROGRAMACION){
			msgRGB.red=0;
			msgRGB.blue=0;
			msgRGB.green=0;
			osMessageQueuePut(getModRGBQueueID(), &msgRGB, 0U, 0U);
			
		 osMessageQueueGet(getModCOMQueueID(), &msgComPC, NULL, 100);   // wait for message nueva pulsacion
	
			sprintf(lineaReciTT,"%s",msgComPC.lineaRecibida);
		  lon=strlen(lineaReciTT);
			
			//if(lineaReciTT[0]!=0x01||lineaReciTT[2]!=lon || lineaReciTT[lon-1]!=(char)0xFE){
				//okay=0;
			//}
		if(lineaReciTT[1]==(char)0x20){
				h=atoi(&lineaReciTT[3])+atoi(&lineaReciTT[4]);
				m=atoi(&lineaReciTT[6])+atoi(&lineaReciTT[7]);
				s=atoi(&lineaReciTT[9])&atoi(&lineaReciTT[10]);
				
				
				if(h<24&&m<60&&s<60&&h>=0&&s>=0&&m>=0){
					horasP=h;
					minP=m;
					segP=s;
					lineaReciTT[1]=0xDF;
					lineaU=lineaReciTT[2];
//					for(int i=0;i<strlen(lineaReciTT);i++){
//						lineaU[i]=(char)lineaReciTT[i];
//					}
					  sprintf(msgComPC.linea,"0x01 0xDF %s 0xFE",&lineaReciTT[lon-1]);
					  msgComPC.send=1;
						osMessageQueuePut(getModCOMQueueID(), &msgComPC, 0U, 0U);
				}else{
					okay=0x00;				
				}		
			}else if(lineaReciTT[1]==(char)0x25){
				
				
				num=atoi(&lineaReciTT[3])+atoi(&lineaReciTT[4]);
				
				if(num>4&&num<=30){
					//tempReferiencia=num+0.1*atoi(&lineaReciTT[6]);
					
					sprintf(msgComPC.linea,"0x01 0xDA %s 0xFE",&lineaReciTT[lon-1]);
					msgComPC.send=1;
					osMessageQueuePut(getModCOMQueueID(), &msgComPC, 0U, 0U);
				}else{
					okay=0x00;
				}
			}else if(lineaReciTT[1]==(char)0x55){
				while(!stop){
				 //osDelay(1000);
				 lineaReciTT[1]=0xAF;
				 
				 strcat(bufftram,tramaLect);
				 //strcat(&lineaReciTT[strlen(lineaReciTT)-1],&FE);
				 sprintf(msgComPC.linea,"0x01 0xAF %s 0xFE",bufftram);
				 msgComPC.send=1;
				 
				 osMessageQueuePut(getModCOMQueueID(), &msgComPC, 0U, 0U);
				}
	
			}else if(lineaReciTT[1]==(char)0x60){
				bufferVacia();
				//lineaReciTT[1]=0x9F;
				
				sprintf(msgComPC.linea,"0x01 0x9F 0x04 0xFE");
				msgComPC.send=1;
				osMessageQueuePut(getModCOMQueueID(), &msgComPC, 0U, 0U);
			}else{
			  okay=0x00;
			    
		   }
	   }

		
	
	}
	
}


	/*---------------------------MODULO----------------------------------*/
void initModPRINCIPAL(void){
	estado=REPOSO;
	seleccionado=HORAS;
	horas=minutos=segundos=horasP=minP=segP=0;
	Lref=23000;
	segundosPrevios=60;
	porcentajeA=1;//porcentaje anterior el cual inicializamos a un valor que no vaya a dar porcentaje para uqe entre simepre
  cont=0;
	bufferVacia();
	initModRGB();
	initModJoystick();
	initModClock();
	initModLCD();
	initModBH();
	initModPOT();
	initModPWM();
	initModCOMPC();
	Init_Th_Principal();
	
} 






















