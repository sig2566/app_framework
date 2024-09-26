/******************************************************************
 * tcp_ip_driver.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include <unistd.h>

#include <rte_acl.h>
#include <rte_config.h>
#include "dpdk_classify.h"
#include "i_dpdk_eth_api.h"
/////////////////////////////////////////////////////////////

/*
 * Byte offset of fields in message header
 */
#define BYTE_OFFSET_DST_MAC_ADDR_B0  (0)
#define BYTE_OFFSET_DST_MAC_ADDR_B4  (4)
#define BYTE_OFFSET_ECPRI_MSG        (15)
#define BYTE_OFSET_RU_PORT_ID        (19)

#define ALIGN_4_BYTES(x)  (x & 0xFFFFFFFC)



///////// ACL SETTINGS STRUCTURES ///////////////////////////
//rule field definitions
struct rte_acl_field_def ecpri_rule_field_defs[] =
{
    // first input field - always one byte size.
    // It is stub
    {
        .type        = RTE_ACL_FIELD_TYPE_BITMASK,
        .size        = sizeof(uint8_t),
        .field_index = 0,
        .input_index = 0,
        .offset      = 0
    },

    // All next fields must be 4 bytes!

    // first part of Destination MAC - 4 consecutive bytes.
    {
        .type        = RTE_ACL_FIELD_TYPE_BITMASK,
        .size        = sizeof(uint32_t),
        .field_index = 1,
        .input_index = 1,
        .offset      = ALIGN_4_BYTES(BYTE_OFFSET_DST_MAC_ADDR_B0)
    },
    // second part of Destination MAC - 4 consecutive bytes. used first 2 bytes
    {
        .type        = RTE_ACL_FIELD_TYPE_BITMASK,
        .size        = sizeof(uint32_t),
        .field_index = 2,
        .input_index = 2,
        .offset      = ALIGN_4_BYTES(BYTE_OFFSET_DST_MAC_ADDR_B4)
    },

    // ecpriMessage
    {
        .type        = RTE_ACL_FIELD_TYPE_BITMASK,
        .size        = sizeof(uint32_t),
        .field_index = 3,
        .input_index = 3,
        .offset      = ALIGN_4_BYTES(BYTE_OFFSET_ECPRI_MSG)
    },

    // RU_Port_Id
    {
        .type        = RTE_ACL_FIELD_TYPE_BITMASK,
        .size        = sizeof(uint32_t),
        .field_index = 4,
        .input_index = 4,
        .offset      = ALIGN_4_BYTES(BYTE_OFSET_RU_PORT_ID)
    },
};


/* AC context creation parameters. */
struct rte_acl_param ecpri_acl_param = {
    .name         = "ecpri_ACL",
    .socket_id    = SOCKET_ID_ANY,
    .rule_size    = RTE_ACL_RULE_SZ(RTE_DIM(ecpri_rule_field_defs)), // number of fields per rule.
    .max_rule_num = 20,                                              // maximum number of rules in the AC context.
};

RTE_ACL_RULE_DEF(acl_rules, RTE_DIM(ecpri_rule_field_defs));

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Clasiify network packet
// \param[in] packet       Pointer to mbuf which contains packet
// \return Classififation result
uint16_t CDPDK_Classify::Classify  (const void *packet)
{
    uint32_t result;
    if (rte_acl_classify(acx, (const uint8_t**)&packet, &result, 1, 1))
    {
        // We come here when arguements are not right
        ASSERT(0);
        return 0;
    }
    return result;
}


// Clasiify network packet
// \param[in] packet       Pointer to mbuf which contains packet
// \return E_OK or E_FAIL
EResultT CDPDK_Classify::Configure  (ether_addr *mac)
{
    //Assign parameters
    if (assign_params(&ecpri_acl_param) != E_OK)
    {
        printf("Error assign ACL params\n");
        return E_FAIL;
    }

    // Assign a rule definition
    if (assign_rule_field_definitions(ecpri_rule_field_defs, RTE_DIM(ecpri_rule_field_defs)) != E_OK)
    {
        printf("Error assign ACL rule definitions\n");
        return E_FAIL;
    }

    //adding rules
    struct acl_rules acl_rule = {};

    acl_rule.data.category_mask      = 1;
    acl_rule.data.priority           = 1;

    // Stub
    acl_rule.field[0].mask_range.u8  = 0;
    acl_rule.field[0].value.u8       = 0;

    // Destination MAC
    acl_rule.field[1].mask_range.u32 = 0xFFFFFFFF;
    acl_rule.field[1].value.u32      = getMacFirst4bytes(mac);
    acl_rule.field[2].mask_range.u32 = 0xFFFF0000;
    acl_rule.field[2].value.u32      = getMacLast2bytes(mac);

    // ecpriMessage
    acl_rule.field[3].mask_range.u32 = 0xFF;
    // RU_Port_Id
    acl_rule.field[4].mask_range.u8  = 0x3F;

    // UPLANE queues
    acl_rule.field[3].value.u8       = 0; // ecpriMessage

    // Add 8 rules for u plane Queues
    for (int i = 0; i <= 7; i++)
    {
        acl_rule.field[4].value.u8       = i; // RU_Port_Id
        acl_rule.data.userdata           = INGRESS_UPLANE_LAYER0_Q + i;
        int rule_id = add_rule(&acl_rule);
    }

   // Add 8 rules for the Cplane Queues
   // CPLANE queues
   acl_rule.field[3].value.u8       = 2; // ecpriMessage
   for (int i = 0; i <= 7; i++)
   {
      acl_rule.field[4].value.u8       = i; // RU_Port_Id
      acl_rule.data.userdata           = INGRESS_CPLANE_LAYER0_Q + i;
      int rule_id = add_rule(&acl_rule);
   }
   
   // build rules
   if (build() != E_OK)
   {
	printf("Error build acl\n");
        return E_FAIL;
   }
   
   return E_OK;
}

//Local methods
void CDPDK_Classify::Reset () 
{

}

uint32_t CDPDK_Classify::getMacFirst4bytes(struct ether_addr *mac)
{
    uint32_t rez = 0;
    uint8_t *addr = mac->addr_bytes;
    rez = addr[0] << 24 | addr[1] << 16 | addr[2] << 8 | addr[3];
    return rez;
}

uint32_t CDPDK_Classify::getMacLast2bytes(struct ether_addr *mac)
{
    uint32_t rez = 0;
    uint8_t *addr = mac->addr_bytes;
    rez = addr[4] << 24 | addr[5] << 16;
    return rez;
}

// Assign params
// \param[in] param     pointer to parameter structure
// \return EResultT

EResultT CDPDK_Classify::assign_params(const rte_acl_param* param)
{
    memcpy(&Config, param, sizeof(rte_acl_param));
    return E_OK;
}

// Assign a rule field definitions
// \param[in] definitions  pointer to array of rule definitions
// \param[in] count        count of definitions in array
// \return EResultT

EResultT CDPDK_Classify::assign_rule_field_definitions(rte_acl_field_def*
                                                                      definitions,
                                                                  uint8_t count)
{
    count_of_fields = count;
    size_t DefSize  = sizeof(struct rte_acl_field_def) * count;
    RuleDefs.resize(DefSize);
    memcpy(&RuleDefs[0], definitions, DefSize);
    return E_OK;
}

// Add a rule
// \param[in] rule    pointer to rule. Rule structure must be defined by the RTE_ACL_RULE_DEF macro;
// \return index of rule in the rule list
int CDPDK_Classify::add_rule(void* rule)
{
    size_t                 SizeOfRule = sizeof(rte_acl_rule_data) + count_of_fields * sizeof(rte_acl_field);
    std::vector< uint8_t > RuleCopy;
    RuleCopy.resize(SizeOfRule);
    memcpy(&RuleCopy[0], rule, SizeOfRule);
    int Index = 0;

    for (std::vector< std::vector< uint8_t > >::iterator It =
                                                             Rules.begin();
         It != Rules.end(); ++It, ++Index)
    {
        if (It->size() == 0)
        {
            *It = RuleCopy;
            return Index;
        }
    }
    Rules.push_back(RuleCopy);
    return Rules.size() - 1;
}

//build method
// \return EResultT
EResultT CDPDK_Classify::build()
{
    if (acx)
        rte_acl_free(acx);

    /* create an empty AC context  */
    if ((acx = rte_acl_create(&Config)) == 0)
    {
	LOG(E_HIGH, "error creating acl\n");
        return E_FAIL;
    }

    /* add rules to the context */
    uint32_t AllCategoryMask = 0;

    for (std::vector< std::vector< uint8_t > >::iterator It =
             Rules.begin();
         It != Rules.end(); ++It)
    {
        if (It->size() != 0)
        {
            rte_acl_rule* RulePtr = (rte_acl_rule*)(&(*It->begin()));
            if (rte_acl_add_rules(acx, RulePtr, 1))
            {
                LOG(E_HIGH, "error adding rules to acl\n");
                return E_FAIL;
            }
            AllCategoryMask |= ((rte_acl_rule_data*)RulePtr)->category_mask;
        }
    }

    /*finding count of categories*/
    int num_categories = 0;
    while (AllCategoryMask != 0)
    {
        AllCategoryMask >>= 1;
        num_categories++;
    }
    LOG(E_MEDIUM, "Categories mask = %X, count of categories = %d\n", AllCategoryMask, num_categories);

    /* prepare AC build config. */
    struct rte_acl_config cfg;
    cfg.num_categories = num_categories;
    cfg.num_fields     = count_of_fields;
    memcpy(cfg.defs, &RuleDefs[0], RuleDefs.size());

    /* build the runtime structures for added rules, with 2 categories. */
    if (rte_acl_build(acx, &cfg) != 0)
    {
        LOG(E_HIGH, "error building rules\n");
        return E_FAIL;
    }
    return E_OK;
}

//
EResultT CClassifyFactory::IInit (IModuleControlCallBackAPI *callback_ptr, ITarget * target_api, const char *init_info)
{
    CBaseModule::IInit (callback_ptr, target_api);
    //Example of initialization of the profiler counter
    char prof_name[] = "CLASSIFY";
    classify_prof_.Init (prof_name, callback_ptr, init_info);	//INIT Profiler counter
    LOG (E_MEDIUM, "init passed");
    return E_OK;
}

EResultT CClassifyFactory::IColdStart ()
{
    return E_OK;
}

EResultT CClassifyFactory::IConfigure (EConfigId id, void *in, void **out)
{
    char str[]= "Call passed";
    callback_ptr_->ILogData(E_LOW, str);
    uint8_t BurstSize = 32;
    
    dpdk_classify_[num_classify_drivers_].Init (callback_ptr_, num_classify_drivers_);
    *out =  (void *) (static_cast < DpdkClassifyAPI * >(&(dpdk_classify_[num_classify_drivers_])));
    ++num_classify_drivers_;
    ASSERT (num_classify_drivers_ < MAX_CLASSIFY_DRIVERS);
    return E_OK;
}

class CClassifyFactory * g_module_ptr = NULL;

//API function to connect with ITarget API
  extern
  "C"
  uint32_t
IGetConnectAPI (void **target_ptr)
{
  g_module_ptr = new CClassifyFactory ();
  *target_ptr = static_cast < IModuleControlAPI * >(g_module_ptr);
  return 0;
}



