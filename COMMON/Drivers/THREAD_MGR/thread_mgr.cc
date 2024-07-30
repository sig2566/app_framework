/******************************************************************
 * tcp_ip_driver.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "thread_mgr.h"

#define MAXPENDING (5)




//
#define MAX_EXTRA_ARGS 32
EResultT ThreadFactory::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api)
{
	uint32_t i;
	char *eargv[MAX_EXTRA_ARGS+1];
	uint32_t eargc;
	char app_name[]= "phy_app";
	int32_t ret;

	struct timespec cur_time, prev_time, tst_time;
	int64_t correct_usec;
	uint32_t  num=10000;
	clock_gettime(CLOCK_MONOTONIC, &prev_time);
	callback_ptr->IDelay_us(1000);
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
	int64_t usec_diff1 = DiffBetweenTimespec(cur_time, prev_time)/1000;
	clock_gettime(CLOCK_MONOTONIC, &prev_time);
	callback_ptr->IDelay_us(100);
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
	int64_t usec_diff = DiffBetweenTimespec(cur_time, prev_time)/1000;
	clock_gettime(CLOCK_MONOTONIC, &prev_time);
	callback_ptr->IDelay_us(10);
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
	int64_t usec_diff2 = DiffBetweenTimespec(cur_time, prev_time)/1000;

	CBaseModule::IInit(callback_ptr, target_api);
    //Initialization of the profiler counter
	INIT_PROF_CNT(THREAD_PROF, callback_ptr_, thread_prof_);

	for(i=0; i< MAX_THREADS; i++)
	{
		dpdk_thread_driver_[i].Init(callback_ptr_, this);
	}

	//Prepare parameters for eal init
	LOG(E_MEDIUM,"rte_eal_init: %s", eal_init_cmd_);
	eargv[0]= app_name;
	eargc = rte_strsplit(eal_init_cmd_, strlen(eal_init_cmd_),	&eargv[1], MAX_EXTRA_ARGS, ' ');

	ret = rte_eal_init(eargc + 1, eargv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Cannot init EAL\n");


	LOG(E_MEDIUM,  "Thread manager initialized");

    return E_OK;
}

EResultT ThreadFactory::IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info)
{
	if(strlen(eal_init_cmd_)== 0)
	{
		strncpy(eal_init_cmd_, add_info, GEN_BUF_SIZE);
	}

	return CBaseModule::IGetInfo(module_name, major_ver, minor_ver, build_num, add_info);

}


EResultT ThreadFactory::IConfigure(uint32_t id, void *in, void **out)
{
	if(id==e_PUT_INIT_DATA)
	{
		char param_name[]="dpdk_init";
		if(strcmp((char*)in, param_name)==0)
		{
			char *cmd= (char*)*out;
			strncpy(eal_init_cmd_, cmd, GEN_BUF_SIZE);
			return E_OK;
		}
		ASSERT(0);
	}
	num_thread_drivers_++;
	ASSERT(num_thread_drivers_ <= MAX_THREADS);
	*out = (void*)(static_cast<WorkerThread_api*>(&(dpdk_thread_driver_[num_thread_drivers_-1])));
	LOG(E_INFO,"Thread allocation id= %d",num_thread_drivers_-1);
	return E_OK;
}
EResultT ThreadFactory::IWarmStart()
{

	uint32_t i;
	for(i=0; i< num_thread_drivers_; i++)
	{
		uint32_t core_id= dpdk_thread_driver_[i].get_affinity();
		if( core_id!= -1)
		{
			dpdk_thread_driver_[i].start_thread(NULL, core_id);
		}
		else
		{
			LOG(E_CRITICAL, "Thread %d has no affinity", i);
			ASSERT(0);
		}
	}
	return E_OK;
}

EResultT ThreadFactory::IStop(ESeverityT severity)
{
	uint32_t i;
	//Stop running all threads. They are stopped reading the tasks queues.
	for(i=0; i< MAX_THREADS; i++)
	{
		if(dpdk_thread_driver_[i].get_affinity() != -1)
		{
			dpdk_thread_driver_[i].sync_stop();
		}
	}
	return E_OK;

}
EResultT ThreadFactory::IHotStart()
{

	uint32_t i;
	for(i=0; i< MAX_THREADS; i++)
	{
		if(dpdk_thread_driver_[i].get_affinity() != -1)
		{
			//dpdk_thread_driver_[i].start();
		}
	}
	return E_OK;
}


bool ThreadFactory::AllocThread(uint32_t core_num)
{
	uint64_t test_mask=0;
	if(core_num >= MAX_THREADS )
	{
		LOG(E_CRITICAL, "Wrong core allocated. The requested number is too big");
		return false;
	}
	test_mask = 1<<core_num;
	if(test_mask & alloc_thread_mask_)
	{
		LOG(E_CRITICAL, "Wrong core allocation. Core thread already allocated");
		return false;
	}
	alloc_thread_mask_ |= test_mask;
	return true;
}

class ThreadFactory     *g_module_ptr = NULL;


//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new ThreadFactory();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}

