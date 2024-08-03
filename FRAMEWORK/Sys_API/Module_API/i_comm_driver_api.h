/******************************************************************
 * i_comm_driver_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef API_MODULE_API_I_TEST_GLUE_LOGIC_API_H_
#define API_MODULE_API_I_TEST_GLUE_LOGIC_API_H_
#include "i_api_common.h"
#include "i_handler_api.h"

enum EMediaAddrlType
{
    e_IPV4,
    e_IPV6,
    e_ETHERNET,
    e_MAX_PROTOCOL
};

enum ENodeType
{
    e_PHY,
    e_MAC
};
struct NetAddress
{
    ENodeType  node_type;
    EMediaAddrlType comm_protocol;
    uint8_t     dest_addr[256];
    uint32_t srv_tx_port;
    uint32_t srv_rx_port;

    void Reset()
    {
        memset((char*)this, 0, sizeof(NetAddress));
        comm_protocol= e_MAX_PROTOCOL;
    }
};
/*******************************************************************************************//*!
*@class CTestGlueLogic_api
*@brief The purpose of this class is :
*@brief Testing of glue logic
***********************************************************************************************/
struct CCommDriver_api
{
    virtual EResultT OpenConnection(NetAddress *dest)= 0;
    virtual EResultT SendMessage(char* buf, uint32_t size) = 0;
    virtual EResultT RecvMessage(char* buf, int32_t *size) = 0;
    virtual EResultT CloseConnection() = 0;

};

#endif /* API_MODULE_API_I_TEST_GLUE_LOGIC_API_H_ */
