/*
 * crse_control.cc
 * Author: Igor Shoihet 
 */

#include "crse_control.h"
#include "target.h"


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
	char mod_name[] = "RUN_ENV.so";
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

EResultT CRSE_Control::IInit(IRSE_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info)
{
	char default_cnf_file[] = "sysconfig.xml";
	char prof_cnt_name[]= "GEN";
	char prof_ev_timer_[] = "EV_TIMER";
	string cfg_file;
	IGetConnectAPI_t get_connect_api;
	void *tmp_ptr = NULL;
	uint32_t index = -1;
	IModuleControlAPI *mod_ptr = NULL;
	std::string config_str = "";


	//Timer initialization
	DataReset();

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
	string rt_data_file = pt.get<string>("shared_debug_file", "rt_debug.dat");
    RTDBG_SetSharedMemFile(rt_data_file.c_str());
    RTDBG_ActivateDebugDataCollection();
    RTDBG_Start();

    //Define profiler and logs of this module
	top_services_.Init(static_cast<IRSE_DebugAPI *>(this));
	top_services_.AddModule(mod_name_, NULL, config_str);
	prof_cnt_gen_.Init(mod_name_, prof_cnt_name);
	prof_timer_.Init(mod_name_, prof_ev_timer_);



	BOOST_FOREACH( boost::property_tree::ptree::value_type const& v, pt.get_child("components_list") )
	{
		if( v.first == "component" )
		{

			char mod_name[GEN_BUF_SIZE];
			char buf[GEN_BUF_SIZE];
			char file_name[GEN_BUF_SIZE*2];
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
			char entry_func_name[200];
			sprintf(entry_func_name,"%s_IGetConnectAPI", modname.c_str());
			get_connect_api = (IGetConnectAPI_t)dlsym(handle, entry_func_name);
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
		char str_tmp[100];
		modules_[i].module_info_.module_p_->IGetInfo(mod_name, &major, &minor, &build_num, str_tmp);
		if(strcmp(modules_[i].module_info_.module_name_, mod_name)!= 0)
		{
			printf("Warning: module %s has wrong internal name: %s\n",
					modules_[i].module_info_.module_name_,mod_name);
		}
		LOG(E_HIGH,"Loaded module %s Ver: %d.%d build:%d date: %s",modules_[i].module_info_.module_name_, major, minor, build_num, str_tmp);
	}
	//Init module
	for(i=0; i < num_modules_; i++)
	{
		modules_[i].module_info_.module_p_->IInit( static_cast<IModuleControlCallBackAPI*>(&modules_[i]) , static_cast<ITarget*>(Target_p_), modules_[i].config_info_.data());
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

EResultT CRSE_Control::IConfigure(EConfigId id, void *in, void **out)
{
    //Configure scheduler due to numeroogy.
	return E_OK;
}

EResultT CRSE_Control::IGetInfo(const char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info)
{
	return E_OK;
}

EResultT CRSE_Control::IExitReq(ESeverityT severity)
{
	return E_OK;
}

EResultT CRSE_Control::IDebugInit(const char* add_info)
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

EResultT CRSE_Control::IProfilerSave(const char* file_name)
{
	return E_OK;
}

EResultT CRSE_Control::ITraceSave(const char* file_name)
{
	return E_OK;
}

//Execution the system
EResultT CRSE_Control::ICall(uint32_t param)
{
    run_period_usec_ = (int32_t)param;
    if(run_period_usec_ == 0)
    {
        run_period_usec_ = -1; //Setting for running forever
    }
    OSA_mutex_unlock(&sync_mutex_);
    OSA_sleep(10);
    // for RT debug - temporary
	printf("\n##################################################\n");
	printf(  "################## RT Run Start ##################\n");
	printf(  "##################################################\n\n");

    return E_OK;
}

EResultT CRSE_Control::ISetBP(const char *data, uint32_t *id)
{
	return E_OK;
}

EResultT CRSE_Control::IClearBP(uint32_t id)
{
	return E_OK;
}

EResultT CRSE_Control::ISendCLI(const char* command_str, char **respond)
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




void CRSE_Control::thread_run()
{

   if(run_period_usec_ == 0)
    {

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
    prof_cnt_gen_.Stop();

}

