/*
 * i_stitcher_engine_api.h
 *
 *  Created on: July 23, 2019
 *      Author: D. Matas
 */

#ifndef API_MODULE_API_I_STITCHER_ENGINE_API_H_
#define API_MODULE_API_I_STITCHER_ENGINE_API_H_

#include "i_api_common.h"
#include "ru_global_defines.h"
#include "i_ru_test_module_api.h"
#include "i_networking_api.h"
#include <pcap.h>

// Module name
#define STITCHER_ENGINE_MODULE	"STITCHER_ENGINE"

// Name of input FIFO for Data Engine JobDescriptors
#define STITCHER_ENGINE_FIFO_IN   "STITCHER_ENGINE_FIFO_IN_JOB_DESCS"

#define UL_USER_PLANE_PACKETS_MEM_NAME	"UL_USER_PLANE_PACKETS"

#define     STITCHER_INPUT_SIZE                  (MAX_NUM_SUB_CURRIERS_BH * sizeof(Complex) + MAX_SECTIONS_IN_UPLANE_PACKET * U_PLANE_SECTION_HEADER_MAX_SIZE)
#ifdef PCAP_HEADER_MODE
#define     STITCHER_INPUT_GROSS_SIZE            (MAX_NUM_SUB_CURRIERS_BH * sizeof(Complex) + MAX_SECTIONS_IN_UPLANE_PACKET * (U_PLANE_SECTION_HEADER_MAX_SIZE + ETHER_HDR_LEN + sizeof(pcap_pkthdr) + ECPRI_HEADER_SIZE))
#else
#define     STITCHER_INPUT_GROSS_SIZE            (MAX_NUM_SUB_CURRIERS_BH * sizeof(Complex) + MAX_SECTIONS_IN_UPLANE_PACKET * (U_PLANE_SECTION_HEADER_MAX_SIZE + ETHER_HDR_LEN + sizeof(OfflineDataHeader) + ECPRI_HEADER_SIZE))
#endif
/******************************************************************************************//*!
 *@class Ant_Group_Pool_API
 *@brief The purpose of this class is :
 *@brief This class expose the Antenna Groups Pool Module control API.
  *********************************************************************************************/
struct Stitcher_API
{
	virtual void		setRUsysVars(uint32_t* mtu_cpu_fh_size_ptr) = 0;
	virtual void		setStitcherSymbolJD_memory_ptr(uint8_t* StitcherSymbolJDtable_ptr) = 0;
	virtual void		getNewUplaneMessage(uint8_t** uplane_msg_ptr) = 0;
	virtual EResultT    GetDpdkDriveFronthaulPtr (void* dpdk_eth_driver_fronthaul_api) = 0;
	virtual uint32_t    GetRBIndexFromStitcher() = 0;
	virtual void StitcherProcessNotUL()=0;
	virtual void SetProfiler(CProfileCnt* ul_prof,CProfileCnt* ul_symbol_tick_prof)=0;


};


//Job Descriptor of the Stitcher DE
struct StitcherEngineJobDesc
{
	uint8_t		stitcher_info_table_index_;//double-buffer index of FPGA memory according to Symbol number (should be toggled each next symbol)
};


#endif /* API_MODULE_API_I_STITCHER_ENGINE_API_H_ */
