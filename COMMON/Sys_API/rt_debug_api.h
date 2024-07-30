/******************************************************************
 * rt_debug_api.h.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef TOOLS_TRACER_RT_DEBUG_API_H_
#define TOOLS_TRACER_RT_DEBUG_API_H_
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <errno.h>

#include "i_sys_types.h"
#include "common_typedef.h"
#include "i_sys_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
#define TRACE_STRING_SIZE (128)
#define DEFAULT_TRACE_ID (0)
typedef enum
{
	RTDBG_INIT,
	RTDBG_RUNNING_CONSUMER,
	RTDBG_RUNNING_PRODUCER,
	RTDBG_STOPPED
} RTDBG_STATUS_T;
typedef uint32_t HANDLER;
struct RT_counter
{
	volatile __align(CACHE_ALIGNMENT) int64_t val;
	char							  cnt_name[TRACE_STRING_SIZE];
};

//Counter API -- Support event counters
//Returns the counter structure
RT_counter* RTDBG_GetCounter(HANDLER debug_grp, uint32_t num );

//Allocate new counter in the group debug_grp;
volatile int64_t* RTDBG_AllocCounter(HANDLER debug_grp, char* cnt_name);

//Returns number of counters in the current group
uint32_t    RTDBG_GetCountersNum(HANDLER debug_grp);

//TRACE API -- Support RT trace
//Save trace data for per group debug_grp
void 		RTDBG_AddTrace(HANDLER debug_grp, uint32_t trace_id, uint32_t line_num, GenSysTime *sys_time, uint64_t var0 , uint64_t var1, uint64_t var2, uint64_t var3);

//Extract the earliest trace entry
bool        RTDBG_GetTraceEntry(char *trace_entry_str, timespec *linux_time);

//Add new trace entry with presentation printf like format 'format' To the group trace formats data.
uint32_t    RTDBG_AddTraceEntry(HANDLER grp, char* format);

//Log Support
//Add log to the debug_grp group cyclic buffer
void RTDBG_AddLog(HANDLER debug_grp, GenSysTime *sys_time, char *log_str);

//Extract the earliest log
bool RTDBG_GetLog(char *log_strm, timespec *linux_time);

//Profiler support
//Allocate new profiling entry with name prof_name
uint32_t  	RTDBG_AddProfiler(HANDLER debug_grp, char *prof_name);

//Save the latest profiler measurement.
void		RTDBG_SaveProfMeas(HANDLER debug_grp, uint32_t prof_id, ProfileData *data);
//Extract number allocated profilers per group.
uint32_t	RTDBG_GetProfCntrs(HANDLER debug_grp);
//Extract commulative profiler data, using all saved measurements
bool		RTDBG_GetProfInfo(HANDLER debug_grp, uint32_t prof_id, ProfileData *prof_data, char *grp_name, char* prof_name);

//General RT Debug API
void        RTDBG_Init();

//Allocates debugging group.
// This function returns index of the debug group. If the group with the sane name is allocated then the index of existed group is removed.
HANDLER     RTDBG_AllocRTDebugGroup(char* group_name);

//Return the name of the group debug_grp
bool		RTDBG_GetGrpName(HANDLER debug_grp, char  *grp_name);

// Return number of debugging groups
uint32_t	RTDBG_GetGrpNum();

//Stop collection of traces, logs
void        RTDBG_Stop();

//Start collection of traces, logs
void        RTDBG_Start();

//Set the name of the shared memory file. This file will be created in the /tmp directory,
void		RTDBG_SetSharedMemFile(char* mem_mapped_file);

//Creating shared memory file to save the RT debugging info.
bool  	RTDBG_ActivateDebugDataCollection();

//Attach to existed file to extract the collected data
bool  	RTDBG_AttachDebugData();

//Unmap and close the shared memory file.
void	RTDBG_Close();

//Extract collected trace data (traces, logs, event counters, profiler measurements
bool	RTDBG_ExtractDebugData(char *out_file);

//Get Version
char*       RTDBG_GetVersion();

//Get Status
RTDBG_STATUS_T RTDBG_GetStatus();
#ifdef __cplusplus
}
#endif


#endif /* TOOLS_TRACER_RT_DEBUG_API_H_ */
