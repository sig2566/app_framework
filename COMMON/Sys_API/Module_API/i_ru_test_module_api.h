/******************************************************************
 * i_ru_test_module_api.h
 *
 * Author:	Y. Keshet
 *
 * Copyright (c) 2020 - 2022 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef I_RU_TEST_MODULE_API_H_
#define I_RU_TEST_MODULE_API_H_


#include "i_api_common.h"
#include "i_dpdk_eth_api.h"


//	This is a header that comes before an Ethernet frame
//	(in offline mode only)
typedef struct PACK_STRUCT
{
	timespec timestamp;			//	Time Stamp
	uint32_t data_length;		//	Length of all the data that is after the OfflineDataHeader
								//	(for example: include Ethernet header, ORAN packet and FCS)
}OfflineDataHeader;


//	This enum describes the device type with which
//	the DPDK Driver communicates:
enum DpdkDeviceType
{
	kFrontHaul = 0,
	kFPGA
};


//	This enum describes the test module with which
//	the DPDK Driver communicates:
enum DpdkTestModuleType
{
	kGlueLogicTest = 0,
	kEmulatorLoader,
	kFpgaSimulator
};


/*******************************************************************************************//*!
*@class TestModuleAPI
*@brief The purpose of this class is :
*@brief API for offline test modules (GL Test, FPGA simulator)
***********************************************************************************************/
struct TestModuleAPI
{
	   virtual EResultT GetPacket(INGRESS_CLASSIFICATOR_Q_TYPE queue_type, uint8_t*& retrieved_data, DpdkTestModuleType* test_module_type_ptr) = 0;
 	   virtual EResultT SendPacket(EGRESS_Q_TYPE queue_type, uint8_t* data_to_send_ptr) = 0;

};



#endif	//	I_RU_TEST_MODULE_API_H_
