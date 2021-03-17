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

typedef enum DECISION_E {
	 TURN_OFF, TURN_ON,
} Desison_t;

typedef Desison_t (*deside_fuc_t)(void);

typedef struct PredictionMod_t {
	char* name;
	uint8_t name_size;
	deside_fuc_t funct_ptr;
} PredictionMod_t;

void StartDecisionMakerTask(void const *argument);

void DecisionMakerInit(int16_t maintaining_temperature_);

void AddTemperature(int16_t temperature);

Desison_t AskDecision(void);

PredictionMod_t* CurrentMode(void);



#endif /* INC_DECISION_MAKER_H_ */