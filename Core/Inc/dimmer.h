/*
 * dimmer.h
 *
 *  Created on: Jul 18, 2021
 *      Author: Nikon
 */

#ifndef INC_DIMMER_H_
#define INC_DIMMER_H_
#include "stm32f1xx_hal.h"

typedef struct DIMMER_Handle_t {
	uint16_t zc_pin;
	uint16_t psm_pin;
	GPIO_TypeDef *zc_port;
	GPIO_TypeDef *psm_port;
	uint8_t power;
	uint8_t state;
	uint8_t counter;
	uint8_t psm_cout;
	TIM_HandleTypeDef *timer;
} DIMMER_Handle_t;

void DIMMER_Init(DIMMER_Handle_t *hdim, uint16_t zc_pin, uint16_t psm_pin,
		GPIO_TypeDef *zc_port, GPIO_TypeDef *psm_port, TIM_HandleTypeDef *timer);

void DIMMER_isr_zero_ctrl_callback(DIMMER_Handle_t *hdim, uint16_t GPIO_pin);

void DIMMER_isr_timer_callback(DIMMER_Handle_t *hdim);

void DIMMER_SetPower(DIMMER_Handle_t *hdim, uint8_t power);

#endif /* INC_DIMMER_H_ */
