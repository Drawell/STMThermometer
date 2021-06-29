/*
 * PCMessageSender.c
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */

#include "outworld_informer.h"
#include "pc_messages.h"

static UART_HandleTypeDef *uart;
static uint16_t timeout;
static uint8_t is_printing = 0;
char volotile_screen_text[40];

void InitOutworldInformer(UART_HandleTypeDef *uart_, uint16_t ms_timeout) {
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

void UpdateScreenInformation(int16_t temperature,
		int16_t maintaining_temperature, PredictionMod_t *prediction_mode) {

	if (prediction_mode != NULL) {
		//SSD1306_Clear();

		sprintf(volotile_screen_text, "Maintaining: %d",
				maintaining_temperature);
		SSD1306_GotoXY(0, 0);
		SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

		sprintf(volotile_screen_text, "Current: %d", temperature);
		SSD1306_GotoXY(0, 16);
		SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

		SSD1306_GotoXY(0, 32);
		SSD1306_Puts(prediction_mode->name, &Font_7x10, 1);

		SSD1306_UpdateScreen();
	}
}

void PrintError(char *error_message) {
	SSD1306_GotoXY(0, 48);
	SSD1306_Puts(error_message, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

void SendHelloMessage() {
	osMessagePut(PCMessageQueueHandle, &pcm_hello_message, 0);
}

void SendTemperatureMessage(uint8_t sensor_idx, int16_t temperature) {
	sprintf(&volotile_temperature_message_text, "\n\rTemp: %d: %d", sensor_idx,
			temperature);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_temperature_message, 0);
}

void SendCurrentModMessage(char *mod_name, uint8_t name_size) {
	pcm_volotile_message.text = mod_name;
	pcm_volotile_message.size = name_size;
	osMessagePut(PCMessageQueueHandle, &pcm_current_mode_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);

}

void SendErrorMessage(char *text, uint8_t size) {
	pcm_volotile_message.text = text;
	pcm_volotile_message.size = size;
	osMessagePut(PCMessageQueueHandle, &pcm_error_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);
}

void SendActionMessage(char *text, uint8_t size) {
	pcm_volotile_message.text = text;
	pcm_volotile_message.size = size;
	osMessagePut(PCMessageQueueHandle, &pcm_action_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);
}

void SendMaintainingTempMessage(int16_t temperature) {
	sprintf(&volotile_temperature_message_text, "%d", temperature);
	osMessagePut(PCMessageQueueHandle, &pcm_maintaining_temp_message, 0);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_temperature_message, 0);
}
