#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef void (*encoder_callback_t)(void);

typedef struct ENCODER_Handle_t {
	uint16_t a_pin;
	uint16_t b_pin;
	uint16_t push_pin;
	GPIO_TypeDef *b_port;
	GPIO_TypeDef *push_port;
	osTimerId mute_timer;
	int8_t counter;
	uint8_t divider;
	uint8_t mute;
	encoder_callback_t turn_left_callback;
	encoder_callback_t turn_right_callback;
	encoder_callback_t turn_push_callback;
} ENCODER_Handle_t;

void ENCODER_Init(ENCODER_Handle_t *henc, uint16_t a_pin, uint16_t b_pin,
		uint16_t push_pin, GPIO_TypeDef *b_port, GPIO_TypeDef *push_port,
		osTimerId mute_timer, encoder_callback_t turn_left_callback,
		encoder_callback_t turn_right_callback,
		encoder_callback_t turn_push_callback);

void ENCODER_isr_callback(ENCODER_Handle_t *henc, uint16_t GPIO_pin);

void ENCODER_mute_timer_callback(ENCODER_Handle_t *henc);

#endif /* ENCODER_H_ */
