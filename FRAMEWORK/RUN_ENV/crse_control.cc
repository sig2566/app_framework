/*
 * crse_control.cc
 * Author: Igor Shoihet 
 * Copyright (c) 2018 - 2020 Airspan Networks 
 * All Rights Reserved.  
 * Confidential and Proprietary - Airspan Networks  
 */

#include "crse_control.h"
#include "target.h"
#include <boost/regex.hpp>

//Running thread wrapper function
static void *(start_frame_work_thread)(void *arg)
{
    CRSE_Control *control_ptr = (CRSE_Control*)arg;
    while(1)
        control_ptr->thread_run();
}

//General class methods


CRSE_Control::CRSE_Control() {
	// TODO Auto-generated constructor stub
	char mod_name[] = "GLUE";
	//DataReset();
	strcpy(mod_name_, mod_name);
	callback_ptr_= static_cast<IModuleControlCallBackAPI*>(&top_services_);
	logs_areas_num_ = 0;
	run_period_usec_ = -1;
}

CRSE_Control::~CRSE_Control() {
	// TODO Auto-generated destructor stub
}

EResultT CRSE_Control::IColdStart()
{
	uint32_t i;

	for(i=0; i < num_modules_; i++)
	{
		modules_[i].module_info_.module_p_->IColdStart();
	}
	return E_OK;
}

EResultT CRSE_Control::IInit(IGL_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info)
{
//	char modname[80] = "./SAMPLE_MODULE.so";
	char default_cnf_file[] = "sysconfig.xml";
	char prof_cnt_name[]= "GEN";
	char prof_ev_timer_[] = "EV_TIMER";
	string cfg_file;
	IGetConnectAPI_t get_connect_api;
	void *tmp_ptr = NULL;
	uint32_t index = -1;
	IModuleControlAPI *mod_ptr = NULL;
	std::string config_str = "";
	//Define profiler and logs of this module
	top_services_.Init(this);
	top_services_.AddModule(mod_name_, NULL, config_str);
	prof_cnt_gen_.Init(prof_cnt_name, callback_ptr_);
	prof_timer_.Init(prof_ev_timer_, callback_ptr_);

	{
		//Add system time area and init
		const uint32_t TIME_BUF_SIZE = 4;
		char time_area_name[]= "SYS_TIME";
		CMemArea time_area, *time_area_p;
		time_area.Setup(TIME_BUF_SIZE, sizeof(SysTimeT), time_area_name, E_WRITE, 0);
		time_area_p_ = &time_area;
		AddMemArea(&time_area_p_);

		//Add FIFO queue of timer event requests.
		char timer_events_req_area[] = "TIMER_EVENT_REQ";
		CMemArea tmp_req_area;
		tmp_req_area.Setup(MAX_TIMER_EVENTS, sizeof(CSchedulerData), timer_events_req_area, E_WRITE, 0);
		timer_events_q_= &tmp_req_area;
		AddMemArea(&timer_events_q_);

		for(int i= 0; i<TIME_BUF_SIZE; i++)
		{
			SysTimeT *time_p= (SysTimeT*)time_area_p_->AllocFIFO();
			time_p->nf =0;
			time_p->nsf = 0;
			time_p->offset = 0;
			time_area_p_->DualBufSwap();
		}


	}
	//Timer initialization
	DataReset();
	OSA_spinlock_create(&timer_lock_);

	//Add external modules from xlm list file
	if(config_file[0]!=0)
	{
		cfg_file= config_file;
	}
	else
	{
		cfg_file = default_cnf_file;
	}
//	istream is(cfg_file);
    ptree pt;
    read_xml(cfg_file, pt);
	BOOST_FOREACH( boost::property_tree::ptree::value_type const& v, pt.get_child("components_list") ) 
	{
		if( v.first == "component" )
		{

			char mod_name[GEN_BUF_SIZE];
			char buf[GEN_BUF_SIZE];
			char file_name[GEN_BUF_SIZE];
			std::stringstream s1;
			std::string modname = v.second.get<std::string>("name");
			ptree config_info_tree= v.second.get_child("config");
			std::string config;
			if(config_info_tree.empty())
			{
				config= v.second.get<std::string>("config");
			}
			else{
				//The config element body contains complex XML structure
				//This is used by ru/du_system modules
				write_xml(s1,config_info_tree);
				config = s1.str();

			}
			//printf("DEBUG: config=%s\n", config.data());
			strcpy(buf, modname.c_str());
			sprintf(file_name, "./%s.so", buf);
			dlerror();
			//Load so library
			void* handle = dlopen(file_name, RTLD_LAZY);
			if (!handle) {
				char buf[GEN_BUF_SIZE];
				sprintf(buf,"%s",dlerror());
				printf("Error load module %s : %s\n",file_name, buf);
				ASSERT(0);
			}

			// reset errors
			dlerror();
			get_connect_api = (IGetConnectAPI_t)dlsym(handle, "IGetConnectAPI");
			char *dlsyerror_ = dlerror();
			if (dlsyerror_) {
				printf( "Cannot load symbol IGetConnectAPI: %s\n", dlsyerror_);
				dlclose(handle);
				return E_FAIL;
			}

			get_connect_api(&tmp_ptr);

			if(tmp_ptr == NULL)
			{
				ASSERT(0);
			}
			mod_ptr = (IModuleControlAPI*)tmp_ptr;
			//Add module to DB.
			index = AddModule(buf, mod_ptr, config);
		}

	}
	uint32_t i;
	for(i=0; i < num_modules_; i++)
	{
		uint32_t major, minor, build_num;
		char mod_name[GEN_BUF_SIZE+1];
		strncpy(mod_name, modules_[i].module_info_.module_name_, GEN_BUF_SIZE);
		char *str_tmp= new char[modules_[i].config_info_.size()+GEN_BUF_SIZE] ;
		strcpy(str_tmp, modules_[i].config_info_.data());
		modules_[i].module_info_.module_p_->IGetInfo(mod_name, &major, &minor, &build_num, str_tmp);
		if(strcmp(modules_[i].module_info_.module_name_, mod_name)!= 0)
		{
			printf("Warning: module %s has wrong internal name: %s\n",
					modules_[i].module_info_.module_name_,mod_name);
		}
		delete str_tmp;
		LOG(E_HIGH,"Loaded module %s Ver: %d.%d build:%d date: %s",modules_[i].module_info_.module_name_, major, minor, build_num, str_tmp);
	}
	//Init module
	for(i=0; i < num_modules_; i++)
	{
		modules_[i].module_info_.module_p_->IInit( static_cast<IModuleControlCallBackAPI*>(&modules_[i]) , static_cast<ITarget*>(Target_p_));
	}

	control_callback_ = control_callback_ptr;

	//Fill logs and profiling counters tables
	{
		uint32_t i, j, nuprof_cnt_;;
		CMemAreaP	*prof_areas= NULL;
		for(i=0; i<num_modules_; i++)
		{
			//Get pointers to profiler counters areas
			prof_areas= modules_[i].GetProfileCntrs(&nuprof_cnt_);
			for(j=0; j< nuprof_cnt_; j++)
			{
				perf_cntr_areas_[prof_cnt_num_] = prof_areas[j];
				prof_cnt_num_++;
			}
		}
		//Add log and profiling info for this class

		prof_areas= top_services_.GetProfileCntrs(&nuprof_cnt_);
		for(j=0; j< nuprof_cnt_; j++)
		{
		  perf_cntr_areas_[prof_cnt_num_] = prof_areas[j];
		  prof_cnt_num_++;
		}


	}
	LOG(E_HIGH,"Loaded module %s Ver: %d.%d build_time: %s",MOD_NAME, MAJOR_ID, MINOR_ID, TIME_DATE);
	LOG(E_HIGH,"Finish init modules from %s file",cfg_file.data());
	IColdStart();
	IWarmStart();
	IHotStart();
	return E_OK;
}



EResultT CRSE_Control::IWarmStart()
{
	uint32_t i;
	run_period_usec_ = 0; //Inform the running thread to stop running
    OSA_mutex_create(&sync_mutex_);
    OSA_mutex_lock(&sync_mutex_);
    OSA_thread_create(&main_thread_, NULL, start_frame_work_thread, (void*)this);
	for(i=0; i < num_modules_; i++)
	{
		modules_[i].module_info_.module_p_->IWarmStart();
	}

	return E_OK;
}

EResultT CRSE_Control::IHotStart()
{
	uint32_t i;
	__align(CACHE_ALIGNMENT) TimeEvParams timer_ev_params;

	//It is necessary to sync rge system and Linux clocks after every hot start.
	timer_ev_params.is_time_sync_ready_= false;
	timer_ev_params.sys_time_.reset();
	SysTimeT *addrW= (SysTimeT *)time_area_p_->AllocFIFO();
	SysTimeT *addrR= (SysTimeT *)time_area_p_->GetReadBufP();
	clock_gettime(CLOCK_MONOTONIC, &timer_ev_params.last_linux_time_);
	timer_ev_params.last_timer_event_scheduler_entry_=0;
	timer_ev_params.current_timer_event_scheduler_entry_ = 0;
	memcpy((char*)&timer_ev_params_, (char*)&timer_ev_params, sizeof(timer_ev_params));
	addrW->reset();
	addrR->reset();


	for(i=0; i < num_modules_; i++)
	{
		modules_[i].module_info_.module_p_->IHotStart();
	}
	return E_OK;
}

EResultT CRSE_Control::IStop(ESeverityT severity)
{
    uint32_t i;
    run_period_usec_ = 0; //Inform the running thread to stop running
    //Wait to stop running
    while(timer_ev_params_.is_running_!=false)
    {
        OSA_sleep(20);
    }

    for(i=0; i < num_modules_; i++)
    {
        modules_[i].StopModule(severity);
    }
    for(i=0; i < num_modules_; i++)
    {
    	modules_[i].GetProfileInfo();
	}
	return E_OK;
}

EResultT CRSE_Control::IConfigure(uint32_t id, void *in, void **out)
{
    //Configure scheduler due to numeroogy.
	return E_OK;
}

EResultT CRSE_Control::IGetInfo(char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info)
{
	return E_OK;
}

EResultT CRSE_Control::IExitReq(ESeverityT severity)
{
	return E_OK;
}

//If fapi_req_p==NULL than there is not other requests in this TTI
EResultT CRSE_Control::IFAPI_req_put(void* fapi_req_p)
{
	return E_OK;
}

//fapi_evt_p == NULL means no events.
EResultT CRSE_Control::IFAPI_evt_get(void** fapi_evt_p)
{
	return E_OK;
}

EResultT CRSE_Control::IDebugInit(char* add_info)
{
	return E_OK;
}

EResultT CRSE_Control::IGetMemAreasNum(uint32_t *areas_num)
{
	*areas_num = num_areas_;
	return E_OK;
}
CMemArea*  	CRSE_Control::IGetMemArea(uint32_t area_num)
{
	ASSERT(num_areas_ > area_num);
	return &mem_areas_[area_num];;
}
EResultT CRSE_Control::IGetModulesNum(uint32_t *modules_num)
{
	*modules_num = num_modules_;
	return E_OK;
}
CModuleInfo CRSE_Control::IGetModule(uint32_t module_num)
{
	ASSERT(module_num < num_modules_);
	return modules_[module_num].module_info_;
}
EResultT CRSE_Control::ISetLogSeverity(ESeverityT severity)
{
	uint32_t i;
	for(i=0; i< num_modules_; i++)
	{
		modules_[i].SetSeverity(severity);
	}
	return E_OK;
}
CMemArea**  	CRSE_Control::IGetProfilingData(uint32_t *nuentries_)
{
	*nuentries_ = prof_cnt_num_;
	return perf_cntr_areas_;
}

CMemArea**	CRSE_Control::IGetLogData(uint32_t *nuentries_)
{
	*nuentries_ = logs_areas_num_;
	return logs_areas_;
}

EResultT CRSE_Control::IProfilerSave(char* file_name)
{
	return E_OK;
}

EResultT CRSE_Control::ITraceSave(char* file_name)
{
	return E_OK;
}

//Execution the system
EResultT CRSE_Control::ICall(SysTimeT *sys_time_p, uint32_t param)
{
    run_period_usec_ = (int32_t)param;
    if(run_period_usec_ == 0)
    {
        run_period_usec_ = -1; //Setting for running forever
    }
    timer_ev_params_.is_running_ = true;
    OSA_mutex_unlock(&sync_mutex_);
    OSA_sleep(10);
    // for RT debug - temporary
	printf("\n##################################################\n");
	printf(  "################## RT Run Start ##################\n");
	printf(  "##################################################\n\n");

    return E_OK;
}

EResultT CRSE_Control::ISetBP(SysTimeT *sys_time_p, char *data, uint32_t *id)
{
	return E_OK;
}

EResultT CRSE_Control::IClearBP(uint32_t id)
{
	return E_OK;
}

EResultT CRSE_Control::ISendCLI(char* command_str, char **respond)
{
	return E_OK;
}

//Local methods
uint32_t CRSE_Control::AddModule(char* module_name, IModuleControlAPI *control_api, std::string &config_info)
{
	uint32_t res= num_modules_;
	if(num_modules_>= MAX_MODULES )
		ASSERT(0);
	modules_[num_modules_].AddModule(module_name, control_api, config_info);
	num_modules_++;
	return res;
}

void CRSE_Control::AddMemArea(CMemAreaP *mearea_p_)
{
	uint32_t i;
	ASSERT(num_areas_ <= MAX_AREAS);
	for(i= 0; i<= num_areas_; i++)
	{
		if(strcmp(mem_areas_[i].name_, (*mearea_p_)->name_) == 0)
		{
			// Memory area should be added only in one place
			ASSERT(FALSE);
			*mearea_p_= NULL;
			return;

		}
	}

	mem_areas_[num_areas_]= **mearea_p_;
	mem_areas_[num_areas_].MemAlloc();
	*mearea_p_= &mem_areas_[num_areas_];
	num_areas_++;
}

//Add timer event request to the timer_events_q_
void CRSE_Control::RegisterTimerEvent(TimerEventSchedulerT *sched_info)
{
	ASSERT(sched_info->slot_offset< NUM_SUBF_IN_TIME_EVENT_TAB*NUM_SLOTS_SF); // NUM_SUBF_IN_TIME_EVENT_TAB*NUM_SLOTS_SF =4
	ASSERT(sched_info->usec_offset < SUBFRAME_USECS);
	timer_events_q_->PushFIFO_MT(sched_info, sizeof(TimerEventSchedulerT));
}
void CRSE_Control::SetSingleTimerEvent(TimerEventSchedulerT *sched_info, uint32_t timer_event_entry, int32_t num)
{
	//Only one event is added
	volatile CSchedulerData *sched_entry = NULL;
	if(sched_info->is_permanent)
	{
		ASSERT(perm_sched_attached_cnts_[timer_event_entry]< MAX_TIMER_EVENTS);
		sched_entry = &perm_timer_event_scheduler_[timer_event_entry][perm_sched_attached_cnts_[timer_event_entry]];
		perm_sched_attached_cnts_[timer_event_entry]++;
	}
	else
	{
		ASSERT(tmp_sched_attached_cnts_[timer_event_entry]< MAX_TIMER_EVENTS);
		sched_entry = &tmp_timer_event_scheduler_[timer_event_entry][tmp_sched_attached_cnts_[timer_event_entry]];
		tmp_sched_attached_cnts_[timer_event_entry]++;
	}

	if((num !=-1) && (sched_info->send_vals != NULL))
	{
		sched_entry->seq_val =  sched_info->send_vals[num];
	}
	sched_entry->cookie = sched_info->send_val;
	sched_entry->target_offset= sched_info->usec_offset;
	sched_entry->callback_timer_api = sched_info->callback_timer_api;
}
void CRSE_Control::SetTimerEvent(TimerEventSchedulerT *sched_info, SysTimeT *sys_time_p)
{
	uint32_t timer_event_entry = 0, i;
	uint32_t j, num_slots=1;
	//Calculate the entry in the timer events table, using sched_info data
	uint32_t curr_slot_pos = ((sys_time_p->nsf*NUM_SLOTS_SF + (SLOT_DURATION + sys_time_p->offset)/SLOT_DURATION) * SLOT_DURATION)%TIME_EVENT_TAB_ENTRIES_NUM;
	timer_event_entry = ((curr_slot_pos + sched_info->slot_offset)* SLOT_DURATION + sched_info->usec_offset)%TIME_EVENT_TAB_ENTRIES_NUM;

	if(sched_info->event_periodicy == E_EVERY_SLOT)
	{
		num_slots = NUM_SUBF_IN_TIME_EVENT_TAB * NUM_SLOTS_SF; //Set events for 4 slots. That is distance for timing events setting.
	}
	for(j=0; j< num_slots;  j++)
	{
		if(sched_info->event_offsets == NULL )
		{
			SetSingleTimerEvent(sched_info, timer_event_entry);
		}
		else
		{
			for(i=0; i < sched_info->num_events; i++)
			{
				uint32_t event_entry = (timer_event_entry + sched_info->event_offsets[i]/SCHED_INTERVAL_USECS)%TIME_EVENT_TAB_ENTRIES_NUM;
				SetSingleTimerEvent(sched_info, event_entry, i);
			}
		}
		timer_event_entry = (timer_event_entry + SLOT_DURATION) % TIME_EVENT_TAB_ENTRIES_NUM;
	}
}

//Synchronization system and Linux time of the system
// Options:
// sys_time_p == NULL && linux_time == NULL : Update the system time due to the current Linux time
// sys_time_p != NULL && linux_time != NULL : Tuning the system time due to the sync point between Linux time and the System time
//                                             If such configuration is called the first time that it starts the System time clocks
// sys_time_p != NULL && linux_time == NULL : Immediately update the system time. It is used for off-line debugging
// sys_time_p == NULL && linux_time != NULL : Debugging mode when the Linux time watch simulating by the test module. It is
//											  used for off line testing
EResultT CRSE_Control::ISyncTime(SysTimeT *sys_time_p, timespec *linux_time)
{
	TimerEventSchedulerT sched_info;
	OSA_spinlock_lock(&timer_lock_);
	//Update System time
	struct timespec cur_time;
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
  __align(CACHE_ALIGNMENT) TimeEvParams timer_ev_params;
	memcpy((char*)&timer_ev_params, (char*)&timer_ev_params_, sizeof(timer_ev_params));
	int64_t nsec_diff = DiffBetweenTimespec(cur_time, timer_ev_params.last_linux_time_);
	int64_t usec_diff = nsec_diff/1000;
	//Add new timer events
	while(timer_events_q_->PopFIFO_MT(&sched_info, sizeof(TimerEventSchedulerT)))
	{
		SetTimerEvent(&sched_info, &timer_ev_params.sys_time_);
	}

	//Sync the system time with the latest Linux time
	if((sys_time_p == NULL) && (linux_time == NULL))
	{
		//Update the system time due to change of the Linux time only if they hte system and Linux clocks were synchronized before.
		if(timer_ev_params.is_time_sync_ready_== true)
		{
			if(usec_diff > 0)
			{
				timer_ev_params.sys_time_ = SysTimeUpdate(usec_diff, &timer_ev_params.sys_time_, nsec_diff%1000);
				timer_ev_params.last_linux_time_ = cur_time;
			}
		}
		else //Fix 26/2/20 for calling CB in case symc time not updated
		{
			memcpy((char*)&timer_ev_params_, (char*)&timer_ev_params, sizeof(timer_ev_params));
			OSA_spinlock_unlock(&timer_lock_);
			return E_OK;

		}
	}
	else if((sys_time_p != NULL) && (linux_time != NULL))
	{
		//Tuning the system time due to synchronization point, provided by the function parameters
		int32_t diff_usecs = DiffBetweenTimespec(*linux_time, cur_time)/1000;
		timer_ev_params.sys_time_ = *sys_time_p;
		timer_ev_params.sys_time_ = SysTimeUpdate(-diff_usecs, &timer_ev_params.sys_time_);

		//Set the timer position into the timer events table.
		timer_ev_params.last_timer_event_scheduler_entry_ = (timer_ev_params.sys_time_.nsf*NUM_SLOTS_SF * SLOT_DURATION + timer_ev_params.sys_time_.offset) %TIME_EVENT_TAB_ENTRIES_NUM;
		timer_ev_params.last_linux_time_ = cur_time;
		//System and Linux clocks are in sync now.
		timer_ev_params.is_time_sync_ready_ = true;
	}
	else if ((sys_time_p != NULL) && (linux_time == NULL))
	{
		//Debugging mode. Immediate update the system time
		timer_ev_params.sys_time_ = *sys_time_p;
		timer_ev_params.sys_time_ = SysTimeUpdate(0, &timer_ev_params.sys_time_);
		timer_ev_params.last_linux_time_ = cur_time;
		//System and Linux clocks are out off sync now.
		timer_ev_params.is_time_sync_ready_ = false;

	}
	else
	{
		//Debugging mode. Update Linux time
		int64_t nsec_diff = DiffBetweenTimespec(*linux_time, timer_ev_params.last_linux_time_);
		uint64_t diff= nsec_diff/1000;
		uint64_t abs_diff = diff < 0 ? diff * (-1) : diff;

		if(abs_diff > SLOT_DURATION)
		{
			//Time was changed for more then 1 slot. It is assumed as debug initialize stage. So set new Linux debugging time
			timer_ev_params.last_timer_event_scheduler_entry_ = (timer_ev_params.sys_time_.nsf*NUM_SLOTS_SF * SLOT_DURATION + timer_ev_params.sys_time_.offset) %TIME_EVENT_TAB_ENTRIES_NUM;
			timer_ev_params.last_linux_time_ = *linux_time;
		}
		else if(diff > 0) //Linux time was increased less then 1 slot. Update the system time and call timer events
		{
			timer_ev_params.sys_time_ = SysTimeUpdate(diff, &timer_ev_params.sys_time_, nsec_diff%1000);
			timer_ev_params.last_linux_time_ = *linux_time;
		}

		//System and Linux clocks are out off sync now.
		timer_ev_params.is_time_sync_ready_ = false;


	}

	//Call timing events
	uint32_t current_timer_event_scheduler_entry_ =
			(timer_ev_params.sys_time_.nsf*NUM_SLOTS_SF * SLOT_DURATION + timer_ev_params.sys_time_.offset) %TIME_EVENT_TAB_ENTRIES_NUM;
	uint32_t pos = timer_ev_params.last_timer_event_scheduler_entry_;
	SysTimeT *read_sys_time = (SysTimeT *)time_area_p_->GetReadBufP();
	uint64_t prof_timer_ev_delay = 0;
	uint32_t timer_ev_period= (TIME_EVENT_TAB_ENTRIES_NUM + current_timer_event_scheduler_entry_ - pos)%TIME_EVENT_TAB_ENTRIES_NUM;
	while(pos != current_timer_event_scheduler_entry_)
	{
		uint32_t i;
		pos = (pos+1)%TIME_EVENT_TAB_ENTRIES_NUM;

		//Calculate profiling
		prof_timer_ev_delay += timer_ev_period * (tmp_sched_attached_cnts_[pos] + perm_sched_attached_cnts_[pos]);
		timer_ev_period--;

		//Send temporary events
		for(i=0; i< tmp_sched_attached_cnts_[pos]; i++)
		{
			SysTimeT dest_time= *read_sys_time;
			dest_time.SysTimeUpdate(-tmp_timer_event_scheduler_[pos][i].target_offset-timer_ev_period);
			tmp_timer_event_scheduler_[pos][i].callback_timer_api->TimerEvCallback(read_sys_time, tmp_timer_event_scheduler_[pos][i].cookie,
																					tmp_timer_event_scheduler_[pos][i].seq_val, &dest_time);
		}
		tmp_sched_attached_cnts_[pos] = 0; //Clean events table
		//Send permanent events
		for(i=0; i< perm_sched_attached_cnts_[pos]; i++)
		{
			SysTimeT dest_time= *read_sys_time;
			dest_time.SysTimeUpdate(-perm_timer_event_scheduler_[pos][i].target_offset-timer_ev_period);
			perm_timer_event_scheduler_[pos][i].callback_timer_api->TimerEvCallback(read_sys_time,
																					perm_timer_event_scheduler_[pos][i].cookie,
																					perm_timer_event_scheduler_[pos][i].seq_val,
																					&dest_time);
		}

	};
	prof_timer_.Stop(&prof_timer_ev_delay);
	timer_ev_params.last_timer_event_scheduler_entry_ = current_timer_event_scheduler_entry_;
	memcpy((char*)&timer_ev_params_, (char*)&timer_ev_params, sizeof(timer_ev_params));
	OSA_spinlock_unlock(&timer_lock_);
	return E_OK;
}

void CRSE_Control::thread_run()
{

   if(run_period_usec_ == 0)
    {
        //Stop running
    	timer_ev_params_.is_running_ = false;
        OSA_mutex_lock(&sync_mutex_);
    }
    if(run_period_usec_ == -1)
    { //Running forever
    }
    else
    {
    	run_period_usec_ = run_period_usec_ -5;
    	if(run_period_usec_< 0)
    	{
    		run_period_usec_ = 0;
    	}
    }
    prof_cnt_gen_.Start();
    usleep(5);
    ISyncTime(NULL, NULL);
    prof_cnt_gen_.Stop();

}

