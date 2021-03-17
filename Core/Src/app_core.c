/*
 * app_core.c
 *
 *  Created on: 30 янв. 2021 г.
 *      Author: Nikon
 */

#include "app_core.h"

static void choseAndSendError(OW_ERROR_E error);

void AppRun() {
	SendHelloMessage();
	PredictionMod_t *mode = CurrentMode();
	SendCurrentModMessage(mode->name, mode->name_size);

	Sensor_t sens1;
	Sensor_t sens2;

	Sensor_t *sensors[2];
	sensors[0] = &sens1;
	sensors[1] = &sens2;

	OW_ERROR_E error;
	error = OWStart(sensors, 2);
	if (error != OK) {
		choseAndSendError(error);
		return;
	}

	int16_t temperature;
	uint8_t idx = 0;

	for (;;) {
		temperature = 0;
		error = OWMeasureTemperature(sensors[idx], &temperature);
		if (error != OK) {
			choseAndSendError(error);
			return;
		} else {
			SendTemperatureMessage(idx, temperature);
		}

		idx = (idx + 1) % 2;
	}

}

static void choseAndSendError(OW_ERROR_E error) {
	switch(error)
	{
		case TIMEOUT_ERROR:
			SendErrorMessage("timeout", 8);
			break;
		case CHECK_CRC_ERROR:
			SendErrorMessage("check crc", 10);
			break;
		case SHORT_CIRCUIT_ERROR:
			SendErrorMessage("short circuit", 14);
			break;
		case NO_SENSOR_ERROR:
			SendErrorMessage("no sensors", 13);
			break;
		case TOO_MANY_SENSORS_ERROR:
			SendErrorMessage("too many sensors", 17);
			break;
		case UNKONWN_ERROR:
		default:
			SendErrorMessage("unknown", 8);
	}


}
