/*
 * app_core.h
 *
 *  Created on: 30 янв. 2021 г.
 *      Author: Nikon
 */

#ifndef APP_CORE_H_
#define APP_CORE_H_

#include "cmsis_os.h"
#include <ow_driver.h>
#include <decision_maker.h>
#include <outworld_informer.h>

typedef void(*SetControllingPinFunc_t)(uint8_t);

void AppRun(SetControllingPinFunc_t setCtrlPinFunc_);



#endif /* APP_CORE_H_ */
