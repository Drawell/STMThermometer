#ifndef DS2480_DRIVER_H
#define DS2480_DRIVER_H
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"


#define BUFFER_SIZE 255

//osMessageQId message_q_DS2480_id;
osMessageQId UartMessageQueueHandle;

typedef enum OW_ERROR_E {
	OK = 0,
	UNKONWN_ERROR,
	TIMEOUT_ERROR,
	CHECK_CRC_ERROR,
	SHORT_CIRCUI_ERROR,
	NO_SENSOR_ERROR,
	TOO_MANY_SENSORS_ERROR,
	SET_BAUD_ERROR,
	DETECT_ERROR,


} OW_ERROR_E;

typedef struct Sensor_t
{
	uint8_t addr[8];
} Sensor_t;

void OWDriverInit(UART_HandleTypeDef* uart_, uint16_t ms_timeout);

void ReceiveUartTask(void const *argument);

OW_ERROR_E OWStart(Sensor_t* sensors[], uint8_t size);
OW_ERROR_E OWMeasureTemperature(Sensor_t* sensor, int16_t* temperature);




#endif //DS2480_DRIVER_H
