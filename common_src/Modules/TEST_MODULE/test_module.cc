/*
 * SAMPLE_MODULE.cpp
 *
 *  Created on: Feb 12, 2018
 *      Author: swuser
 */

#include "i_api_common.h"
#include "version.h"

using namespace ns_5g_phy;
class CModuleControl : public IModuleControlAPI, public TimerEvent_api
{
	IModuleControlCallBackAPI* callback_ptr_;
	ITarget* 	target_ptr_;
	CMemArea *in_ptr, *out_ptr, *job_req_, *job_rsp_;
	char mod_name_[MDO_NAME_SIZE];
public:
	virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api)
	{
		EResultT res;
		uint32_t inc_size= 100;
		uint32_t size= 10;
		callback_ptr_= callback_ptr;
		target_ptr_= target_api;
		DEFINE_MEMAREA(size, inc_size, "TEST_MEM_AREA_1", 0, in_ptr);
		LOG(E_MEDIUM, "%s init passed", mod_name_);
		return E_OK;
	}
	virtual EResultT IColdStart()
	{
		MEMAREA_CONNECT("TEST_MEM_AREA_2", &out_ptr, E_WRITE);
		MEMAREA_CONNECT("TEST_MEM_AREA_1", &in_ptr, E_READ);
		MEMAREA_CONNECT("TEST_JOB_REQ", &job_req_, E_READ);
		MEMAREA_CONNECT("TEST_JOB_RSP", &job_rsp_, E_WRITE);

		return E_OK;
	}
	virtual EResultT IWarmStart()
	{
		return E_OK;
	}
	virtual EResultT IHotStart()
	{
		//Register the module for call at the beginning of every subframe
		TimerEventSchedulerT   sched_info;
		sched_info.usec_offset = 20;
		sched_info.send_val = (uint32_t)E_WAKEUP;
		sched_info.num_events= 1;
		sched_info.callback_timer_api= static_cast<TimerEvent_api*>(this);
		sched_info.is_permanent = true;
		sched_info.event_periodicy = E_EVERY_SLOT;
		callback_ptr_->IRegisterTimerEvent(&sched_info);
		return E_OK;
	}
	virtual void TimerEvCallback(SysTimeT *sys_time_p, int32_t param, int32_t seq_val, SysTimeT *dst_time)
	{
		ICall(sys_time_p, param);
	}
	virtual EResultT IStop(ESeverityT severity)
	{
		return E_OK;
	}
	virtual EResultT ICall(SysTimeT *sys_time_p, uint32_t param)
	{
		if((param == (uint32_t)E_WAKEUP) || (param == (uint32_t)E_DEBUG))
		{
			LOG(E_MEDIUM, "%s is called by debug or timer", mod_name_);
		}

		if(param == (uint32_t)E_DIRECT)
		{
			//Get request, handle it and send reponse
			JobReqBase_t *req = (JobReqBase_t*)job_req_->PopFIFO();
			JobRspBase_t *resp = (JobRspBase_t *)job_rsp_->AllocFIFO();
			uint32_t tmp;
			ASSERT(req != NULL);
			ASSERT(resp != NULL);
			resp->req_id = req->req_id;
			tmp = (*(uint32_t*)req->in_buf);
			(*(uint32_t*)req->out_buf) = tmp;

			//Send response
			resp->req_id = req->req_id;
			resp->res = E_OK;
			job_rsp_->PushFIFO();

			//Callback call
			if(req->call_back_p != NULL)
			{
				req->call_back_p->ICall(sys_time_p, (uint32_t)E_RESPONSE);
			}
			LOG(E_MEDIUM, "%s is called by other module", mod_name_);
		}
		return E_OK;
	}
	virtual EResultT IConfigure(uint32_t id, void *in, void **out)
	{
		char str[]= "Call passed";
		callback_ptr_->ILogData(E_LOW, str);
		return E_OK;
	}
	virtual EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info)
	{
		strcpy(module_name, mod_name_);
		*major_ver= MAJOR_ID;
		*minor_ver= MINOR_ID;
		*build_num= 0;
		strcpy(add_info,TIME_DATE);
		return E_OK;
	}
	CModuleControl(const char *mod_name)
	{
		callback_ptr_ = NULL;
		target_ptr_ = NULL;
		strcpy(mod_name_, mod_name);
		in_ptr = NULL;
		out_ptr = NULL;
		job_req_ = NULL;
		job_rsp_ = NULL;
	}
};

class CModuleControl     *g_module_ptr = NULL;

//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new CModuleControl(MOD_NAME);
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;
}
