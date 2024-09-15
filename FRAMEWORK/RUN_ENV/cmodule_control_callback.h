/*
 * cmodule_control_callback.h
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */

#ifndef CMODULECONTROLCALLBACK_H_
#define CMODULECONTROLCALLBACK_H_

#include "i_api_common.h"
using namespace ai_framework_proj;

class CRSE_Control;

/******************************************************************************************//*!
 *@class CModuleControlCallBack 
 *@brief The purpose of this class is :  
 *@brief Implementation asynchronious framework services for 5G PHY modules.  
 *@brief The services are called using IModuleControlCallBackAPI interface 
 *@brief Every 5G module is connected with corresponding object. 
 *********************************************************************************************/
class CModuleControlCallBack: public ai_framework_proj::IModuleControlCallBackAPI {
	IRSE_DebugAPI *main_class_ptr_;
	//Profiler counters support
	CMemAreaP 	     prof_cnt_tab_[MAX_PROFILE_CNT];
	CProfileCnt     *prof_cnt_q_head_;
	CProfileCnt     *prof_cnt_q_tail_;

	uint32_t		 prof_cnt_num_;

	//Logging support
	ESeverityT cur_severity_;
	uint32_t		rt_debug_grp_= -1;


public:
	CModuleInfo			module_info_;
	std::string		    config_info_;

	CModuleControlCallBack();
	virtual ~CModuleControlCallBack();
	EResultT IMemAreaDefine(CMemAreaP *mearea_ptr_);
	EResultT IRegistryTraceEntry(char *format_str, uint32_t *id);
	EResultT IRegistryProfileEntry(CProfileCnt *ptr, const char *name, uint32_t *prof_id);
	EResultT ILogData(ESeverityT severity, char *str);
	EResultT ITraceData(uint32_t id, uint32_t line_num, uint64_t val0= 0, uint64_t val1=0, uint64_t val2=0, uint64_t val3=0) ;
	EResultT IStopRequest(ESeverityT severity);
	EResultT IGetModule(const char mod_name[], IModuleControlAPI **mod_ptr);
	EResultT IMemAreaMount(CMemAreaP *mearea_ptr_, char area_name[], EAccessT ac_type);
	EResultT IDelay_us(uint32_t usecs);
	EResultT IAllocateEventCnt(const char *cnt_name, volatile int64_t **cnt_ptr);
	EResultT ISaveProfileInfo(uint32_t prof_id, ProfilePoint *data);

	void Init(IRSE_DebugAPI *ptr)
	{
		main_class_ptr_ = ptr;
		module_info_.Init();
	}

	//Class Functions
	void AddModule(char* module_name, IModuleControlAPI *control_api, std::string &config_info);
	CModuleInfo* GetModuleInfo();


	CMemAreaP  *GetProfileCntrs(uint32_t *cnt_num);
	void	   SetSeverity(ESeverityT val)
	{
		cur_severity_ = val;
	}


	void StopModule(ESeverityT severity);

	void GetProfileInfo();
};

#endif /* CMODULECONTROLCALLBACK_H_ */
