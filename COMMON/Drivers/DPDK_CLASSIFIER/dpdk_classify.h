/******************************************************************
 * dpdk_classify.h
 * Author: Evgeniy Sokolov
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_CLASSIFY_H
#define DPDK_CLASSIFY_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include <vector>

#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_dpdk_classify_api.h"

/******************************************************************************************//*!
 *@class CDPDK_Eth_driver
 *@brief This class represents DPDK Ethernet device
 *@brief Support communication using TCP_IP sockets.
 *********************************************************************************************/

class CDPDK_Classify : public DpdkClassifyAPI
{
public:
    // Clasiify network packet
    // \param[in] packet       Pointer to mbuf which contains packet
    // \return Classififation result
    virtual uint16_t Classify  (const void *packet);

    // Clasiify network packet
    // \param[in] packet       Pointer to mbuf which contains packet
    // \return E_OK or E_FAIL
    virtual EResultT Configure  (ether_addr *mac);

    //Local methods
    void Reset();

    void Init(IModuleControlCallBackAPI* ptr, uint32_t id)
    {
    	callback_ptr_= ptr;
    	dev_id_ = id;
    }

    CDPDK_Classify()
    {
        Reset();
        callback_ptr_ = NULL;
        dev_id_= -1;
    }

    ~CDPDK_Classify()
    {
        if (acx)
            rte_acl_free(acx);
    }
    
private:
    IModuleControlCallBackAPI* callback_ptr_;
    uint32_t dev_id_;

    //Local private methods
    uint32_t getMacFirst4bytes(struct ether_addr *mac);
    uint32_t getMacLast2bytes(struct ether_addr *mac);
    
    // Assign params
    // \param[in] param     pointer to parameter structure
    // \return EResultT
    EResultT assign_params(const rte_acl_param* param);

    // Assign a rule field definitions
    // \param[in] definitions  pointer to array of rule definitions
    // \param[in] count        count of definitions in array
    // \return EResultT
    EResultT assign_rule_field_definitions(rte_acl_field_def* definitions, uint8_t count);

    // Add a rule
    // \param[in] rule    pointer to rule. Rule structure must be defined by the RTE_ACL_RULE_DEF macro;
    // \return index of rule in the rule list
    int add_rule(void* rule);

    //build method
    // \return EResultT
    EResultT build();

    //DPDK ACL context
    struct rte_acl_ctx* acx = 0;
    //saved ACL config
    rte_acl_param Config;
    //saved acl Rule definitions
    std::vector< uint8_t > RuleDefs;
    //saved ACL rule list
    std::vector< std::vector< uint8_t > > Rules;
    //count of fields in definition
    int count_of_fields = 0;
};

/******************************************************************************************//*!
 *@class CCommDriverFactory
 *@brief The purpose of this class is :
 *@brief This class holds the communication drivers.
 *********************************************************************************************/
#define MAX_CLASSIFY_DRIVERS (4)
class CClassifyFactory : public CBaseModule
{
    /***********************************************************************************/
    //Examples of useful data types (optional)
    CProfileCnt classify_prof_;  //Example of profiler counter
    CDPDK_Classify dpdk_classify_[MAX_CLASSIFY_DRIVERS];
    uint32_t num_classify_drivers_;
     public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info);
    EResultT IConfigure(EConfigId id, void *in, void **out);
    EResultT IColdStart();
    CClassifyFactory()
    {
        num_classify_drivers_ = 0;
    }
};

#endif /* DPDK_CLASSIFY_H */

