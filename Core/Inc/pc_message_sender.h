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

osMessageQId PCMessageQueueHandle;

void InitPSPCMessageSender(UART_HandleTypeDef* uart_, uint16_t ms_timeout);

void StartPCSendMessageTask(void const * argument);

void SendHelloMessage();

void SendTemperatureMessage(uint8_t sensor_idx, int16_t temperature);

void SendCurrentModMessage(char* mod_name, uint8_t name_size);

void SendErrorMessage(char* text, uint8_t size);


#endif /* INC_PC_MESSAGE_SENDER_H_ */
