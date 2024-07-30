/******************************************************************
 * i_ru_test_glue_logic_no_oran_api.h
 * Author:
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef I_RU_TEST_GLUE_LOGIC_NO_ORAN_API_H_
#define I_RU_TEST_GLUE_LOGIC_NO_ORAN_API_H_
#include "i_api_common.h"
#include "i_handler_api.h"



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


#endif /* I_RU_TEST_GLUE_LOGIC_NO_ORAN_API_H_ */
