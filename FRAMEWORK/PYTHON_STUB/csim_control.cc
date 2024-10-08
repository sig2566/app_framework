/******************************************************************
 * csim_control.cc
 * Author: Igor Shoihet
 *******************************************************************/

#include "csim_control.h"

CSimControl::CSimControl() {
	// TODO Auto-generated constructor stub
	uint32_t i;
	target_ptr_ = NULL;
	control_ptr_ = NULL;
	debug_ptr_ = NULL;
	num_areas_ = 0;
	num_modules_ = 0;
	sys_time_idx_ = -1;
	for(i=0; i< MAX_AREAS; i++)
	{
		mem_areas_p_[i] = NULL;
	}

	for(i=0; i< MAX_MODULES; i++)
	{
		modules_[i].Init();
	}
	prof_info_.clear();
	last_logs_.clear();
	log_tab_p_ = NULL;

}

CSimControl::~CSimControl() {
	// TODO Auto-generated destructor stub
}

void CSimControl::Init(const char *cfg_path)
{
	EResultT res;
	uint32_t i;

	char modname[80] = "./RUN_ENV.so";
	IGetConnectAPI_t get_connect_api;
	void *tmp_ptr = NULL;
	uint32_t index = -1;

	//TODO Loads modules due too sysconfig.xml file
	dlerror();
	void* handle = dlopen(modname, RTLD_LAZY);
    if (!handle) {
		char buf[GEN_BUF_SIZE];
		sprintf(buf,"%s",dlerror());
		printf("Error load module %s : %s\n",modname, buf);
    	ASSERT(0);
    }
    num_areas_ = 0;
    // reset errors
    dlerror();
    get_connect_api = (IGetConnectAPI_t)dlsym(handle, "IGetConnectAPI");
    const char *dlsyerror_ = dlerror();
    if (dlsyerror_) {
    	//TODO add errror indication,
         dlclose(handle);

    }

    get_connect_api(&tmp_ptr);

    if(tmp_ptr == NULL)
    {
    	ASSERT(0);
    }
    target_ptr_ = reinterpret_cast<ITarget*>(tmp_ptr);
	target_ptr_->IGetTarget(E_CONTROL_API, 0,  &tmp_ptr);
	control_ptr_ = (IRSE_ControlAPI*)tmp_ptr;
	ASSERT(control_ptr_);

	target_ptr_->IGetTarget(E_DEBUG_API, 0,  &tmp_ptr);
	debug_ptr_ = (IRSE_DebugAPI*)tmp_ptr;
	ASSERT(debug_ptr_);
	//char *tmp_str= cfg_path;
	res = control_ptr_->IInit(reinterpret_cast<IRSE_ControlCallBackAPI*>(this), cfg_path, NULL);
	ASSERT(res == E_OK);

	//Get shared memory areas and modules access for debugging
	res = debug_ptr_->IGetMemAreasNum(&num_areas_);
	ASSERT(res == E_OK);

	res = debug_ptr_->IGetModulesNum(&num_modules_);
	ASSERT(res == E_OK);

	for(i=0; i< num_areas_; i++)
	{
		char name[] = "SYS_TIME";
		mem_areas_p_[i] = debug_ptr_->IGetMemArea(i);
		if(strcmp((*mem_areas_p_)[i].name_,name)== 0)
		{
			sys_time_idx_ = i;
		}

	}

	for(i=0; i< num_modules_; i++)
	{
		modules_[i] = debug_ptr_->IGetModule(i);
	}


}

void CSimControl::RestartHot()
{
	EResultT res;
	res=control_ptr_->IHotStart();
	ASSERT(res == E_OK);

}

void CSimControl::Stop()
{
    control_ptr_->IStop(ai_framework_proj::E_HIGH);
}
void CSimControl::Start(uint32_t sfn, uint32_t usec)
{
	CMemArea *meptr_= mem_areas_p_[sys_time_idx_];
	debug_ptr_->ICall((sfn*1000)+usec);
}
CMemArea **CSimControl::GetMemAreas(uint32_t *nuareas_)
{
	*nuareas_ = num_areas_;
	return mem_areas_p_;

}
CModuleInfo* CSimControl::GetModules(uint32_t *numodules_)
{
	*numodules_ = num_modules_;
	return modules_;
}
void CSimControl::GetInfo(EDebugOperationT cmd, void *input, void **out)
{

}
EResultT CSimControl::IStopRequest(ESeverityT severity)
{
	return E_OK;
}
EResultT CSimControl::I_TTI_evt()
{
	return E_OK;
}

std::vector<std::string> &CSimControl::GetProfilerCntrs()
{
	uint32_t nuprof_cnt_, i;
	uint32_t measure_cnt, max_measure_cnt;
	CMemArea** cnt_tab_p;
	FILE    *f_out;

	prof_info_.clear();
	cnt_tab_p = debug_ptr_->IGetProfilingData(&nuprof_cnt_);
	for(i=0; i<nuprof_cnt_; i++)
	{
		measure_cnt=0;
		ProfileData *cnt_p, cnt_val;
		char buf[200];
		cnt_val.Reset();

#ifdef PROFILING_TEST
		char filename[200];
		strcpy(filename,"..//Tests//RU//ProfilingTest//");
		strcat(filename,strchr (cnt_tab_p[i]->GetName(),'.')+1);
		strcat(filename,".csv");
		f_out=fopen(filename,"w");
		ASSERT(f_out)
		fprintf(f_out,"Average, Max, Last_cnt, Max_measure_cnt, Max_cnt_time(nsec)\n");
#endif
		while((cnt_p= (ProfileData*)cnt_tab_p[i]->PopFIFO()) !=NULL)
		{

			//fwrite(cnt_p,sizeof(CProfilePoint),1,f_out);
			measure_cnt++;
			cnt_val.last_cnt_ = cnt_p->last_cnt_;
			if(cnt_val.max_cnt_ < cnt_p->max_cnt_)
			{
			    cnt_val.max_cnt_ = cnt_p->max_cnt_;
			    cnt_val.max_cnt_time_ = cnt_p->max_cnt_time_;
			    max_measure_cnt = measure_cnt;
			}
			cnt_val.average_cnt_ += cnt_p->average_cnt_ * cnt_p->meas_num_;
			cnt_val.meas_num_ += cnt_p->meas_num_;
		}
		if(cnt_val.meas_num_ > 0)
		{
		    cnt_val.average_cnt_ = cnt_val.average_cnt_ / cnt_val.meas_num_;
		    sprintf(buf,"%s | %lu | %lu | %lu | %lu | %f", cnt_tab_p[i]->GetName(),  cnt_val.average_cnt_, cnt_val.max_cnt_, cnt_val.last_cnt_, cnt_val.meas_num_, cnt_val.max_cnt_time_*1e-9);
            prof_info_.push_back(buf);
#ifdef PROFILING_TEST
            fprintf(f_out,"%llu, %llu, %llu, %u, %f\n",cnt_val.average_cnt_, cnt_val.max_cnt_, cnt_val.last_cnt_, cnt_val.meas_num_, cnt_val.max_cnt_time_*1e-9);
#endif
		}
#ifdef PROFILING_TEST
		fclose(f_out);
#endif
	}
	return prof_info_;
}

//Extract logs from modules with sorting by time.
std::vector<std::string> &CSimControl::GetLastLogs()
{
	char log_str[1000];
	timespec linux_time;
	while(RTDBG_GetLog(log_str, &linux_time, -1)== true)
	{
		last_logs_.push_back(log_str);
	}
	return last_logs_;
}
