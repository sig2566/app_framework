/******************************************************************
 * dpdk_eth_mac.cc
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "i_dpdk_buffer_manager.h"
#include "dpdk_eth_mac.h"

// API methods implementation

/******************************************************************************************//*!
 *@function setSrcMac()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::setSrcMac(const char *srcMac)
{
    if (!srcMac) {
        return E_FAIL;
    }
    memcpy(&this->src_mac, srcMac, sizeof(struct ether_addr));
    return E_OK;
}
   
/******************************************************************************************//*!
 *@function setSrcMac()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::setSrcMac(const struct ether_addr *srcMac)
{
    if (!srcMac) {
        return E_FAIL;
    }

    memcpy(&this->src_mac, srcMac, sizeof(struct ether_addr));
    return E_OK;
}

/******************************************************************************************//*!
 *@function cutEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
struct ether_addr *CDPDK_Eth_Mac::getSrcMac()
{
    return &src_mac;
}

/******************************************************************************************//*!
 *@function cutEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::cutEthHdr(struct rte_mbuf *pkt, int vlan)
{
    if( rte_pktmbuf_adj(pkt, vlan ? (ETHER_HDR_LEN + 4) : (ETHER_HDR_LEN)) == NULL ) {
        return E_FAIL;
    } 
     
	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr)
{
    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(pkt, sizeof(struct ether_hdr));   
    if ( hdr == NULL ) {
        return E_FAIL;   
    }

    memcpy(hdr, ethhdr, sizeof(struct ether_hdr));

	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt)
{
    struct rte_mbuf *header = rte_pktmbuf_alloc(header_mp);
    if (!header) {
        return E_FAIL;
    }

    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(header, sizeof(struct ether_hdr));
    if ( hdr == NULL ) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    memcpy(hdr, ethhdr, sizeof(struct ether_hdr));
    if (rte_pktmbuf_chain(header, pkt) != 0 ) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    *out_pkt = header;

	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type)
{
    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(pkt, sizeof(struct ether_hdr));   
    if ( hdr == NULL ) {
        return E_FAIL;   
    }

    ether_addr_copy((struct ether_addr * const)dst_mac, &hdr->d_addr);
    ether_addr_copy((struct ether_addr * const)src_mac, &hdr->s_addr);

    hdr->ether_type = eth_type;

	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt)
{
    struct rte_mbuf *header = rte_pktmbuf_alloc(header_mp);
    if (!header) {
        return E_FAIL;
    }

    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(header, sizeof(struct ether_hdr));
    if ( hdr == NULL) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    ether_addr_copy((struct ether_addr * const)dst_mac, &hdr->d_addr);
    ether_addr_copy((struct ether_addr * const)src_mac, &hdr->s_addr);

    hdr->ether_type = eth_type;
    if (rte_pktmbuf_chain(header, pkt) != 0 ) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    *out_pkt = header;
	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type)
{
    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(pkt, sizeof(struct ether_hdr));   
    if ( hdr == NULL ) {
        return E_FAIL;   
    }

    ether_addr_copy((struct ether_addr * const)dst_mac, &hdr->d_addr);
    ether_addr_copy(&src_mac, &hdr->s_addr);

    hdr->ether_type = eth_type;

	return E_OK;
}

/******************************************************************************************//*!
 *@function addEthHdr()
 *@brief The purpose of this function is :
 *@brief Create and initiate required mempools according to "settings" structure
 *@brief
 *@brief
 *@brief
 *@brief returns: EResultT (OK/FAIL)
 *********************************************************************************************/
EResultT CDPDK_Eth_Mac::addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt)
{
    struct rte_mbuf *header = rte_pktmbuf_alloc(header_mp);
    if (!header) {
        return E_FAIL;
    }

    struct ether_hdr *hdr = (struct ether_hdr *)rte_pktmbuf_prepend(header, sizeof(struct ether_hdr));
    if ( hdr == NULL) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    ether_addr_copy((struct ether_addr * const)dst_mac, &hdr->d_addr);
    ether_addr_copy(&src_mac, &hdr->s_addr);

    hdr->ether_type = eth_type;
    if (rte_pktmbuf_chain(header, pkt) != 0 ) {
        rte_pktmbuf_free(header);
        return E_FAIL;
    }

    *out_pkt = header;
	return E_OK;
}

//
EResultT CDpdkEthMac::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
{

	CBaseModule::IInit(callback_ptr, target_api, init_info);
    LOG(E_MEDIUM,  "init passed");

    return E_OK;
}

EResultT CDpdkEthMac::IColdStart()
{
		return E_OK;
}

EResultT CDpdkEthMac::IConfigure(EConfigId id, void *in, void **out)
{
    num_eth_mac_++;
    ASSERT (num_eth_mac_ < MAX_ETH_MAC);
    *out =  (void *)(static_cast<DpdkEthMacAPI*>(&dpdk_eth_mac_[num_eth_mac_]));
	return E_OK;
}

class CDpdkEthMac     *g_module_ptr = NULL;

//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new CDpdkEthMac();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}

