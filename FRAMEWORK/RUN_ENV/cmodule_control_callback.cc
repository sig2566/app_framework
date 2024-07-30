/*
 * cmodule_control_callback.cc
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */

#include "cmodule_control_callback.h"
#include "crse_control.h"

CModuleControlCallBack::CModuleControlCallBack() {
	// TODO Auto-generated constructor stub
	main_class_ptr_= NULL;
	prof_cnt_num_= 0;
	cur_severity_ = E_INFO;
    prof_cnt_q_head_= NULL;
    prof_cnt_q_tail_= NULL;


}

CModuleControlCallBack::~CModuleControlCallBack() {
	// TODO Auto-generated destructor stub
}

EResultT CModuleControlCallBack::IMemAreaDefine(CMemAreaP *mearea_ptr_)
{
	main_class_ptr_->AddMemArea(mearea_ptr_);
	return E_OK;
}
EResultT CModuleControlCallBack::IRegistryTraceEntry(char *format_str, uint32_t *id)
{
	uint32_t tmp;
	tmp = RTDBG_AddTraceEntry(rt_debug_grp_, format_str);
	*id= tmp;
	return E_OK;
}

EResultT CModuleControlCallBack::IAllocateEventCnt(char *cnt_name, volatile int64_t **cnt_ptr)
{
	*cnt_ptr = RTDBG_AllocCounter(rt_debug_grp_, cnt_name);
	return E_OK;
}

EResultT CModuleControlCallBack::IRegistryProfileEntry(CProfileCnt *ptr, char *name, uint32_t *prof_id)
{
    char buf[200];
    ASSERT(ptr->GetNextProf() == NULL);
    if(prof_cnt_q_head_==NULL)
    {
        prof_cnt_q_head_ = ptr;
        prof_cnt_q_tail_ = ptr;
    }
    else
    {
        prof_cnt_q_tail_->ConnectProf(ptr);
        prof_cnt_q_tail_ = ptr;
    }
    *prof_id = RTDBG_AddProfiler(rt_debug_grp_, name);
	return E_OK;
}

EResultT CModuleControlCallBack::ISaveProfileInfo(uint32_t prof_id, ProfileData *data)
{
	RTDBG_SaveProfMeas(rt_debug_grp_, prof_id, data);
	return E_OK;
}

void CModuleControlCallBack::StopModule(ESeverityT severity)
{
    module_info_.module_p_->IStop(severity);

}
void CModuleControlCallBack::GetProfileInfo()
{
	CProfileCnt *ptr = prof_cnt_q_head_;
    //Collect profiling data over all module profiling counters
    while(ptr != NULL)
    {
        ptr->ForceStop();
        ptr = ptr->GetNextProf();
    }
}
CMemAreaP*  CModuleControlCallBack::GetProfileCntrs(uint32_t *cnt_num)
{
	*cnt_num = prof_cnt_num_;
	return prof_cnt_tab_;
}
EResultT CModuleControlCallBack::IRegisterTimerEvent(TimerEventSchedulerT *sched_info)
{
	main_class_ptr_->RegisterTimerEvent(sched_info);
	return E_OK;
}
EResultT CModuleControlCallBack::ILogData(ESeverityT severity, char *str)
{
	SysTimeT sys_time;
	if(severity <= cur_severity_)
	{
		sys_time = main_class_ptr_->GetSysTime();
		RTDBG_AddLog(rt_debug_grp_, &sys_time, str);
	}
	return E_OK;
}

EResultT CModuleControlCallBack::ITraceData(uint32_t id, uint32_t line_num, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
	SysTimeT sys_time;
	IGetSysTime(&sys_time);
	RTDBG_AddTrace(rt_debug_grp_, id, line_num, &sys_time, val0 , val1, val2, val3);
	return E_OK;
}
EResultT CModuleControlCallBack::IStopRequest(ESeverityT severity)
{
	RTDBG_Stop();
	return E_OK;
}
EResultT CModuleControlCallBack::IGetSysTime(SysTimeT *sys_time_p)
{
	*sys_time_p= main_class_ptr_->GetSysTime();

	return E_OK;
}
//Class Functions
void CModuleControlCallBack::AddModule(char* module_name, IModuleControlAPI *control_api, std::string &config_info)
{
	module_info_.Init();
	strcpy(module_info_.module_name_, module_name);
	module_info_.module_p_ = control_api;
	config_info_ = config_info;
	//Get RT debug group
	rt_debug_grp_ = RTDBG_AllocRTDebugGroup(module_info_.module_name_);
	if(rt_debug_grp_== -1)
	{
		printf("Warning: Debug group allocation was failed for module %s\n", module_info_.module_name_);
	}

}

CModuleInfo* CModuleControlCallBack::GetModuleInfo()
{
	return &module_info_;
}

EResultT CModuleControlCallBack::IGetModule(char mod_name[], IModuleControlAPI **mod_ptr)
{
	uint32_t numodules_;
	uint32_t i;
	main_class_ptr_->IGetModulesNum(&numodules_);
	*mod_ptr = NULL;
	for(i= 0; i < numodules_; i++)
	{
		CModuleInfo mod= main_class_ptr_->IGetModule(i);
		if(strcmp(mod.module_name_, mod_name) == 0)
		{
			*mod_ptr = mod.module_p_;
			return E_OK;
		}
	}
	ASSERT(FALSE);
	return E_FAIL;

}
EResultT CModuleControlCallBack::IMemAreaMount(CMemAreaP *mearea_ptr_, char area_name[], EAccessT ac_type)
{
	uint32_t nuareas_;
	uint32_t i;
	main_class_ptr_->IGetMemAreasNum(&nuareas_);
	*mearea_ptr_ = NULL;
	for(i= 0; i < nuareas_; i++)
	{
		CMemArea* area_ptr= main_class_ptr_->IGetMemArea(i);
		if(strcmp(area_ptr->GetName(), area_name) == 0)
		{
			//Check that only one module may get write access to some area.
			if(ac_type == E_WRITE)
			{
				if(area_ptr->GetAccess()== E_WRITE)
				{
					ASSERT(FALSE);
					*mearea_ptr_ = NULL;
					return E_FAIL;
				}
				area_ptr->SetAccess(ac_type);
			}
			*mearea_ptr_ = area_ptr;
			return E_OK;
		}
	}
	ASSERT(FALSE);
	return E_FAIL;

}
//Synchronization system and Linux time of the system
// Options:
// sys_time_p == NULL && linux_time == NULL : Update the system time due to the current Linux time
// sys_time_p != NULL && linux_time != NULL : Tuning the system time due to the sync point between Linux time and the System time
//                                             If such configuration is called the first time that it starts the System time clocks
// sys_time_p != NULL && linux_time == NULL : Immediately update the system time. It is used for off-line debugging
EResultT CModuleControlCallBack::ISyncTime(SysTimeT *sys_time_p , timespec *linux_time )
{
	main_class_ptr_->ISyncTime(sys_time_p, linux_time);

	return E_OK;
}

EResultT CModuleControlCallBack::IDelay_us(uint32_t usecs)
{
	main_class_ptr_->Delay_us(usecs);
	return E_OK;
}
