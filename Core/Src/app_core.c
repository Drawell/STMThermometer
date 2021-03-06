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

static int16_t temperature;
static PredictionMod_t *mode;

void AppRun(SetControllingPinFunc_t setCtrlPinFunc_) {
	setCtrlPinFunc = setCtrlPinFunc_;

	SendHelloMessage();
	mode = CurrentMode();

	PrintPredictionMode(mode);
	PrintMaintaningTemperature(GetMaintainigTemp());

	SendCurrentModMessage(mode->name, mode->name_size);
	SendMaintainingTempMessage(GetMaintainigTemp());

	Sensor_t sens1;
	Sensor_t *sensors[1];
	sensors[0] = &sens1;

	OW_ERROR_E error;
	error = OWStart(sensors, 1);
	if (error != OK) {
		HandleError(error);
		return;
	}

	uint8_t idx = 0;
	Desison_t desision;
	Desison_t last_desision = TURN_ON;

	for (;;) {
		int16_t new_temperature = 0;
		error = OWMeasureTemperature(sensors[idx], &new_temperature);
		if (error != OK) {
			HandleError(error);
			if (error != CHECK_CRC_ERROR)
				return;
		} else {
			temperature = new_temperature;
			PrintTemperature(temperature);

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

void IncreaseMaintainingTemperature(void)
{
	ChangeMaintainingTemp(1);
	PrintMaintaningTemperature(GetMaintainigTemp());
	SendMaintainingTempMessage(GetMaintainigTemp());
}

void DecreaseMaintainingTemperature(void)
{
	ChangeMaintainingTemp(-1);
	PrintMaintaningTemperature(GetMaintainigTemp());
	SendMaintainingTempMessage(GetMaintainigTemp());
}

void SelectNextMode(void)
{
	SelectNextMode_();
	mode = CurrentMode();
	PrintPredictionMode(mode);
	SendCurrentModMessage(mode->name, mode->name_size);
}

static void HandleError(OW_ERROR_E error) {
	switch (error) {
	case TIMEOUT_ERROR:
		SendErrorMessage("timeout\r\n", 10);
		PrintErrorMessage("timeout error");
		break;
	case CHECK_CRC_ERROR:
		SendErrorMessage("check crc\r\n", 12);
		PrintErrorMessage("check crc error");
		break;
	case SHORT_CIRCUIT_ERROR:
		SendErrorMessage("short circuit\r\n", 16);
		PrintErrorMessage("short circuit");
		break;
	case NO_SENSOR_ERROR:
		SendErrorMessage("no sensors\r\n", 15);
		PrintErrorMessage("no sensors");
		break;
	case TOO_MANY_SENSORS_ERROR:
		SendErrorMessage("too many sensors\r\n", 19);
		PrintErrorMessage("too many sensors");
		break;
	case UNKONWN_ERROR:
	default:
		SendErrorMessage("unknown\r\n", 10);
		PrintErrorMessage("unknown error");
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

