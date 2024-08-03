/******************************************************************
 * i_dpdk_buffer_manager.h
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_DPDK_BUFFER_MANAGER_API_H_
#define API_MODULE_DPDK_BUFFER_MANAGER_API_H_

#include "i_api_common.h"
#include "i_sys_types.h"
#include <rte_config.h>
#include <rte_mempool.h>
#include <rte_ethdev.h>
//#include <rte_mbuf_core.h>




#define DPDK_UPLANE_MTU_SIZE 9000
#define DPDK_CPLANE_MTU_SIZE 7000

#define DPDK_BUFFER_MANAGER_MODULE "DPDK_BUFFER_MANAGER_OFFLINE"

#define MAX_POOLNAME_LEN				(256)
#define MAX_POOL_OPSNAME_LEN			(256)

#define MAX_NUM_MEMPOOLS_PER_MANAGER	(10)

// Buffer Manager settings
/*
typedef struct mempool_settings
{
	int8_t					pool_name[MAX_POOLNAME_LEN] ;
	uint16_t				buffers_number;
	uint16_t				buffers_size;
	uint16_t				cache_size;
	uint16_t				private_data_size;
	rte_mempool_ctor_t*		mp_init;
	void*					mp_init_arg;
	rte_mempool_obj_ctor_t*	obj_init;
	void*					obj_init_arg;
	int16_t					socket_id;
	uint16_t				flags;
} mempool_settings_t;
*/
typedef struct mempool_settings
{
	int8_t					pool_name[MAX_POOLNAME_LEN] ;
	uint32_t				buffers_number;
	uint16_t				buffers_size;
	uint16_t				cache_size;
	uint16_t				private_data_size;
	int16_t					socket_id;
	int8_t            		pool_ops_name[MAX_POOL_OPSNAME_LEN];
	} mempool_settings_t;


typedef struct buffer_manager_settings
{
	uint16_t				number_mempools;
	mempool_settings_t		mempool_setup[MAX_NUM_MEMPOOLS_PER_MANAGER];

} buffer_manager_settings_t;


/*******************************************************************************************//*!
*@class Dpdk_Buffer_Manager_API
*@brief The purpose of this class is :
*@brief Interface for DPDK Buffer Manager
*@brief
***********************************************************************************************/
struct Dpdk_Buffer_Manager_API
{
	// creates and initiates all mempools for this Buffer Manager according to settings structure
	virtual EResultT Init(buffer_manager_settings_t* settings) = 0;

	//returns most proper mempool to user according to buff_size
	//if nothing matches - return NULL
	virtual struct rte_mempool * getMempool(uint32_t buff_size) = 0;

	//get a mempool handle according to the pool-name. returns failure if mempool name does not exist on this Buffer manager
	virtual struct rte_mempool* getMempoolHandleByName(const char* pool_name) = 0;

	//create ring of mbuffs & return pointer to ring to user according to the mempool
	virtual struct rte_ring* preAllocRing(uint32_t buff_size, char* ring_name, struct rte_mempool* mempool_ptr) = 0;

} ;

#endif /* API_MODULE_DPDK_BUFFER_MANAGER_API_H_ */
