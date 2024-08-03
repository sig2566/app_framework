/*****************************************************************
 * File Name:	i_fronthaul_emulator_api.h
 *
 * Description:	This file describes the FrontHaul Emulator's API
 * 				and related prototypes.
 *
 * Author:		Y. Keshet
 *
 * Copyright (c) 2020 - 2022 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *****************************************************************/
#ifndef I_FRONTHAUL_EMULATOR_API_H__
#define I_FRONTHAUL_EMULATOR_API_H__


#define	FRONTHAUL_EMULATOR_MODULE_NAME	"FRONTHAUL_EMULATOR"
#define TASKS_QUEUE_NAME				"TASKS_FIFO"
#define TIME_EVENTS_FIFO_NAME			"TIME_EVENTS_FIFO"

#define	INIT_SYSTEM_TIME_NF       (0)
#define	INIT_SYSTEM_TIME_NSF      (0)
#define	INIT_SYSTEM_TIME_OFFSET   (0)
// Synchronize the system time to 1/1/2020 00:00:00 GMT
#define	INIT_SYNC_TIME_YEAR       (2020 - 1900)
#define	INIT_SYNC_TIME_MONTH      (1) - (1)      //  months since January     - [0,11]
#define	INIT_SYNC_TIME_DAY        (1)
#define	INIT_SYNC_TIME_HOUR       (0)
#define	INIT_SYNC_TIME_MIN        (0)
#define	INIT_SYNC_TIME_SEC        (0)
#define	INIT_SYNC_TIME_ISDST      (-1)


#define SYMBOL_WINDOW_SIZE_IN_RING	(3)

enum TASKS_TYPE
{
	TIME_EVENT_TASK = 0
};


enum TIME_PARAMETERS
{
	SYMBOL_DL_PARAM	= 12,
	SYMBOL_UL_PARAM	= 13,
	SLOT_PARAM		= 15
};

enum TimeEventType
{
	SymbolTickDLEvent,
	SymbolTickULEvent,
	SlotTickEvent
};

struct TimeEventStruct
{
	SysTimeT       sys_time;          // Lagged time
	TimeEventType  time_event_type;   // enum time event - Slot_Tick ,Symbol_Tick UL/DL
	int32_t        seq_val;           // time event counter value: symbol - [0..13] , Slot - [0..19]
	SysTimeT       dst_time;          // RF Time
};

struct FronthaulEmulatorAPI
{

};


#endif		//	I_FRONTHAUL_EMULATOR_API_H__
