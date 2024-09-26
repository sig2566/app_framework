/******************************************************************
 * tcp_ip_driver.cc
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "i_dpdk_buffer_manager.h"
#include "dpdk_buffer_manager_offline.h"
#include <math.h>

/******************************************************************************************//*!
 *@function closest_pow2()
 *@brief The purpose of this function is :
 *@brief Calculate the closets power of two to the input number
 *@brief
 *@brief
 *@brief
 *@brief returns: The closest power of two to the input number.
 *********************************************************************************************/
uint32_t CDPDK_Buffer_Manager::closest_pow2(uint32_t buffers_number){

	return (buffers_number > 0) ? pow(2,ceil(log2(buffers_number))) - 1 : 0 ;
}

///////////////////// API methods implementation /////////////////////
/******************************************************************************************//*!
 *@function Init()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Buffer_Manager::Init(buffer_manager_settings_t* settings)
{
	EResultT retValue = E_OK;
	for(uint16_t mempool_count=0; mempool_count<settings->number_mempools; mempool_count++)
	{

		for(int32_t initiated_mempools_num=0; initiated_mempools_num<mempools_number_; initiated_mempools_num++)
		{
			if(settings->mempool_setup[mempool_count].buffers_size == dpdk_mempools_[initiated_mempools_num].mempool_size)
			{
				printf("ERROR: CDPDK_Buffer_Manager:: Trying to create mempool (%s). Current buffer manager already has mempool (%s) with required size (%d), please use it\n",
													(const char*)settings->mempool_setup[mempool_count].pool_name,
													(const char*)dpdk_mempools_[initiated_mempools_num].mempool_name,
													settings->mempool_setup[mempool_count].buffers_size);
				//retValue = E_FAIL; //open that when we will arrange and unit the mempools
				//continue; //open that when we will arrange and unit the mempools
			}
		}

		// Set buffers_number to the closest power of two to the current number.
		settings->mempool_setup[mempool_count].buffers_number = closest_pow2(settings->mempool_setup[mempool_count].buffers_number);
		dpdk_mempools_[mempool_count].mempool_ptr = rte_pktmbuf_pool_create_by_ops(
															(const char*)settings->mempool_setup[mempool_count].pool_name,
															settings->mempool_setup[mempool_count].buffers_number,
															settings->mempool_setup[mempool_count].cache_size,
															settings->mempool_setup[mempool_count].private_data_size,
															settings->mempool_setup[mempool_count].buffers_size,
															settings->mempool_setup[mempool_count].socket_id,
															nullptr //(const char *)settings->mempool_setup[mempool_count].pool_ops_name
															);

		
		if(NULL != dpdk_mempools_[mempool_count].mempool_ptr)
		{
			printf("CDPDK_Buffer_Manager(0x%llx):: New mempool created: Name = %s\tBufSize = %d\tNumBuffers = %d\tCacheSize = %d\n",
															this,
															(const char*)settings->mempool_setup[mempool_count].pool_name,
															settings->mempool_setup[mempool_count].buffers_size,
															settings->mempool_setup[mempool_count].buffers_number,
															settings->mempool_setup[mempool_count].cache_size);

			dpdk_mempools_[mempool_count].mempool_size = settings->mempool_setup[mempool_count].buffers_size;
			dpdk_mempools_[mempool_count].mempool_id = mempools_number_;
			strcpy((char*)&dpdk_mempools_[mempool_count].mempool_name[0], (const char*)settings->mempool_setup[mempool_count].pool_name);
			mempools_number_++;
		}
		else
		{
		    rte_exit(EXIT_FAILURE, "Pool create failed: %s\n", rte_strerror(rte_errno));

			retValue = E_FAIL;
			break;
		}
	}
	return retValue;
}

/******************************************************************************************//*!
 *@function getMempool
 *@brief The purpose of this function is :
 *@brief Get a mempool pointer according to the given buff_size
 *@brief Note: the function is not multithread-protected, so it cannot be used by different threads without external locking
 *@brief
 *@brief
 *@brief returns: pointer to specific mempool, NULL - if failed
 *********************************************************************************************/
struct rte_mempool * CDPDK_Buffer_Manager::getMempool(uint32_t buff_size)
{
	struct rte_mempool * mempool_ptr	 = NULL;
	uint32_t last_matched_mempool_size	 = 0xffffffff;
	uint32_t last_matched_mempool_id	 = 0;

	for(uint16_t mempool_count=0; mempool_count<mempools_number_; mempool_count++)
	{
		// find most matching pool for the given buffer size
//		if(buff_size == dpdk_mempools_[mempool_count].mempool_size) // exact buffer size was found - no need to search more, we should not have 2 mempools with the same buffer sizes
//		{
//			mempool_ptr = dpdk_mempools_[mempool_count].mempool_ptr;
//			break;
//		}
//		else if((buff_size <= dpdk_mempools_[mempool_count].mempool_size) &&
//				(dpdk_mempools_[mempool_count].mempool_size <= last_matched_mempool_size))
		if((buff_size <= dpdk_mempools_[mempool_count].mempool_size) &&
			(dpdk_mempools_[mempool_count].mempool_size <= last_matched_mempool_size))
		{
			last_matched_mempool_size	= dpdk_mempools_[mempool_count].mempool_size;
			last_matched_mempool_id		= dpdk_mempools_[mempool_count].mempool_id;
			mempool_ptr = dpdk_mempools_[mempool_count].mempool_ptr;
		}
	}
	if(NULL != mempool_ptr)
	{
		dpdk_mempools_[last_matched_mempool_id].mempool_consumers_number++;
	}
	return mempool_ptr;
}


/******************************************************************************************//*!
 *@function getMempoolHandleByName
 *@brief The purpose of this function is :
 *@brief Get a mempool pointer according to the given mempool name
 *@brief Note: the function is not multithread-protected, so it cannot be used by different threads without external locking
 *@brief
 *@brief
 *@brief returns: handle of the specific mempool, NULL - if failed to find
 *********************************************************************************************/
struct rte_mempool* CDPDK_Buffer_Manager::getMempoolHandleByName(const char* pool_name)
{
	struct rte_mempool* pool_handle = nullptr;

	for(uint16_t pool_count = 0; pool_count<mempools_number_; pool_count++)
	{
		if(!strcmp((const char*)dpdk_mempools_[pool_count].mempool_name, pool_name))
		{
			pool_handle = dpdk_mempools_[pool_count].mempool_ptr;
			break;
		}
	}
	return pool_handle;
}

/******************************************************************************************//*!
 *@function preAllocRing
 *@brief The purpose of this function is :
 *@brief Create a ring of preallocated Mbuffs from the given mempool
 *@brief (has to belong to this Buffer manager object, otherwise - return error)
 *********************************************************************************************/

struct rte_ring* CDPDK_Buffer_Manager::preAllocRing( uint32_t num_mbuffs,
													 char*     ring_name,
													 struct rte_mempool* mempool_ptr)

{
	struct rte_ring*  mbufs_prealloc_ring_ptr = NULL;
	struct rte_mbuf*  prealloc_mbufs_arr[MAX_BUFFERS_TO_PREALLOCATE];

	//printf("[%s] [%s] [%d] **********************   preAllocRing   MAX_BUFFERS_TO_PREALLOCATE[%d] num_mbuffs[%d]\n", __FILE__, __FUNCTION__, __LINE__ , MAX_BUFFERS_TO_PREALLOCATE, num_mbuffs);


	if (num_mbuffs > MAX_BUFFERS_TO_PREALLOCATE)
		{
			printf("Number of Mbuffs to allocate is larger than max Buff size\n");
			ASSERT(0);
		}

	//printf (" [%d] [%s] :num_mbuffs[%d] \n", __LINE__, __FILE__,num_mbuffs );

    //1. create ring
	mbufs_prealloc_ring_ptr = rte_ring_create(ring_name, rte_align32pow2(num_mbuffs) , SOCKET_ID_ANY, RING_F_SP_ENQ | RING_F_SC_DEQ);

	if (!mbufs_prealloc_ring_ptr)
	{
		printf("Could not create input mbufs ring. ERRNO: %s\n", rte_strerror(rte_errno));
		ASSERT(0);
	}

	//2. allocate ring of mbuf's
	if(rte_pktmbuf_alloc_bulk( mempool_ptr, prealloc_mbufs_arr, num_mbuffs))
	{
		printf("Could not preallocate input mbufs. ERRNO = %s\n", rte_strerror(rte_errno));
		ASSERT(0);
	}

	//4. put the array on the ring
	uint32_t free_space = 0;
	uint32_t obj_enqued =  rte_ring_enqueue_bulk(	mbufs_prealloc_ring_ptr,
													(void * const*)(prealloc_mbufs_arr),
													num_mbuffs,
													&free_space);
	if (obj_enqued !=	num_mbuffs)
	{
		printf("Could not enqueue %d packets to Ring. Packets dropped. Free space on Ring = %d, obj_enqued = %d\n", num_mbuffs, free_space, obj_enqued);
	}
	return mbufs_prealloc_ring_ptr;
}

// Private methods

void CDPDK_Buffer_Manager::Reset()
{
    callback_ptr_		= NULL;
    buffer_manager_id_	= 0xFFFF;
    mempools_number_	= 0;
    memset(dpdk_mempools_, 0, sizeof(mempool_info_t)*MAX_NUM_MEMPOOLS_PER_MANAGER);
}

void CDPDK_Buffer_Manager::Config(IModuleControlCallBackAPI* ptr, uint32_t manager_id)
{
	callback_ptr_		= ptr;
	buffer_manager_id_	= manager_id;

}


//
EResultT CDpdkBufManagerFactory::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
{

	CBaseModule::IInit(callback_ptr, target_api, init_info);
    LOG(E_MEDIUM,  "init passed");


    //char* argv[4] = {"-l" ,"0-3", "-n", "4"};
    //int argc = 5;
    //char* argv[2] = {"-l" ,"0-3"};
    //char* argv[4] = {"-l", "0-3", "--huge-dir", "/dev/hugepages"};
    //int argc = 5;
    //char* argv[4] = {"test", "-l", "0-3", "--no-huge"};
    /*
    char* argv[3] = {"test", "-l", "0-3"};
    int argc = sizeof(argv)/sizeof(char*);


	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");
     */

	//uint16_t DevCount = rte_eth_dev_count_avail();
	//printf("DevCount = %d\n", DevCount);



    return E_OK;
}

EResultT CDpdkBufManagerFactory::IColdStart()
	{
		//
		return E_OK;
	}

EResultT CDpdkBufManagerFactory::IConfigure(EConfigId id, void *in, void **out)
	{
		num_buffer_managers_++;
		ASSERT(num_buffer_managers_ < MAX_BUFFER_MANAGERS);
		dpdk_buffer_managers_[num_buffer_managers_].Config(callback_ptr_, num_buffer_managers_);
		*out = (void*)(static_cast<Dpdk_Buffer_Manager_API*>(&(dpdk_buffer_managers_[num_buffer_managers_])));
		return E_OK;
	}

class CDpdkBufManagerFactory     *g_module_ptr = NULL;


//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new CDpdkBufManagerFactory();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}

