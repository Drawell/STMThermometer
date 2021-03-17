/*
 * PCMessageSender.c
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */

#include "pc_message_sender.h"
#include "pc_messages.h"

static UART_HandleTypeDef *uart;
static uint16_t timeout;

void InitPSPCMessageSender(UART_HandleTypeDef* uart_, uint16_t ms_timeout)
{
	uart = uart_;
	timeout = ms_timeout;
}

void StartPCSendMessageTask(void const *argument) {
	osEvent evt;
	Message_t *message;

	for (;;) {
		evt = osMessageGet(PCMessageQueueHandle, osWaitForever); // wait for message
		if (evt.status == osEventMessage) {
			message = (Message_t*) evt.value.p;
			if (message != NULL)
				HAL_UART_Transmit(uart, message->text, message->size, timeout);
		}
	}

}

void SendHelloMessage() {
	osMessagePut(PCMessageQueueHandle, &pcm_hello_message, 0);
}

void SendTemperatureMessage(uint8_t sensor_idx, int16_t temperature) {
	sprintf(&volotile_temperature_message_text, "\n\r%d: %d", sensor_idx, temperature);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_temperature_message, 0);
}

void SendCurrentModMessage(char* mod_name, uint8_t name_size) {
	pcm_volotile_message.text = mod_name;
	pcm_volotile_message.size = name_size;
	osMessagePut(PCMessageQueueHandle, &pcm_current_mode_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);

}

void SendErrorMessage(char* text, uint8_t size) {
	pcm_volotile_message.text = text;
	pcm_volotile_message.size = size;
	osMessagePut(PCMessageQueueHandle, &pcm_error_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);
}

