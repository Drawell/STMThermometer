/*
 * PCMessageSender.h
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */

#ifndef INC_PC_MESSAGE_SENDER_H_
#define INC_PC_MESSAGE_SENDER_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "decision_maker.h"
#include "fonts.h"
#include "ssd1306.h"

osMessageQId PCMessageQueueHandle;

void InitOutworldInformer(UART_HandleTypeDef* uart_, uint16_t ms_timeout);

void StartPCSendMessageTask(void const * argument);

void StartDisplayPrintTask(void const * argument);

void PrintTemperature(int16_t temperature);

void PrintMaintaningTemperature(int16_t maintaining_temperature);

void PrintPredictionMode(PredictionMod_t* prediction_mode);

void PrintPower(uint8_t power);

void PrintErrorMessage(char* error_message);

void SendHelloMessage();

void SendTemperatureMessage(uint8_t sensor_idx, int16_t temperature);

void SendCurrentModMessage(char* mod_name, uint8_t name_size);

void SendErrorMessage(char* text, uint8_t size);

void SendPowerMessage(uint8_t power);

void SendMaintainingTempMessage(int16_t temperature);

#endif /* INC_PC_MESSAGE_SENDER_H_ */
