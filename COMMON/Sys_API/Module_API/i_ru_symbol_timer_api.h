/*
 * i_ru_symbol_timer_api.h
 *
 *  Created on: May 29, 2019
 *      Author: E. Glotman
 */

#ifndef API_MODULE_API_I_RU_SYMBOL_TIMER_API_H_
#define API_MODULE_API_I_RU_SYMBOL_TIMER_API_H_

#include "i_main_ru_handler_api.h"
#include "ru_global_defines.h"

#define RU_SYMBOL_TIMER_MODULE	"SYMBOL_TIMER"

/******************************************************************************************//*!
 *@class Ru_symbol_Timer_API
 *@brief The purpose of this class is :
 *@brief This class expose the RU_SYMBOL_TIMER API.
  *********************************************************************************************/
struct Ru_Symbol_Timer_API
{
	virtual EResultT ruSymbolTimerProcess(CALL_TIMER_T caller) = 0;
};


#endif /* API_MODULE_API_I_RU_SYMBOL_TIMER_API_H_ */
