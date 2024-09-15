/*
 * cmodule_control_callback.cc
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */

#include "cmodule_control_callback.h"


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

EResultT CModuleControlCallBack::IAllocateEventCnt(const char *cnt_name, volatile int64_t **cnt_ptr)
{
	*cnt_ptr = RTDBG_AllocCounter(rt_debug_grp_, (char*)cnt_name);
	return E_OK;
}

EResultT CModuleControlCallBack::IRegistryProfileEntry(CProfileCnt *ptr, const char *name, uint32_t *prof_id)
{

	return E_OK;
}

EResultT CModuleControlCallBack::ISaveProfileInfo(uint32_t prof_id, ProfilePoint *data)
{
	RTDBG_ProfFlushMeas(rt_debug_grp_, prof_id, data);
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

EResultT CModuleControlCallBack::ILogData(ESeverityT severity, char *str)
{

	if(severity <= cur_severity_)
	{
		RTDBG_AddLog(rt_debug_grp_, str);
	}
	return E_OK;
}

EResultT CModuleControlCallBack::ITraceData(uint32_t id, uint32_t line_num, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
	RTDBG_AddTrace(rt_debug_grp_, id, line_num, val0 , val1, val2, val3);
	return E_OK;
}
EResultT CModuleControlCallBack::IStopRequest(ESeverityT severity)
{
	RTDBG_Stop();
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

EResultT CModuleControlCallBack::IGetModule(const char mod_name[], IModuleControlAPI **mod_ptr)
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

EResultT CModuleControlCallBack::IDelay_us(uint32_t usecs)
{
	main_class_ptr_->Delay_us(usecs);
	return E_OK;
}
