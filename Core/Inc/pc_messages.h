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

uint8_t volotile_temperature_message_text[30];
Message_t pcm_volotile_temperature_message =
{
	.text = &volotile_temperature_message_text,
	.size = 30
};

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

Message_t pcm_current_mode_message =
{
	.text = "\n\rCurrent mode: ",
	.size = 17
};

Message_t pcm_error_message =
{
	.text = "\n\rError: ",
	.size = 10
};

Message_t pcm_action_message =
{
	.text = "\n\rAction: ",
	.size = 11
};


Message_t pcm_maintaining_temp_message =
{
	.text = "\n\rMaintaining Temp: ",
	.size = 21
};

#endif /* INC_PC_MESSAGES_H_ */
