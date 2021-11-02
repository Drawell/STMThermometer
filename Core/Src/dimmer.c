/*
 * dimmer.c
 *
 *  Created on: Jul 18, 2021
 *      Author: Nikon
 */

#include "dimmer.h"

#define WAIT_FOR_ZERO_CTRL_INT 0
#define WAIT_FOR_TIMER 1
#define SIGNAL_END 3
#define IGNORE_TICKS_AFTER_ZERO_INT 3
#define MAX_POWER_TICKS 100

void DIMMER_Init(DIMMER_Handle_t *hdim, uint16_t zc_pin, uint16_t psm_pin,
		GPIO_TypeDef *zc_port, GPIO_TypeDef *psm_port, TIM_HandleTypeDef *timer) {
	hdim->zc_pin = zc_pin;
	hdim->psm_pin = psm_pin;
	hdim->zc_port = zc_port;
	hdim->psm_port = psm_port;
	hdim->timer = timer;
	hdim->state = 0;
	hdim->counter = 0;
	hdim->psm_cout = 0;
	DIMMER_SetPower(hdim, 50);
}

void DIMMER_isr_zero_ctrl_callback(DIMMER_Handle_t *hdim, uint16_t GPIO_pin) {
	if (GPIO_pin == hdim->zc_pin) {
		if (hdim->state == WAIT_FOR_ZERO_CTRL_INT) {
			hdim->state = WAIT_FOR_TIMER;
			hdim->psm_cout = 0;
			hdim->counter = 0;
		}
	}
}

void DIMMER_isr_timer_callback(DIMMER_Handle_t *hdim) {
	if (hdim->state != WAIT_FOR_ZERO_CTRL_INT) {
		if (hdim->state >= SIGNAL_END) {
			HAL_GPIO_WritePin(hdim->psm_port, hdim->psm_pin, GPIO_PIN_RESET);
			hdim->state = WAIT_FOR_ZERO_CTRL_INT;
		}
		else if (hdim->counter > IGNORE_TICKS_AFTER_ZERO_INT + MAX_POWER_TICKS - hdim->power) {
			HAL_GPIO_WritePin(hdim->psm_port, hdim->psm_pin, GPIO_PIN_SET);
			hdim->state++;
		}

		hdim->counter++;
	}
}

void DIMMER_SetPower(DIMMER_Handle_t *hdim, uint8_t power) {
	hdim->power = power > 100 ? 100 : power;
}
