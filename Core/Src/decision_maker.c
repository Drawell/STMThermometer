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

#define MINIMAL_MAINTAINING_TEMPERATURE 2000
#define MAXIMAL_MAINTAINING_TEMPERATURE 10000

static int16_t buf[TEMP_BUFFER_SIZE];
static int16_t write_ptr = 0;
static int16_t read_ptr = 0;

static int16_t maintaining_temperature = 2000;
static uint8_t power = 50;
static uint8_t is_calculated = 0;

static uint8_t NaivePredictFunc(void);
static uint8_t PIDPredictFunc(void);

PredictionMod_t predict_modes[] = {
		{ .name = "PID", .name_size = 4, .funct_ptr = &PIDPredictFunc },
		{ .name = "Naive", .name_size = 6, .funct_ptr = &NaivePredictFunc }
		};

static int16_t current_mode_ptr = 0;

void StartDecisionMakerTask(void const *argument) {
	for (;;) {
		if (!is_calculated) {
			power = (*predict_modes[current_mode_ptr].funct_ptr)();
			is_calculated = 1;
		}
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
	if (maintaining_temperature < MINIMAL_MAINTAINING_TEMPERATURE) {
		maintaining_temperature = MINIMAL_MAINTAINING_TEMPERATURE;
	} else if (maintaining_temperature > MAXIMAL_MAINTAINING_TEMPERATURE) {
		maintaining_temperature = MAXIMAL_MAINTAINING_TEMPERATURE;
	}
}

void AddTemperature(int16_t temperature) {
	buf[INC_PTR(write_ptr)] = temperature;
	is_calculated = 0;
}

uint8_t GetPower(void) {
	if (buf[write_ptr] >= maintaining_temperature) {
		power = 1;
	}

	return power;
}

void SetPower(uint8_t power_) {
	/*
	if (power_ > 100 || power_ == 0 || power_ == 255)
		return;
	else
		power = power_;*/

}

void SelectNextMode_(void) {
	current_mode_ptr = (current_mode_ptr + 1)
			% (sizeof(predict_modes) / sizeof(PredictionMod_t));
}

PredictionMod_t* CurrentMode(void) {
	return &predict_modes[current_mode_ptr];
}

static uint8_t NaivePredictFunc(void) {

	int16_t temp = buf[write_ptr];

	if (power >= 80) // heating
			{
		if (temp >= maintaining_temperature) {
			return 1;
		} else {
			return 100;
		}
	} else // chilling
	{
		if (temp <= maintaining_temperature - 200) {
			return 100;
		} else {
			return 1;
		}
	}
}

static float kp = 8;
static float ki = 0.015;
static float kd = 20;

static float prev_error = 0;
static float prev_prev_error = 0;
static float integral = 0;
static float dt = 1.1;

static uint8_t PIDPredictFunc(void) {
	int16_t temp = buf[write_ptr];

	float error = maintaining_temperature / 100. - temp / 100.;
	integral += error * dt;

	float error_delta = 0;
	error_delta += (error - prev_error) / dt;
	error_delta += (prev_error - prev_prev_error) / dt;

	prev_prev_error = prev_error;
	prev_error = error;
	float power_ = kp * error +  ki * integral + kd * error_delta;

	if (power_ >= 100.)
		return 100;
	else if (power_ <= 1)
		return 1;
	else
		return (uint8_t) power_;


}

