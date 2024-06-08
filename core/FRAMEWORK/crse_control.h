/*
 * crse_control.h
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */


#ifndef CGLCONTROLAPI_H_
#define CGLCONTROLAPI_H_

#include "i_api_common.h"
class CRSE_Control;
#include "cmodule_control_callback.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "i_osa_api.h"
//Size of timer event table in sub-frames
#define NUM_SUBF_IN_TIME_EVENT_TAB (2)
#define TIME_EVENT_TAB_ENTRIES_NUM ((NUM_SUBF_IN_TIME_EVENT_TAB*SUBFRAME_USECS)/SCHED_INTERVAL_USECS)
class CTarget;

using namespace ns_5g_phy;
using namespace std;
using boost::property_tree::ptree;

struct CSchedulerData								// table for each event (10usec)
													// every registrated module for timer is registered in this structure
{
	TimerEvent_api* callback_timer_api;		// pointer to the timer event API
	uint32_t        cookie;					// Special value to send in the case of timing event
	uint32_t		seq_val;				// Sequence value, used to set periodic parameters, like symbol numbers
	int32_t		    target_offset;			// The event is called in offset relative the target event. The target events may be slot, symbol start, etc.

	CSchedulerData()
	{
		Init();
	}
	void Init()
	{
		cookie = 0;
		callback_timer_api= NULL;
		target_offset = 0;
		seq_val = -1;

	}
};

/*!
 *@class Time event driver
 *@brief The purpose of this class is :
 *@brief Separate time event driver fields, which should be called as volatile and way should be alligned to 64 bytes
 *@brief process and ARM cache sync issues.
 */
struct  TimeEvParams
{
	struct timespec			last_linux_time_;
	bool			        is_running_ = false;
	bool					is_time_sync_ready_ = false;
	uint32_t				last_timer_event_scheduler_entry_ = 0;
	uint32_t				current_timer_event_scheduler_entry_ = 0;
	SysTimeT 				sys_time_;
};

/*!  
 *@class CRSE_Control 
 *@brief The purpose of this class is :  
 *@brief It is the main class of Running Support Environment (RSE) 
 *@brief This class is responsible for connect with user frontend and to 5G modules. 
 *@brief The class support control and debug services for running 5G PHY. 
 */ 
class CRSE_Control : public IGL_ControlAPI, public IGL_DebugAPI
{
	IGL_ControlCallBackAPI  *control_callback_;
	CMemArea				mem_areas_[MAX_AREAS];
	// table for the permanent timers event (every 10usec)
	CModuleControlCallBack	modules_[MAX_MODULES];
	char					mod_name_[80];
	CModuleControlCallBack  top_services_;  // Support logs , traces and profile counters.
	IModuleControlCallBackAPI* callback_ptr_;
	CProfileCnt				prof_cnt_gen_;
	CProfileCnt				prof_timer_;
	uint32_t 				num_modules_;
	uint32_t				num_areas_;
	CTarget					*Target_p_;
	CMemArea 				*time_area_p_;
	uint32_t				prof_cnt_num_;
	uint32_t				logs_areas_num_;
	CMemAreaP				logs_areas_[MAX_MODULES];
	CMemAreaP				perf_cntr_areas_[MAX_AREAS];
	int32_t                run_period_usec_;
	CMemArea			   *timer_events_q_ = NULL;

	//This fields are used for delay function support
	double					timer_ratio_= 0; //Ratio between clock number and time
	uint32_t				timer_correct_ = 0; //Additional time, added due to timer preparation calculation.

	__align(CACHE_ALIGNMENT) volatile TimeEvParams timer_ev_params_;
	CSchedulerData			perm_timer_event_scheduler_[ TIME_EVENT_TAB_ENTRIES_NUM ][ MAX_TIMER_EVENTS ];
	volatile uint32_t				perm_sched_attached_cnts_[TIME_EVENT_TAB_ENTRIES_NUM];
	// table for the temporal timers event (every 10usec)
	CSchedulerData			tmp_timer_event_scheduler_[ TIME_EVENT_TAB_ENTRIES_NUM ][ MAX_TIMER_EVENTS ];
	volatile uint32_t				tmp_sched_attached_cnts_[TIME_EVENT_TAB_ENTRIES_NUM];
	OSA_thread_t            main_thread_;
	OSA_mutex_t             sync_mutex_;
	OSA_spinlock_t 		   timer_lock_;

	SysTimeT SysTimeUpdate(uint32_t usecs, SysTimeT *sys_time, uint32_t nsec_remainder= 0)
	{
		SysTimeT *addrW= (SysTimeT *)time_area_p_->AllocFIFO();
		*addrW = *sys_time;
		addrW->SysTimeUpdate(usecs, nsec_remainder);
//Make updated data readable.
		time_area_p_->DualBufSwap();
		return *addrW;
	}

public:
	SysTimeT GetSysTime()
	{
		SysTimeT *addrR= (SysTimeT *)time_area_p_->GetReadBufP();
		ASSERT(addrR);

		return *addrR;
	}

	CRSE_Control();
	virtual ~CRSE_Control();
	//IGL_ControlAPI methods
	virtual EResultT IInit(	IGL_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info);
	virtual EResultT IColdStart() ;
	virtual EResultT IWarmStart() ;
	virtual EResultT IHotStart() ;
	virtual EResultT IStop(ESeverityT severity);
	virtual EResultT IConfigure(uint32_t id, void *in, void **out);
	virtual EResultT IGetInfo(char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info);
	virtual EResultT IExitReq(ESeverityT severity);
	//If fapi_req_p==NULL than there is not other requests in this TTI
	virtual EResultT IFAPI_req_put(void* fapi_req_p);
	//fapi_evt_p == NULL means no events.
	virtual EResultT IFAPI_evt_get(void** fapi_evt_p);

	// IGL_DebugAPI methods
	virtual EResultT IDebugInit(char* add_info);
	virtual EResultT IGetMemAreasNum(uint32_t *areas_num);
	virtual CMemArea *IGetMemArea(uint32_t area_num);
	virtual EResultT IGetModulesNum(uint32_t *modules_num);
	virtual CModuleInfo IGetModule(uint32_t module_num);
	virtual EResultT ISetLogSeverity(ESeverityT severity);
	virtual EResultT IProfilerSave(char* file_name) ;
	virtual EResultT ITraceSave(char* file_name);
	virtual EResultT ICall(SysTimeT *sys_time_p, uint32_t param);
	virtual EResultT ISetBP(SysTimeT *sys_time_p, char *data, uint32_t *id) ;
	virtual EResultT IClearBP(uint32_t id);
	virtual EResultT ISendCLI(char* command_str, char **respond);
	virtual CMemArea** 	IGetProfilingData(uint32_t *nuentries_);
	virtual CMemArea**	IGetLogData(uint32_t *nuentries_);

	//Synchronization system and Linux time of the system
	// Options:
	// sys_time_p == NULL && linux_time == NULL : Update the system time due to the current Linux time
	// sys_time_p != NULL && linux_time != NULL : Tuning the system time due to the sync point between Linux time and the System time
	//                                             If such configuration is called the first time that it starts the System time clocks
	// sys_time_p != NULL && linux_time == NULL : Immediately update the system time. It is used for off-line debugging
	EResultT ISyncTime(SysTimeT *sys_time_p = NULL, timespec *linux_time =NULL );

	void thread_run();

	//Local methods
	uint32_t AddModule(char* module_name, IModuleControlAPI *control_api, std::string &config_info);

	void AddMemArea(CMemAreaP *mearea_p_);

	void RegisterTimerEvent(TimerEventSchedulerT *sched_info);
	void SetTimerEvent(TimerEventSchedulerT *sched_info, SysTimeT *sys_time_p);
	void SetTarget(CTarget *ptr)
	{
		Target_p_ = ptr;
	}
	void SetSingleTimerEvent(TimerEventSchedulerT *sched_info, uint32_t timer_event_entry, int32_t num= -1);
	void DataReset()
	{
		uint32_t i,j;
		num_modules_= 0;
		Target_p_ = NULL;
		__align(CACHE_ALIGNMENT) TimeEvParams timer_ev_params;
		control_callback_ = NULL;

		prof_cnt_num_=0;
		prof_cnt_num_=0;
		SysTimeT *addrW= (SysTimeT *)time_area_p_->AllocFIFO();
		SysTimeT *addrR= (SysTimeT *)time_area_p_->GetReadBufP();
		timer_ev_params.sys_time_.reset();
		timer_ev_params.is_running_= false;
		timer_ev_params.is_time_sync_ready_= false;
		clock_gettime(CLOCK_MONOTONIC, &timer_ev_params.last_linux_time_);
		timer_ev_params.last_timer_event_scheduler_entry_=0;
		timer_ev_params.current_timer_event_scheduler_entry_ = 0;
		//There is problem to copy from not volatile to volatile structure, so memcpy is used
		memcpy((char*)&timer_ev_params_, (char*)&timer_ev_params, sizeof(timer_ev_params));
		addrW->reset();
		addrR->reset();
		for(i= 0; i< TIME_EVENT_TAB_ENTRIES_NUM; i++)
		{
			for(j=0; j< MAX_TIMER_EVENTS; j++)
			{
				tmp_timer_event_scheduler_[i][j].Init();
				perm_timer_event_scheduler_[i][j].Init();
			}
			tmp_sched_attached_cnts_[i]= 0;
			perm_sched_attached_cnts_[i]= 0;
		}

		for(i=0; i< MAX_MODULES; i++)
			modules_[i].Init(this);
		SetTimer();
		//Default setting for numerology 1;
	}
	//Setting timer parameters. Should be called during the system initialization
	void SetTimer()
	{
		struct timespec cur_time, prev_time;
		uint32_t i, nsec_diff, num= 100000000;
		clock_gettime(CLOCK_MONOTONIC, &prev_time);
		for(i=0;i<num;i++);
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		nsec_diff= DiffBetweenTimespec(cur_time, prev_time);
		timer_ratio_= ((double)num*1000)/nsec_diff;
		clock_gettime(CLOCK_MONOTONIC, &prev_time);
		uint32_t tmp= (uint32_t)timer_ratio_*num;
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		timer_correct_ = (DiffBetweenTimespec(cur_time, prev_time)+500)/1000;
	}

	void Delay_us(uint32_t usecs)
	{
		if(usecs <= timer_correct_)
		{
			return;
		}
		uint32_t num = (uint32_t)(usecs- timer_correct_)*timer_ratio_;
		uint32_t i;
		for(i=0;i<num;i++);
	}

};

#endif /* CGLCONTROLAPI_H_ */
