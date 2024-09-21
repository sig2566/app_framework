/*
 * crse_control.h
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */


#ifndef CGLCONTROLAPI_H_
#define CGLCONTROLAPI_H_
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "i_api_common.h"
class CRSE_Control;
#include "cmodule_control_callback.h"

#include "i_osa_api.h"
//Size of timer event table in sub-frames
#define NUM_SUBF_IN_TIME_EVENT_TAB (2)
#define TIME_EVENT_TAB_ENTRIES_NUM ((NUM_SUBF_IN_TIME_EVENT_TAB*SUBFRAME_USECS)/SCHED_INTERVAL_USECS)
class CTarget;

using namespace ai_framework_proj;
using namespace std;
using boost::property_tree::ptree;


/*!  
 *@class CRSE_Control 
 *@brief The purpose of this class is :  
 *@brief It is the main class of Running Support Environment (RSE) 
 *@brief This class is responsible for connect with user frontend and to 5G modules. 
 *@brief The class support control and debug services for running 5G PHY. 
 */ 
class CRSE_Control : public IRSE_ControlAPI, public IRSE_DebugAPI
{
	IRSE_ControlCallBackAPI  *control_callback_;
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
	uint32_t				prof_cnt_num_;
	uint32_t				logs_areas_num_;
	CMemAreaP				logs_areas_[MAX_MODULES];
	CMemAreaP				perf_cntr_areas_[MAX_AREAS];
	int32_t                run_period_usec_;


	//This fields are used for delay function support
	double					timer_ratio_= 0; //Ratio between clock number and time
	uint32_t				timer_correct_ = 0; //Additional time, added due to timer preparation calculation.

	OSA_thread_t            main_thread_;
	OSA_mutex_t             sync_mutex_;


public:

	CRSE_Control();
	virtual ~CRSE_Control();
	//RSE_ControlAPI methods
	virtual EResultT IInit (IRSE_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info);
	virtual EResultT IColdStart() ;
	virtual EResultT IWarmStart() ;
	virtual EResultT IHotStart() ;
	virtual EResultT IStop(ESeverityT severity);
	virtual EResultT IConfigure(uint32_t id, void *in, void **out);
	virtual EResultT IGetInfo(const char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info);
	virtual EResultT IExitReq(ESeverityT severity);
	//If fapi_req_p==NULL than there is not other requests in this TTI
	virtual EResultT IFAPI_req_put(void* fapi_req_p);
	//fapi_evt_p == NULL means no events.
	virtual EResultT IFAPI_evt_get(void** fapi_evt_p);

	// IRSE_DebugAPI methods
	virtual EResultT IDebugInit(const char* add_info);
	virtual EResultT IGetMemAreasNum(uint32_t *areas_num);
	virtual CMemArea *IGetMemArea(uint32_t area_num);
	virtual EResultT IGetModulesNum(uint32_t *modules_num);
	virtual CModuleInfo IGetModule(uint32_t module_num);
	virtual EResultT ISetLogSeverity(ESeverityT severity);
	virtual EResultT IProfilerSave(const char* file_name) ;
	virtual EResultT ITraceSave(const char* file_name);
	virtual EResultT ICall(uint32_t param);
	virtual EResultT ISetBP(const char *data, uint32_t *id) ;
	virtual EResultT IClearBP(uint32_t id);
	virtual EResultT ISendCLI(const char* command_str, char **respond);
	virtual CMemArea** 	IGetProfilingData(uint32_t *nuentries_);
	virtual CMemArea**	IGetLogData(uint32_t *nuentries_);

	void thread_run();

	//Local methods
	uint32_t AddModule(char* module_name, IModuleControlAPI *control_api, std::string &config_info);

	virtual void AddMemArea(CMemAreaP *mearea_p_);

	void SetTarget(CTarget *ptr)
	{
		Target_p_ = ptr;
	}
	void DataReset()
	{
		uint32_t i,j;
		num_modules_= 0;
		Target_p_ = NULL;
		control_callback_ = NULL;

		prof_cnt_num_=0;
		prof_cnt_num_=0;
		for(i=0; i< MAX_MODULES; i++)
			modules_[i].Init(static_cast<IRSE_DebugAPI *>(this));
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

	virtual void Delay_us(uint32_t usecs)
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
