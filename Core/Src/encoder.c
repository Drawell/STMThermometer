#include "encoder.h"

// начальная инициализация
void ENCODER_Init(ENCODER_Handle_t *henc, uint16_t a_pin, uint16_t b_pin,
		uint16_t push_pin, GPIO_TypeDef *b_port, GPIO_TypeDef *push_port,
		osTimerId mute_timer, encoder_callback_t turn_left_callback,
		encoder_callback_t turn_right_callback,
		encoder_callback_t turn_push_callback) {
	henc->a_pin = a_pin;
	henc->b_pin = b_pin;
	henc->push_pin = push_pin;
	henc->b_port = b_port;
	henc->push_port = push_port;
	henc->mute_timer = mute_timer;
	henc->counter = 0;
	henc->divider = 1;
	henc->mute = 0;
	henc->turn_left_callback = turn_left_callback;
	henc->turn_right_callback = turn_right_callback;
	henc->turn_push_callback = turn_push_callback;
}

// должна вызываться из обработчика прерываний GPIO энкодера
void ENCODER_isr_callback(ENCODER_Handle_t *henc, uint16_t GPIO_pin) {
	if (!henc->mute) {
		if (GPIO_pin == henc->a_pin) {
			if (HAL_GPIO_ReadPin(henc->b_port, henc->b_pin) == GPIO_PIN_RESET) {
				if (--henc->counter <= -henc->divider) {
					henc->counter = 0;
					henc->turn_left_callback();
					//osMessagePut(henc->message_queue, ENCODER_TurnLeftMsg, 0);
				}
			} else {
				if (++henc->counter >= henc->divider) {
					henc->counter = 0;
					henc->turn_right_callback();
					//osMessagePut(henc->message_queue, ENCODER_TurnRightMsg, 0);
				}
			}
			henc->mute = 1;
			osTimerStart(henc->mute_timer, 10 /* ms */);
		}

		if (GPIO_pin == henc->push_pin) {
			henc->counter = 0;

			if (HAL_GPIO_ReadPin(henc->push_port, henc->push_pin)
					== GPIO_PIN_SET) {
				henc->turn_push_callback();
				henc->mute = 1;
				osTimerStart(henc->mute_timer, 10 /* ms */);
				//osMessagePut(henc->message_queue, ENCODER_PushDownMsg, 0);
			}
		}
	}
}

void ENCODER_mute_timer_callback(ENCODER_Handle_t *henc) {
	henc->mute = 0;
}
