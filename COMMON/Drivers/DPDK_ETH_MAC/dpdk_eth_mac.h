/******************************************************************
 * dpdk_eth_mac.h
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_BUFFER_MANAGER_OFFLINE_H
#define DPDK_BUFFER_MANAGER_OFFLINE_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_dpdk_buffer_manager.h"
#include "i_dpdk_eth_api.h"

using namespace ai_framework_proj;

/******************************************************************************************//*!
 *@class CDPDK_Eth_Mac
 *@brief This class represents DPDK Eth Mac
 *@brief For DPDK Eth Mac
 *********************************************************************************************/
class CDPDK_Eth_Mac : public DpdkEthMacAPI
{
protected:
    struct ether_addr src_mac;   
public:
    
    EResultT setSrcMac(const char *src_mac);
    EResultT setSrcMac(const struct ether_addr *src_mac);
    struct ether_addr *getSrcMac();

    EResultT cutEthHdr(struct rte_mbuf *pkt, int vlan = 0);

    EResultT addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr);
    EResultT addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt);
    EResultT addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type);
    EResultT addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt);
    EResultT addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type);
    EResultT addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt);

    CDPDK_Eth_Mac()
    {
        memset(&src_mac, 0, sizeof(struct ether_addr));
    }
};

/******************************************************************************************//*!
 *@class CDpdkEthMac
 *@brief The purpose of this class is :
 *@brief This class holds the DPDK Eth Mac Factory.
 *********************************************************************************************/
#define MAX_ETH_MAC (4)
class CDpdkEthMac : public CBaseModule
{
    /***********************************************************************************/
    //Examples of useful data types (optional)
    CDPDK_Eth_Mac dpdk_eth_mac_[MAX_ETH_MAC];
    uint32_t num_eth_mac_;
 public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api);
    EResultT IConfigure(uint32_t id, void *in, void **out);
    EResultT IColdStart();
    CDpdkEthMac()
    {
    }
    ~CDpdkEthMac()
    {
    }
};



#endif /* DPDK_BUFFER_MANAGER_OFFLINE_H */
