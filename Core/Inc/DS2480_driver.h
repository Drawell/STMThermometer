#ifndef DS2480_DRIVER_H
#define DS2480_DRIVER_H
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define BUFFER_SIZE 255

osMessageQId message_q_DS2480_id;

typedef enum DS2480_ERROR_E {
	OK,
	DETECT_ERROR



} DS2480_ERROR_E;

void DS2480_DriverIni(UART_HandleTypeDef* uart_);
DS2480_ERROR_E DS2480_Run(void);
void WriteBuffer(uint8_t data);

void ReceiveUartTask(void const *argument);
void DS2480_Response(uint8_t* responce_buffer, uint16_t size, uint16_t timeout);



#endif //DS2480_DRIVER_H
