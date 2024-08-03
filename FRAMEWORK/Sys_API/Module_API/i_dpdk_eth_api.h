/******************************************************************
 * i_dpdk_eth_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_DPDK_ETH_DEV_API_H_
#define API_MODULE_DPDK_ETH_DEV_API_H_
#include <rte_config.h>
#include <rte_mempool.h>
#include <rte_ethdev.h>
#include "i_dpdk_classify_api.h"

#define BASE_MAC_ADDRESS_FOR_UPLANE_LAYER_WITH_FPGA_START           0x22
#define BASE_MAC_ADDRESS_FOR_UPLANE_LAYER_WITH_FPGA_END             BASE_MAC_ADDRESS_FOR_UPLANE_LAYER_WITH_FPGA_START + RU_MAX_THEORETICAL_NUM_LAYERS-1

#define BASE_MAC_ADDRESS_FOR_DL_CPLANE_LAYER_WITH_FPGA_START        0x42
#define BASE_MAC_ADDRESS_FOR_DL_CPLANE_LAYER_WITH_FPGA_END          BASE_MAC_ADDRESS_FOR_DL_CPLANE_LAYER_WITH_FPGA_START + RU_MAX_THEORETICAL_NUM_LAYERS-1

#define BASE_MAC_ADDRESS_FOR_UL_CPLANE_LAYER_WITH_FPGA_START        0x62
#define BASE_MAC_ADDRESS_FOR_UL_CPLANE_LAYER_WITH_FPGA_END          BASE_MAC_ADDRESS_FOR_UL_CPLANE_LAYER_WITH_FPGA_START + RU_MAX_THEORETICAL_NUM_LAYERS-1

#define BASE_MAC_ADDRESS_FOR_WEIGHT_WITH_FPGA_START                 0x82


#define SOURCE_ADDRESS_OF_WORKER_THREAD      0xf1

#define MAX_COMM_DPDK_DRIVERS (6)	//max number of DPDK Driver instances per each Factory (Online or Offline)

#define DPDK_ETH_MODULE "DPDK_ETH"
#define DPDK_ETH_MODULE_OFFLINE "DPDK_ETH_OFFLINE"


#define RU_CHECK_TEST_END "RU_CHECK_TEST_END"

enum
{
	GENERAL_DRIVER,
	DPAA2_DRIVER
} DPDK_DRIVER;

enum INGRESS_CLASSIFICATOR_Q_TYPE
{
	INGRESS_NON_CLASSIFIED_Q,
	INGRESS_UPLANE_LAYER0_Q,
	INGRESS_UPLANE_LAYER1_Q,
	INGRESS_UPLANE_LAYER2_Q,
	INGRESS_UPLANE_LAYER3_Q,
	INGRESS_UPLANE_LAYER4_Q,
	INGRESS_UPLANE_LAYER5_Q,
	INGRESS_UPLANE_LAYER6_Q,
	INGRESS_UPLANE_LAYER7_Q,

	INGRESS_CPLANE_LAYER0_Q,
	INGRESS_CPLANE_LAYER1_Q,
	INGRESS_CPLANE_LAYER2_Q,
	INGRESS_CPLANE_LAYER3_Q,
	INGRESS_CPLANE_LAYER4_Q,
	INGRESS_CPLANE_LAYER5_Q,
	INGRESS_CPLANE_LAYER6_Q,
	INGRESS_CPLANE_LAYER7_Q,
	INGRESS_LAST_CLASSIFIED_QUEUE

} ;

enum EGRESS_Q_TYPE
{
	EGRESS_UNIFIED_QUEUE,
	LAST_EGRESS_QUEUE
} ;


//Temporary added to this file from the INFRA file dpdkEthDevSettings.hpp
// hash_key for Symmetric RSS (http://galsagie.github.io/2015/02/26/dpdk-tips-1/)
__attribute__((unused)) static uint8_t symmetric_rss_hash_key[40] = { 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A, 0x6D, 0x5A };

static const uint32_t ETH_ADDR_LEN        = 6;            // Length of Ethernet address
// Ethernet device settings
typedef struct st_eth_dev_settings
{
    st_eth_dev_settings()
    {
        // Default settings:
        link_speeds            = ETH_LINK_SPEED_10G;
        hw_rx_queues           = 1;
        hw_tx_queues           = 1;
        hw_rx_queue_size       = 1024;
        hw_tx_queue_size       = 1024;
        max_rx_pkt_len         = ETHER_MAX_LEN;
        num_mbufs              = /*2048*10*/1024 * 4;
        mbuf_data_room_size    = RTE_MBUF_DEFAULT_BUF_SIZE;
        mbuf_cache_size        = 0/*250*/; // <= CONFIG_RTE_MEMPOOL_CACHE_MAX_SIZE
        eth_promiscuous_enable = false;
	
	tx_offloads          = 0;
	rx_offloads          = 0;     
        rss_conf.rss_hf      = 0; // ETH_RSS_IPV4;
        rss_conf.rss_key     = NULL;
        rss_conf.rss_key_len = 0;

        mq_mode_tx = ETH_MQ_TX_NONE;
        mq_mode_rx = ETH_MQ_RX_NONE;

        flow_control.mode	= RTE_FC_NONE; // full flow-control by default
    }

    uint32_t      link_speeds;         // Bitmap of ETH_LINK_SPEED_XXX of speeds
    uint16_t      hw_rx_queues;        // The number of Rx queues to set up for the Ethernet device.
    uint16_t      hw_tx_queues;        // The number of Tx queues to set up for the Ethernet device.
    uint16_t      hw_rx_queue_size;    // Size of Rx ring queues (the number of receive descriptors stored)
    uint16_t      hw_tx_queue_size;    // Size of Tx ring queues (the number of transmit descriptors stored)
    uint32_t      max_rx_pkt_len;      // Only used if jumbo_frame enabled Max Rx frame length. Enables jumbo_frames if > ETHER_MAX_LEN
    uint32_t      num_mbufs;           // The number of elements in the mbuf pool. The optimum size (in terms of memory usage) for a mempool is when n = (2^q - 1).
    uint16_t      mbuf_data_room_size; // Size of data buffer in each mbuf, including RTE_PKTMBUF_HEADROOM
    uint32_t      mbuf_cache_size;     // Size of the per-core object cache. See rte_mempool_create() for details.
    uint64_t      tx_offloads;         // Pre -Port Tx offloads set using different flags
    uint64_t      rx_offloads;         // Pre -Port Rx offloads set using different flags
    bool eth_promiscuous_enable;   // Enable promiscuous mode

    rte_eth_rx_mq_mode mq_mode_rx; // Rx multi-queue packet distribution mode
    rte_eth_tx_mq_mode mq_mode_tx; // TX multi-queues mode
    rte_eth_rss_conf   rss_conf;   // RSS config
    rte_eth_fc_conf    flow_control; // flow control settings
} eth_dev_settings_t;

/*******************************************************************************************//*!
*@class IngressClassifiedQueueAPI
*@brief The purpose of this class is :
*@brief Interface for dpdk ethernet MBUF queue. DPDK Ethernet driver is responsible for
*@brief input data classification and put data
*@brief on different MBUF queues
***********************************************************************************************/
struct IngressClassifiedQueueAPI
{

    // Receive packet (get packet from Rx ring queue if RxThread is running)
     // \param[out] pkt              Will be filled with Pointer to incoming packet.
    //                              AS_INFRA_NULL if no packet received.
    //                              Use rte_pktmbuf_free(pkt) to free the pkt after processing.
    // \return EResultT
    virtual EResultT recv(rte_mbuf*& pkt) =0;

    // Receive burts of packet
    // \param[in] rx_pkts           mbuf pointers array to receive
    // \param[in] count             mbufs count to receive
    // \return Number of received mbufs
    virtual uint16_t recvBurst(rte_mbuf** rx_pkts, uint16_t count) =0;



} ;


/*******************************************************************************************//*!
*@class EgressQueueAPI
*@brief The purpose of this class is:
*@brief
*@brief
***********************************************************************************************/
struct EgressQueueAPI
{
    // Send packet (put packet to Tx ring queue if TxThread is running)
    // \param[in] pkt               mbuf to send. The pkt is freed by send() for sent packets (if AS_INFRA_RETCODE_ROK)
    // \param[in] free_unsent       Frees unsent mbuf
    // \return EResultT (OK/FAIL)
    virtual EResultT send(rte_mbuf* pkt, bool free_unsent = true) =0;

    // Send burts of packet
    // \param[in] tx_pkts           mbuf pointers array to send. The *tx_pkts is freed by send() for sent packets
    // \param[in] count             mbufs count to send
    // \param[in] free_unsent       Frees unsent mbuf
    // \return Number of sent mbufs
    virtual uint16_t sendBurst(rte_mbuf** tx_pkts, uint16_t count, bool free_unsent = true) =0;


} ;


/*******************************************************************************************//*!
*@class DpdKEthDeviceAPI
*@brief The purpose of this class is :
*@brief Interface for DPDK ethernet device. This device supports number MBUF queues separated
*@brief classification tags
***********************************************************************************************/
struct DpdkEthDeviceAPI
{
    // Open port by port_id
    // \param[in] port_id       Port id to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
	virtual EResultT open(uint16_t port_id, eth_dev_settings_t* dev_settings = NULL)= 0;

    // Open port by MAC
    // \param[in] mac           MAC address to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
	virtual EResultT open(const uint8_t mac[ETH_ADDR_LEN], eth_dev_settings_t* dev_settings = NULL)= 0;

    // Close port
    // \return EResultT
	virtual EResultT close()= 0;



    // Check if ETH_LINK UP
    // \return true if link up
	virtual bool isLinkUp()= 0;

    // Busy wait till ETH_LINK UP using timeout
    // \param[in] timeout_ms        Time to wait (ms)
    // \param[in] check_period_ms   Check interval (ms)
    // \return true if link up
    virtual bool waitLinkUp(uint32_t timeout_ms = 0, uint32_t check_period_ms = 1)= 0;

    // Port id
    virtual uint16_t getPortId()= 0;

    // Assign a software classifier to eternet device
    // If software classifier is used, this function should be called before open device
    // \param[in] classifier_api    Pointer to classifier interface class
    // \return EResultT
    virtual EResultT assignSoftwareClassifier(DpdkClassifyAPI *classifier_api, uint16_t classifier_burst_size) = 0;

    // Performs reading and classification packets from separate thread
    // \return EResultT
    virtual EResultT readAndClassification() = 0;

    //This function returns the pointer to the corresponding classifier MBUF Ingress queue handler.
	//NULL if it does not exist
    virtual IngressClassifiedQueueAPI *GetIngressClassifiedQueue( INGRESS_CLASSIFICATOR_Q_TYPE classified_q_val ) = 0;

	//This function returns the pointer to the corresponding classifier MBUF Egress queue handler.
    //NULL if it does not exist
	virtual EgressQueueAPI *GetEgressQueue( EGRESS_Q_TYPE egress_q_val ) = 0;
};

/*******************************************************************************************/ /*!
     *@class DpdKEthMacAPI
     *@brief The purpose of this class is :
     *@brief Interface for DPDK ethernet mac. This module operates with Ethernet headers of
     *@brief incoming/outgoing packets
***********************************************************************************************/
struct DpdkEthMacAPI
{
public:
  // Set current srcMac of CDPDK_Eth_Mac object
  // E_OK if src_mac is not NULL. E_FAIL otherwise.
  virtual EResultT setSrcMac(const char *src_mac) = 0;
  virtual EResultT setSrcMac(const struct ether_addr *src_mac) = 0;

  // This function returns current srcMac of CDPDK_Eth_Mac object.
  // Always successfull.
  virtual struct ether_addr *getSrcMac() = 0;

  // This function cuts Ethernet header of incoming packet.
  // Returns E_OK in case it is successfully done, E_FAIL otherwise.
  virtual EResultT cutEthHdr(struct rte_mbuf *pkt, int vlan = 0) = 0;

  // This group of functions adds ethernet header to certain packet.
  // They return E_OK in case it is successfully done, E_FAIL otherwise.
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr) = 0;
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, struct ether_hdr *ethhdr, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt) = 0;
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type) = 0;
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, const char *src_mac, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt) = 0;
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type) = 0;
  virtual EResultT addEthHdr(struct rte_mbuf *pkt, const char *dst_mac, uint16_t eth_type, struct rte_mempool *header_mp, struct rte_mbuf **out_pkt) = 0;
};

#endif /* API_MODULE_DPDK_ETH_DEV_API_H_ */
