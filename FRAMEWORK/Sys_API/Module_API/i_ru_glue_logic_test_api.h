/******************************************************************
 * i_ru_glue_logic_test_api.h
 * Author:
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef I_RU_TEST_GLUE_LOGIC_API_H_
#define I_RU_TEST_GLUE_LOGIC_API_H_
#include "i_api_common.h"
#include "i_main_ru_handler_api.h"
#include "i_dpdk_eth_api.h"

//Constants for test!!
#define MAX_NUMBER_OF_SLOTS_IN_TEST 30
#define MAX_NUMBER_OF_SLOTS_SYMB_ANTS_GROUP MAX_NUMBER_OF_SLOTS_IN_TEST*MAX_NUM_OFDM_PER_SLOT*ANT_GROUPS_NUMBER




/*******************************************************************************************//*!
*@class CEthDriver_api
*@brief The purpose of this class is :
*@brief API for Ethernet driver
***********************************************************************************************/


struct CTestGlueLogic_api
{
    virtual void DLTestRunPrepare(char *dir) = 0;
    virtual void GetCfgData(RuWaveFormStructIO *wavform_struct_ptr)= 0;
    virtual void GetDLMessage(char **DL_config, char **TX_req) = 0;
    //virtual EResultT ResCompare() = 0;
};


#endif /* I_RU_TEST_GLUE_LOGIC_API_H_ */
