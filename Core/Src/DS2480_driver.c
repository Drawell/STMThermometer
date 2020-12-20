/*
 * DS2480_driver.c
 *
 *  Created on: 20 дек. 2020 г.
 *      Author: Nikon
 */
#include "DS2480_driver.h"

static uint8_t buf[BUFFER_SIZE];
static uint16_t write_ptr = 0;
static uint16_t read_ptr = 0;

//osThreadDef(ReceiveUartThread, osPriorityNormal, 1, 2000);

void ReceiveUartThread(void const *argument);


void DS2480_DriverIni()
{
	//tid_thread1 = osThreadCreate(osThread(ReceiveUartThread), NULL);
}

void DS2480_Response(uint8_t* responce_buffer, uint16_t size, uint16_t timeout)
{
	for (uint16_t i = 0; i < size; i++)
	{
		responce_buffer[i] = buf[read_ptr];
		read_ptr = (read_ptr + 1) % BUFFER_SIZE;
	}

}




void ReceiveUartThread(void const *argument)
{
	/*osEvent evt;
	uint8_t data;

	while(1){
		evt = osMessageGet(MsgBox, osWaitForever);  // wait for message
		if (evt.status == osEventMessage) {
			data = evt.value.p;
			buf[write_ptr] = data;
			write_ptr = (write_ptr + 1) % buf_size;
		}
	}*/

}

