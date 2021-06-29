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
static Desison_t current_decison = TURN_OFF;

static Desison_t NoneAdaptivePredictFunc(void);

PredictionMod_t predict_modes[] = { { .name = "None Adaptive", .name_size = 13,
		.funct_ptr = &NoneAdaptivePredictFunc }, { .name = "Second Mode   ",
		.name_size = 15, .funct_ptr = &NoneAdaptivePredictFunc } };

static int16_t current_mode_ptr = 0;

void StartDecisionMakerTask(void const *argument) {
	for (;;) {

	}
}

void DecisionMakerInit(int16_t maintaining_temperature_) {
	maintaining_temperature = maintaining_temperature_;
}

int16_t GetMaintainigTemp() {
	return maintaining_temperature;
}

void ChangeMaintainingTemp(int8_t value) {
	maintaining_temperature += 100 * value;
	if (maintaining_temperature < 2000) {
		maintaining_temperature = 2000;
	} else if (maintaining_temperature > 10000) {
		maintaining_temperature = 10000;
	}
}

void AddTemperature(int16_t temperature) {
	buf[INC_PTR(write_ptr)] = temperature;
}

Desison_t AskDecision(void) {
	if (buf[write_ptr] >= maintaining_temperature) {
		current_decison = TURN_OFF;
		return TURN_OFF;
	}

	return (*predict_modes[current_mode_ptr].funct_ptr)();
}

void SelectNextMode_(void) {
	current_mode_ptr = (current_mode_ptr + 1) % (sizeof(predict_modes) / sizeof(PredictionMod_t));
}

PredictionMod_t* CurrentMode(void) {
	return &predict_modes[current_mode_ptr];
}

static Desison_t NoneAdaptivePredictFunc(void) {

	int16_t temp = buf[write_ptr];

	if (current_decison == TURN_ON) // heating
			{
		if (temp >= maintaining_temperature) {
			current_decison = TURN_OFF;
			return TURN_OFF;
		} else {
			current_decison = TURN_ON;
			return TURN_ON;
		}
	} else // chilling
	{
		if (temp <= maintaining_temperature - 200) {
			current_decison = TURN_ON;
			return TURN_ON;
		} else {
			current_decison = TURN_OFF;
			return TURN_OFF;
		}
	}
}

