/*
 * decision_maker.h
 *
 *  Created on: Mar 16, 2021
 *      Author: Nikon
 */

#ifndef INC_DECISION_MAKER_H_
#define INC_DECISION_MAKER_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define TEMP_BUFFER_SIZE 255

typedef uint8_t (*deside_fuc_t)(void);

typedef struct PredictionMod_t {
	char* name;
	uint8_t name_size;
	deside_fuc_t funct_ptr;
} PredictionMod_t;

void StartDecisionMakerTask(void const *argument);

void DecisionMakerInit(int16_t maintaining_temperature_);

int16_t GetMaintainigTemp();

void ChangeMaintainingTemp(int8_t value);

void AddTemperature(int16_t temperature);

uint8_t GetPower(void);

PredictionMod_t* CurrentMode(void);

void SelectNextMode_(void);


#endif /* INC_DECISION_MAKER_H_ */
