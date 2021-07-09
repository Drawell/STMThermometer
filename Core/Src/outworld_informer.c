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
char volotile_error_text[40];

int16_t temperature_ = 0;
int16_t maintaining_temperature_ = 0;
PredictionMod_t *prediction_mode_ = NULL;

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

void StartDisplayPrintTask(void const *argument) {
	for (;;) {
		if (prediction_mode_ != NULL) {
			//SSD1306_Clear();

			sprintf(volotile_screen_text, "Maintaining: %d",
					maintaining_temperature_);
			SSD1306_GotoXY(0, 0);
			SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

			sprintf(volotile_screen_text, "Current: %d", temperature_);
			SSD1306_GotoXY(0, 16);
			SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

			SSD1306_GotoXY(0, 32);
			SSD1306_Puts(prediction_mode_->name, &Font_7x10, 1);

			SSD1306_GotoXY(0, 48);
			SSD1306_Puts(volotile_error_text, &Font_7x10, 1);

			SSD1306_UpdateScreen();
		}

		osDelay(100);
	}
}
void PrintTemperature(int16_t temperature) {
	temperature_ = temperature;
}

void PrintMaintaningTemperature(int16_t maintaining_temperature) {
	maintaining_temperature_ = maintaining_temperature;
}

void PrintPredictionMode(PredictionMod_t *prediction_mode) {
	prediction_mode_ = prediction_mode;
}

void PrintErrorMessage(char *error_message) {
	strcpy(volotile_error_text, error_message);
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
