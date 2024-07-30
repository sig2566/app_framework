/******************************************************************
 * dpdk_buffer_manager_offline.h
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_BUFFER_MANAGER_OFFLINE_H
#define DPDK_BUFFER_MANAGER_OFFLINE_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_dpdk_buffer_manager.h"

#define MAX_BUFFERS_TO_PREALLOCATE      (1024)  //for preallocating mbuff_arr

using namespace ns_5g_phy;

typedef struct mempool_info
{
	struct rte_mempool* mempool_ptr;
	uint32_t			mempool_size;
	uint32_t			mempool_id;
	int8_t				mempool_name[MAX_POOLNAME_LEN];
	uint32_t			mempool_consumers_number;
} mempool_info_t;

/******************************************************************************************//*!
 *@class CDPDK_Buffer_Manager
 *@brief This class represents DPDK Buffer Manager
 *@brief For DPDK mempools management
 *********************************************************************************************/

class CDPDK_Buffer_Manager : public Dpdk_Buffer_Manager_API
{
public:



	EResultT Init(buffer_manager_settings_t* settings);

	struct rte_mempool * getMempool(uint32_t buff_size);

	EResultT getMempoolHandleByName(const char* pool_name, struct rte_mempool* mempool_ptr);

	struct rte_mempool* getMempoolHandleByName(const char* pool_name);

	struct rte_ring* preAllocRing(uint32_t num_mbuffs, char* ring_name, struct rte_mempool* mempool_ptr);
    // local methods
	void Reset();
	void Config(IModuleControlCallBackAPI* ptr, uint32_t manager_id);




    CDPDK_Buffer_Manager()
    {
    	Reset();
    }

    ~CDPDK_Buffer_Manager()
    {

    }
private:

    //private methods
	uint32_t closest_pow2(uint32_t buffers_number);

    IModuleControlCallBackAPI*	callback_ptr_;
    uint32_t					buffer_manager_id_;
    mempool_info_t				dpdk_mempools_[MAX_NUM_MEMPOOLS_PER_MANAGER];
    //struct rte_mempool *		dpdk_mempools_[MAX_NUM_MEMPOOLS_PER_MANAGER];
    //uint32_t					mempool_sizes_[MAX_NUM_MEMPOOLS_PER_MANAGER];
    //uint32_t					mempool_consmers_number_[MAX_NUM_MEMPOOLS_PER_MANAGER];
    uint16_t					mempools_number_;
};

/******************************************************************************************//*!
 *@class CDpdkBufManagerFactory
 *@brief The purpose of this class is :
 *@brief This class holds the DPDK Buffers Managers Factory.
 *********************************************************************************************/
#define MAX_BUFFER_MANAGERS (8)
class CDpdkBufManagerFactory : public CBaseModule
{
    /***********************************************************************************/
    //Examples of useful data types (optional)
    CDPDK_Buffer_Manager	dpdk_buffer_managers_[MAX_BUFFER_MANAGERS];
    uint32_t 				num_buffer_managers_;
 public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api);
    EResultT IConfigure(uint32_t id, void *in, void **out);
    EResultT IColdStart();
    CDpdkBufManagerFactory()
    {
    	num_buffer_managers_ = 0;
    }
};



#endif /* DPDK_BUFFER_MANAGER_OFFLINE_H */
