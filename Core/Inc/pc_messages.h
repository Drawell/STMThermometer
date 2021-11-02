/*
 * pc_messages.h
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */

#ifndef INC_PC_MESSAGES_H_
#define INC_PC_MESSAGES_H_

typedef __UINT8_TYPE__ uint8_t;

typedef struct Message_t
{
	uint8_t* text;
	uint8_t size;
} Message_t;


Message_t pcm_hello_message =
{
	.text = "\n\rHello, PC!",
	.size = 12
};

Message_t pcm_volotile_message =
{
	.text = NULL,
	.size = 0
};

uint8_t volotile_temperature_message_text[30];
Message_t pcm_volotile_temperature_message =
{
	.text = &volotile_temperature_message_text,
	.size = 30
};


uint8_t volotile_current_mode_message_text[40];
Message_t pcm_current_mode_message =
{
	.text = volotile_current_mode_message_text,
	.size = 40
};

uint8_t volotile_error_message_text[30];
Message_t pcm_error_message =
{
	.text = volotile_error_message_text,
	.size = 30
};

uint8_t volotile_maintaining_temperature_message_text[30];
Message_t pcm_maintaining_temp_message =
{
	.text = volotile_maintaining_temperature_message_text,
	.size = 30
};

uint8_t volotile_power_message_text[30];
Message_t pcm_power_message =
{
	.text = volotile_power_message_text,
	.size = 30
};


#endif /* INC_PC_MESSAGES_H_ */
