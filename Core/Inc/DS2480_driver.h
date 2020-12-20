#ifndef DS2480_DRIVER_H
#define DS2480_DRIVER_H
#include "stm32f1xx_hal.h"

#define BUFFER_SIZE 255
//#include "cmsis_os.h"


//osMessageQDef(DS2480_MQ, 1, uint8_t); // Define message queue
//osMessageQId  DS2480_MQ;

void DS2480_DriverIni();

void DS2480_Response(uint8_t* responce_buffer, uint16_t size, uint16_t timeout);



#endif
