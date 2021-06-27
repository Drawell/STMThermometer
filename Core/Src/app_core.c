/*
 * app_core.c
 *
 *  Created on: 30 янв. 2021 г.
 *      Author: Nikon
 */

#include "app_core.h"

static void HandleError(OW_ERROR_E error);
static void TurnOnHeater();
static void TurnOffHeater();
static SetControllingPinFunc_t setCtrlPinFunc;


void AppRun(SetControllingPinFunc_t setCtrlPinFunc_) {
	setCtrlPinFunc = setCtrlPinFunc_;

	SendHelloMessage();
	PredictionMod_t *mode = CurrentMode();
	int16_t maintainint_temp = GetMaintainigTemp();

	UpdateScreenInformation(0, maintainint_temp, mode);

	SendCurrentModMessage(mode->name, mode->name_size);
	SendMaintainingTempMessage(maintainint_temp);

	Sensor_t sens1;
	//Sensor_t sens2;

	Sensor_t *sensors[1];
	sensors[0] = &sens1;
	//sensors[1] = &sens2;

	OW_ERROR_E error;
	error = OWStart(sensors, 1);
	if (error != OK) {
		HandleError(error);
		return;
	}

	int16_t temperature;
	uint8_t idx = 0;
	Desison_t desision;
	Desison_t last_desision = TURN_ON;

	for (;;) {
		temperature = 0;
		error = OWMeasureTemperature(sensors[idx], &temperature);
		if (error != OK) {
			HandleError(error);
			return;
		} else {
			UpdateScreenInformation(temperature, maintainint_temp, mode);

			SendTemperatureMessage(idx, temperature);
			AddTemperature(temperature);
			desision = AskDecision();

			if (desision != last_desision) {
				last_desision = desision;
				if (desision == TURN_OFF)
					TurnOffHeater();
				else
					TurnOnHeater();
			}
		}

		idx = (idx + 1) % 1;
	}

}

static void HandleError(OW_ERROR_E error) {
	switch (error) {
	case TIMEOUT_ERROR:
		SendErrorMessage("timeout\r\n", 10);
		break;
	case CHECK_CRC_ERROR:
		SendErrorMessage("check crc\r\n", 12);
		break;
	case SHORT_CIRCUIT_ERROR:
		SendErrorMessage("short circuit\r\n", 16);
		break;
	case NO_SENSOR_ERROR:
		SendErrorMessage("no sensors\r\n", 15);
		break;
	case TOO_MANY_SENSORS_ERROR:
		SendErrorMessage("too many sensors\r\n", 19);
		break;
	case UNKONWN_ERROR:
	default:
		SendErrorMessage("unknown\r\n", 10);
	}

}

static void TurnOnHeater() {
	SendActionMessage("Turn On", 8);
	(*setCtrlPinFunc)(1);
}

static void TurnOffHeater() {
	SendActionMessage("Turn Off", 9);
	(*setCtrlPinFunc)(0);
}

