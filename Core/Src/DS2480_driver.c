/*
 * DS2480_driver.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Nikon
 */
#include "DS2480_driver.h"

#define INC_PTR(ptr) (ptr = (ptr + 1) % BUFFER_SIZE)
static uint8_t buf[BUFFER_SIZE];
static int16_t write_ptr = -1;
static int16_t read_ptr = -1;

static uint8_t buf2[BUFFER_SIZE];
static int16_t write_ptr2 = -1;

static UART_HandleTypeDef* uart;

static uint8_t detect_frame[] = {0x17, 0x45, 0x5B, 0x0F, 0x91};
static uint8_t detect_responce_frame[] = {0x16, 0x44, 0x5A, 0x00, 0x93};

static DS2480_ERROR_E RunDetect();

void DS2480_DriverIni(UART_HandleTypeDef* _uart)
{
	osMessageQDef(message_q_DS2480, 16, uint8_t);
	message_q_DS2480_id = osMessageCreate(osMessageQ(message_q_DS2480), NULL);
	uart = _uart;

}

DS2480_ERROR_E DS2480_Run(void)
{
	DS2480_ERROR_E error = RunDetect();
	if (error != OK)
		return error;

	return OK;
}

void WriteBuffer(uint8_t data)
{
	buf2[INC_PTR(write_ptr2)] = data;
}

void DS2480_Response(uint8_t* responce_buffer, uint16_t size, uint16_t timeout)
{
	for (uint16_t i = 0; i < size; i++)
	{
		responce_buffer[i] = buf[INC_PTR(read_ptr)];
	}

}

void ReceiveUartTask(void const *argument)
{
	osEvent evt;
	uint8_t data;

	while(1){
		evt = osMessageGet(message_q_DS2480_id, osWaitForever);  // wait for message
		if (evt.status == osEventMessage) {
			data = evt.value.p;
			buf[INC_PTR(write_ptr)] = data;
		}
	}
}

static DS2480_ERROR_E RunDetect()
{
	uint8_t code = 0xC1;
	HAL_UART_Transmit(uart, &code, 1, HAL_MAX_DELAY);
	osDelay(4);
	HAL_UART_Transmit(uart, detect_frame, 5, HAL_MAX_DELAY);
	osDelay(4);
	HAL_UART_Abort(uart);

	uint8_t not_ok = 0;
	for (uint16_t i = 0; i < 5; i++)
	{
		not_ok |= (buf2[INC_PTR(read_ptr)] != detect_responce_frame[i]);
	}

	if (not_ok)
		return DETECT_ERROR;


}







