#include "BH1750luz.h"
#include "math.h"

/*--------------------------VARIABLES----------------------------*/

//I2C register addresses
#define BH_CMD    0x10
#define ON_CMD    0x01
#define T_COMPLETE 0x01
#define BH_ADDR 0x23


#define SENSOR_MAX 65535     // Maximum lux value the sensor can output
#define DESIRED_MIN 1000     // Minimum value of the target range
#define DESIRED_MAX 65000    // Maximum value of the target range

extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C* I2Cdrv=&Driver_I2C1;


osTimerId_t tim_temp; 
osMessageQueueId_t mid_MsgQueueLZ;

osThreadId_t tid_Luz;                        
osThreadId_t tid_LZ_Test;
float temperatura;

/*-------------------PROTOTIPOS DE FUNCIONES---------------------*/
void I2C_SignalEvent_LUZ (uint32_t event);
void initI2C(void);
float sensorLuz(void);
int read16(uint8_t reg);
void Init_MsgQueue_Luz (void);
void Thread_Luz (void *argument);
void Init_Thread_SensorLuz(void);
void Th_Luz_Test (void *argument);
/*---------------------------FUNCIONES---------------------------*/
//SENSOR LUZ
/* I2C Signal Event function callback */
void I2C_SignalEvent_LUZ (uint32_t event) {
 
  if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
    osThreadFlagsSet(tid_Luz, T_COMPLETE);
  }

}
void I2C_Initialize(void){
	I2Cdrv->Initialize(I2C_SignalEvent_LUZ);
	I2Cdrv->PowerControl(ARM_POWER_FULL);
	I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 0);
}


//COLA
void Init_MsgQueue_Luz (void) {
 
  mid_MsgQueueLZ = osMessageQueueNew(SIZE_MSGQUEUE_LZ , sizeof(MSGQUEUE_BH_t), NULL);

}
osMessageQueueId_t getModLZQueueID(void){
	return mid_MsgQueueLZ;
}

float medidaLuz(){
    //Signed return value
    float value;
	  float rango;
 
    //Read the 11-bit raw temperature value
    value = read16(BH_CMD);
 
    //Sign extend negative numbers
//    if (value & (1 << 10))
//        value |= 0xFC00;
	  rango=(float)(DESIRED_MAX)/(SENSOR_MAX + DESIRED_MIN);
    value=((value)+DESIRED_MIN)*(rango);
	
		if(value<1000){
				value=DESIRED_MIN;
		}
    //Return the temperature in °C
    return round(value);
}

void Init_BH(uint8_t cmd,uint8_t reg){
	  I2Cdrv->MasterTransmit(BH_ADDR, &cmd, 1, false);
	  osThreadFlagsWait(T_COMPLETE, osFlagsWaitAny, osWaitForever);
		osDelay(200);
		
  //Select the register
  I2Cdrv->MasterTransmit(BH_ADDR, &reg, 1, false);
  /* Wait until transfer completed */
	osThreadFlagsWait(T_COMPLETE, osFlagsWaitAny, osWaitForever);

}
 
int read16(uint8_t reg){
//Create a temporary buffer
  uint8_t lux[2];

	//Read the 16-bit register
	I2Cdrv->MasterReceive(BH_ADDR, lux, 2, false);
  /* Wait until recee completed */
	osThreadFlagsWait(T_COMPLETE, osFlagsWaitAny, osWaitForever);
	
	//Return the combined 16-bit value
  return (lux[0] << 8) | lux[1];
}


//THREAD TEMPERATURA
void Thread_Luz(void *argument) {
			Init_BH(ON_CMD,BH_CMD);
			MSGQUEUE_BH_t msg;
  while (1) {
		msg.lx = medidaLuz();                                        
		osMessageQueuePut(mid_MsgQueueLZ, &msg, 0U, 0U);
		//osDelay(500);
    osThreadYield();                            // suspend thread
  }
}

void Init_Thread_SensorLuz(void) {
  tid_Luz = osThreadNew(Thread_Luz, NULL, NULL);
}
 

/*---------------------------MODULO-----------------------------*/
void initModBH(void){
	I2C_Initialize();
	Init_MsgQueue_Luz();
	Init_Thread_SensorLuz();
	
} 