/*
 * app_core.c
 *
 *  Created on: 30 янв. 2021 г.
 *      Author: Nikon
 */

#include "app_core.h"


void AppRun()
{
	Sensor_t sens1;
	Sensor_t sens2;

	Sensor_t* sensors[2];
	sensors[0] = &sens1;
	sensors[1] = &sens2;

	OWStart(sensors, 2);

	int16_t temperature2;
	OWMeasureTemperature(&sens2, &temperature2);
	int16_t temperature;
	OWMeasureTemperature(&sens1, &temperature);


	int16_t temperature3 = 1;

}
