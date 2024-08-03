/*
 * i_ru_uplan_start_timer_api.h
 *
 *  Created on: May 29, 2019
 *      Author: E. Glotman
 */

#ifndef API_MODULE_API_I_RU_UPLAN_START_TIMER_API_H_
#define API_MODULE_API_I_RU_UPLAN_START_TIMER_API_H_

#include "i_main_ru_handler_api.h"
#include "ru_global_defines.h"

#define RU_UPLAN_START_TIMER_MODULE	"RU_UPLAN_START_TIMER"

/******************************************************************************************//*!
 *@class Ru_Uplan_Start_Timer_API
 *@brief The purpose of this class is :
 *@brief This class expose the RU_UPLAN_START_TIMER API.
  *********************************************************************************************/
struct Ru_Uplan_Start_Timer_API
{
	virtual EResultT ruUplanStartTimerProcess(CALL_TIMER_T caller) = 0;

};


#endif /* API_MODULE_API_I_RU_UPLAN_START_TIMER_API_H_ */




