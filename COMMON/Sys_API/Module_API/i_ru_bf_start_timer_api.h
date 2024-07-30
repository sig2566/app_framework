/*
 * i_ru_bf_start_timer_api.h
 *
 *  Created on: May 29, 2019
 *      Author: E. Glotman
 */

#ifndef API_MODULE_API_I_RU_BF_START_TIMER_API_H_
#define API_MODULE_API_I_RU_BF_START_TIMER_API_H_

#include "i_main_ru_handler_api.h"
#include "ru_global_defines.h"

#define RU_BF_START_TIMER_MODULE	"RU_BF_START_TIMER"

/******************************************************************************************//*!
 *@class Ant_Group_Pool_API
 *@brief The purpose of this class is :
 *@brief This class expose the Antenna Groups Pool Module control API.
  *********************************************************************************************/
struct Ru_Bf_Start_Timer_API
{
	virtual EResultT ruBfStartTimerProcess(CALL_TIMER_T caller) = 0;

};


#endif /* API_MODULE_API_I_RU_BF_START_TIMER_API_H_ */
