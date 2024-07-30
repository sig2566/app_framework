/******************************************************************
 * thread_mgr.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_ETH_H
#define DPDK_ETH_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "rte_string_fns.h"
#include <rte_config.h>
#include <rte_mempool.h>
#include <rte_ethdev.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_thread_api.h"
#include "dpdkEventDevThread.h"

const uint32_t MSG_CHUNK = 512; //Size of receive message data chunk
const uint32_t CHECK_SUM_SIZE = 1;
using namespace ns_5g_phy;

/******************************************************************************************//*!
 *@class ThreadFactory
 *@brief The purpose of this class is :
 *@brief This class holds the threads drivers.
 *********************************************************************************************/
#define MAX_THREADS (64)
class ThreadFactory : public CBaseModule
{
    /***********************************************************************************/
    //Thread profiler
	/*__align(CACHE_ALIGNMENT)*/ DpdkEventDevThread dpdk_thread_driver_[MAX_THREADS];
	/*__align(CACHE_ALIGNMENT)*/ CProfileCnt thread_prof_;  //Example of profiler counter

    uint32_t num_thread_drivers_;
    uint64_t alloc_thread_mask_;
    char eal_init_cmd_[GEN_BUF_SIZE];
 public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api);
    EResultT IConfigure(uint32_t id, void *in, void **out);
    EResultT IWarmStart();
    EResultT IHotStart();
    EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info);
    EResultT IStop(ESeverityT severity);
    //Inform to the factory about core thread allocation. Returns false if the core thread was allocated already.
    bool AllocThread(uint32_t core_num);
    ThreadFactory()
    {
    	num_thread_drivers_ = 0;
    	alloc_thread_mask_ = 0;
    	eal_init_cmd_[0] = 0;
    }
};



#endif /* DPDK_ETH_H */
