/******************************************************************
 * i_eth_driver_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef TEST_GLUE_LOGIC_API_H_
#define TEST_GLUE_LOGIC_API_H_
#include "i_api_common.h"
#include "i_handler_api.h"

//for uplink for GL test only!!!
#define UL_SYMBOL_DATA_IN    "UL_SYMBOL_DATA_IN"

/*******************************************************************************************//*!
*@class CTestGlueLogic_api
*@brief The purpose of this class is :
*@brief API for Ethernet driver
***********************************************************************************************/
struct CTestGlueLogic_api
{
    virtual void DLTestRunPrepare(char *dir) = 0;
    virtual void GetCfgData(WaveFormStructIO *wavform_struct_ptr)= 0;
    virtual void GetDLMessage(char **DL_config, char **TX_req) = 0;
    virtual EResultT ResCompare(int index_slot, int index_TB, struct DL_HandlerIO* dl_handler_ptr) = 0;
};

#endif /* TEST_GLUE_LOGIC_API_H_ */
