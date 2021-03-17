/*
 * decision_maker.c
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */
#include "decision_maker.h"

#define INC_PTR(ptr) (ptr = (ptr + 1) % TEMP_BUFFER_SIZE)
#define ADD_PTR(ptr, len) (ptr = (ptr + len) % TEMP_BUFFER_SIZE)
#define LEN(ptr1, ptr2) (ptr1) < (ptr2) ? ((ptr2) - (ptr1)) : ((ptr2) + TEMP_BUFFER_SIZE - (ptr1))

static int16_t buf[TEMP_BUFFER_SIZE];
static int16_t write_ptr = 0;
static int16_t read_ptr = 0;

static int16_t maintaining_temperature = 2000;
static Desison_t current_desison = TURN_OFF;

static Desison_t NoneAdaptivePredictFunc(void);

PredictionMod_t predict_modes[] =
{
	{
		.name = "None Adaptive",
		.name_size = 13,
		.funct_ptr = &NoneAdaptivePredictFunc
	}
};

static int16_t current_mode_ptr = 0;

void StartDecisionMakerTask(void const *argument) {
	for (;;) {

	}
}

void DecisionMakerInit(int16_t maintaining_temperature_)
{
	maintaining_temperature = maintaining_temperature_;
}

void AddTemperature(int16_t temperature)
{
	buf[INC_PTR(write_ptr)] = temperature;
}

Desison_t AskDecision(void)
{
	if (buf[write_ptr] >= maintaining_temperature)
		return TURN_OFF;

	return (*predict_modes[current_mode_ptr].funct_ptr)();
}

PredictionMod_t* CurrentMode(void)
{
	return &predict_modes[current_mode_ptr];
}

static Desison_t NoneAdaptivePredictFunc(void)
{
	if (current_desison == TURN_ON) // heating
	{
		if (buf[write_ptr] >= maintaining_temperature)
			return TURN_OFF;
		else
			return TURN_ON;
	}
	else // chilling
	{
		if (buf[write_ptr] <= maintaining_temperature - 200)
			return TURN_ON;
		else
			return TURN_OFF;
	}
}




