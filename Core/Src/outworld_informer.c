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
char volotile_screen_text[20];
char volotile_error_text[20] = "\0";

int16_t temperature_ = 0;
int16_t maintaining_temperature_ = 0;
PredictionMod_t *prediction_mode_ = NULL;
uint16_t power_ = 0;

void InitOutworldInformer(UART_HandleTypeDef *uart_, uint16_t ms_timeout,
		I2C_HandleTypeDef *hi2c1) {
	uart = uart_;
	timeout = ms_timeout;

#ifdef LCD_1602
	lcd16x2_i2c_init(hi2c1);
#include "lcd1602.h"
#endif

#ifdef SSD1306
	SSD1306_Init();
#endif

}

void StartPCSendMessageTask(void const *argument) {
	osEvent evt;
	Message_t *message;

	for (;;) {
		evt = osMessageGet(PCMessageQueueHandle, osWaitForever); // wait for message

		if (evt.status == osEventMessage) {
			message = (Message_t*) evt.value.p;
			if (message != NULL) {
				HAL_UART_Transmit(uart, message->text, message->size, timeout);
				osDelay(200);
			}
		}
	}

}

void StartDisplayPrintTask(void const *argument) {
	for (;;) {
		if (prediction_mode_ != NULL) {
			//SSD1306_Clear();

#ifdef LCD_1602
			if (volotile_error_text[0]) {

				lcd16x2_i2c_printf("  Hello ");
			} else {
				lcd16x2_i2c_setCursor(0, 0);
				lcd16x2_i2c_printf("  Hello ");
			}
#endif

#ifdef SSD1306
			if (volotile_error_text[0]) {

				SSD1306_GotoXY(0, 0);
				SSD1306_Puts(volotile_error_text, &Font_7x10, 1);
			} else {

				sprintf(volotile_screen_text, "Maintaining: %d.%d   ",
						maintaining_temperature_ / 100, maintaining_temperature_% 100);
				SSD1306_GotoXY(0, 0);
				SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

				sprintf(volotile_screen_text, "Current: %d.%d   ", temperature_ / 100, temperature_ % 100);
				SSD1306_GotoXY(0, 16);
				SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

				sprintf(volotile_screen_text, "Power: %d   ", power_);
				SSD1306_GotoXY(0, 32);
				SSD1306_Puts(volotile_screen_text, &Font_7x10, 1);

				SSD1306_GotoXY(0, 48);
				SSD1306_Puts(prediction_mode_->name, &Font_7x10, 1);
				for (uint8_t i = prediction_mode_->name_size; i < 20; i++)
					SSD1306_Puts(" ", &Font_7x10, 1);
			}

			SSD1306_UpdateScreen();
#endif
		}

		osDelay(200);
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
void PrintPower(uint8_t power) {
	power_ = power;
}

void PrintErrorMessage(char *error_message) {
	strcpy(volotile_error_text, error_message);
}

void SendHelloMessage() {
	osMessagePut(PCMessageQueueHandle, &pcm_hello_message, 0);
}

void SendTemperatureMessage(uint8_t sensor_idx, int16_t temperature) {
	sprintf(&volotile_temperature_message_text, "\n\rTemp: %d: %d\n\0", sensor_idx,
			temperature);
	osMessagePut(PCMessageQueueHandle, &pcm_volotile_temperature_message, 0);
}

void SendCurrentModMessage(char *mod_name, uint8_t name_size) {
	//pcm_volotile_message.text = mod_name;
	//pcm_volotile_message.size = name_size;
	//osMessagePut(PCMessageQueueHandle, &pcm_current_mode_message, 0);
	//osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);

	sprintf(&volotile_current_mode_message_text, "\n\rCurrent mode: %s\n\0", mod_name);
	osMessagePut(PCMessageQueueHandle, &pcm_current_mode_message, 0);

}

void SendErrorMessage(char *text, uint8_t size) {
	//pcm_volotile_message.text = text;
	//pcm_volotile_message.size = size;
	//osMessagePut(PCMessageQueueHandle, &pcm_volotile_message, 0);

	sprintf(&volotile_error_message_text, "\n\rError: %s\n\0", text);
	osMessagePut(PCMessageQueueHandle, &pcm_error_message, 0);

}

void SendPowerMessage(uint8_t power) {
	sprintf(&volotile_power_message_text, "\n\rPower: %d\n\0", power);
	osMessagePut(PCMessageQueueHandle, &pcm_power_message, 0);
}

void SendMaintainingTempMessage(int16_t temperature) {
	sprintf(&volotile_maintaining_temperature_message_text,
			"\n\rMaintaining Temp: %d\n\0", temperature);
	osMessagePut(PCMessageQueueHandle, &pcm_maintaining_temp_message, 0);
}
