/******************************************************************
 * dpdk_eth_driver.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_ETH_H
#define DPDK_ETH_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_dpdk_eth_api.h"

const uint32_t MSG_CHUNK = 512; //Size of receive message data chunk
const uint32_t CHECK_SUM_SIZE = 1;
const uint32_t RINGS_SIZE = 4096;

using namespace ai_framework_proj;

class CDPDK_Eth_driver;

/*******************************************************************************************/ /*!
*@class ClassifiedQueue
*@brief The purpose of this class is :
*@brief Implementation for dpdk ethernet MBUF queue. DPDK Ethernet driver contains different Rx and Tx queues.
*@brief The worker threads may access the different queues for put and get data. Only one worker thread may access some queue.
*@brief The driver classifier spread the input data to number queues.
***********************************************************************************************/
class ClassifiedQueue: public IngressClassifiedQueueAPI
{
public:
    // Receive packet (get packet from Rx ring queue if RxThread is running)
      // \param[out] pkt              Will be filled with Pointer to incoming packet.
     //                              AS_INFRA_NULL if no packet received.
     //                              Use rte_pktmbuf_free(pkt) to free the pkt after processing.
     // \return EResultT
     EResultT recv(rte_mbuf*& pkt);

     // Receive burts of packet
    // \param[in] rx_pkts           mbuf pointers array to receive
    // \param[in] count             mbufs count to receive
    // \return Number of received mbufs
    uint16_t recvBurst(rte_mbuf **rx_pkts, uint16_t count);

    //Internal methods
    void Init(CDPDK_Eth_driver *Parent, IModuleControlCallBackAPI *ptr, INGRESS_CLASSIFICATOR_Q_TYPE id)
    {
        callback_ptr_ = ptr;
        ingress_queue_id_ = id;
        this->Parent = Parent;
        QueueId = (uint8_t)id;
    }

    INGRESS_CLASSIFICATOR_Q_TYPE get_queue_id_() { return ingress_queue_id_; }

    ClassifiedQueue()
    {
        callback_ptr_ = NULL;
        ingress_queue_id_ = INGRESS_LAST_CLASSIFIED_QUEUE;
        intermediateRing_ = NULL;
    }

    ~ClassifiedQueue()
    {
        if (intermediateRing_)
            rte_ring_free(intermediateRing_);
    };

    rte_ring *intermediateRing_;

protected:
    IModuleControlCallBackAPI *callback_ptr_;
    INGRESS_CLASSIFICATOR_Q_TYPE ingress_queue_id_;
    CDPDK_Eth_driver *Parent;
    uint8_t QueueId;
};

/*******************************************************************************************/ /*!
*@class EgressQueue
*@brief The purpose of this class is :
*@brief Implementation of the EgressQueueAPI
*@brief
*@brief
***********************************************************************************************/
class EgressQueue: public EgressQueueAPI
{
public:
    // Send packet (put packet to Tx ring queue if TxThread is running)
    // \param[in] pkt               mbuf to send. The pkt is freed by send() for sent packets (if AS_INFRA_RETCODE_ROK)
    // \param[in] free_unsent       Frees unsent mbuf
    // \return EResultT
    EResultT send(rte_mbuf* pkt, bool free_unsent = true);

    // Send burts of packet
    // \param[in] tx_pkts           mbuf pointers array to send. The *tx_pkts is freed by send() for sent packets
    // \param[in] count             mbufs count to send
    // \param[in] free_unsent       Frees unsent mbuf
    // \return Number of sent mbufs
    uint16_t sendBurst(rte_mbuf **tx_pkts, uint16_t count, bool free_unsent = true);

    //Internal methods
    void Init(CDPDK_Eth_driver *Parent, IModuleControlCallBackAPI *ptr, EGRESS_Q_TYPE id)
    {
        callback_ptr_ = ptr;
        egress_queue_id_ = id;
        this->Parent = Parent;
    }

    EGRESS_Q_TYPE get_queue_id_() { return egress_queue_id_; }

    EgressQueue()
    {
        callback_ptr_ = NULL;
        egress_queue_id_ = LAST_EGRESS_QUEUE;
    }

    ~EgressQueue(){};

private:
    IModuleControlCallBackAPI *callback_ptr_;
    EGRESS_Q_TYPE egress_queue_id_;
    CDPDK_Eth_driver *Parent;
    uint8_t QueueId;
};

/******************************************************************************************/ /*!
 *@class CDPDK_Eth_driver
 *@brief This class represents DPDK Ethernet device
 *@brief Support communication using TCP_IP sockets.
 *********************************************************************************************/

class CDPDK_Eth_driver : public DpdkEthDeviceAPI
{
public:
    // Open port by port_id
    // \param[in] port_id       Port id to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
    EResultT open(uint16_t port_id, eth_dev_settings_t *dev_settings = NULL);

    // Open port by MAC
    // \param[in] mac           MAC address to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
    EResultT open(const uint8_t mac[ETHER_ADDR_LEN], eth_dev_settings_t *dev_settings = NULL);

    // Close port
    // \return EResultT
    EResultT close();

    // Check if ETH_LINK UP
    // \return true if link up
	bool isLinkUp();

    // Busy wait till ETH_LINK UP using timeout
    // \param[in] timeout_ms        Time to wait (ms)
    // \param[in] check_period_ms   Check interval (ms)
    // \return true if link up
    bool waitLinkUp(uint32_t timeout_ms = 0, uint32_t check_period_ms = 1);

    // Port id
    uint16_t getPortId();

    // Assign a software classifier to eternet device
    // If software classifier is used, this function should be called before open device
    // \param[in] classifier_api    Pointer to classifier interface class
    // \return EResultT
    EResultT assignSoftwareClassifier(DpdkClassifyAPI *classifier_api, uint16_t classifier_burst_size);

    // Performs reading and classification packets from separate thread
    // \return EResultT
    EResultT readAndClassification();

    //This function returns the pointer to the corresponding classifier MBUF Ingress queue handler.
    //NULL if it does not exist
    IngressClassifiedQueueAPI *GetIngressClassifiedQueue(INGRESS_CLASSIFICATOR_Q_TYPE classified_q_val);

    //This function returns the pointer to the corresponding classifier MBUF Egress queue handler.
    //NULL if it does not exist
	EgressQueueAPI* GetEgressQueue( EGRESS_Q_TYPE egress_q_val );

    // Send packet (put packet to Tx ring queue if TxThread is running)
    // \param[in] queue_id          Tx queue id
    // \param[in] pkt               mbuf to send. The pkt is freed by send() for sent packets (if E_OK)
    // \param[in] free_unsent       Frees unsent mbuf
    // \return EResultT
    EResultT send(uint16_t queue_id, rte_mbuf *pkt, bool free_unsent = true);

    // Send burts of packet
    // \param[in] queue_id          Tx queue id
    // \param[in] tx_pkts           mbuf pointers array to send. The *tx_pkts is freed by send() for sent packets
    // \param[in] count             mbufs count to send
    // \param[in] free_unsent       Frees unsent mbuf
    // \return Number of sent mbufs
    uint16_t sendBurst(uint16_t queue_id, rte_mbuf **tx_pkts, uint16_t count, bool free_unsent = true);

    // Receive packet (get packet from Rx ring queue if RxThread is running)
    // \param[in]  queue_id         Rx queue id
    // \param[out] pkt              Will be filled with Pointer to incoming packet.
    //                              AS_INFRA_NULL if no packet received.
    //                              Use rte_pktmbuf_free(pkt) to free the pkt after processing.
    // \return EResultT
    EResultT recv(uint16_t queue_id, rte_mbuf *&pkt);

    // Send burts of packet
    // \param[in] queue_id          Tx queue id
    // \param[in] tx_pkts           mbuf pointers array to receive
    // \param[in] count             mbufs count to receive
    // \param[in] free_unsent       Frees unsent mbuf
    // \return Number of received mbufs
    uint16_t recvBurst(uint16_t queue_id, rte_mbuf **rx_pkts, uint16_t count);

    //Local methods

    void Reset();

    void Init(IModuleControlCallBackAPI *ptr, uint32_t id)
    {
    	callback_ptr_= ptr;
    	dev_id_ = id;
    }
    CDPDK_Eth_driver()
    {
        Reset();
        callback_ptr_ = NULL;
        dev_id_ = -1;
        classifier_api_ = NULL;
        classifier_pkts_ = NULL;
    }

    bool IsSwClassifier()
    {
        return (classifier_api_ != NULL);
    }

    ~CDPDK_Eth_driver()
    {
    }
    //Fills rx_queues_ and tx_queues_ arrays
    void FillQueues();

private:
    IModuleControlCallBackAPI *callback_ptr_;
    uint32_t dev_id_;
    // Get port id by MAC if exists
    // \return EResultT
    static EResultT getPortIdByMac(const uint8_t mac[ETHER_ADDR_LEN], uint16_t &port_id);
    // Port id
    uint16_t m_port_id;
    // Flag: is object initialized
    bool m_bInited;
    // Ethernet device settings
    eth_dev_settings_t m_settings;
    // mempool in memory to hold the mbufs
    rte_mempool **m_mbuf_pool;

    //array of ingress queue object
    ClassifiedQueue rx_queues_[INGRESS_LAST_CLASSIFIED_QUEUE];
    //array of egress queue object
    EgressQueue tx_queues_[LAST_EGRESS_QUEUE];
    //associated classifier object
    DpdkClassifyAPI *classifier_api_;
    //burst size used during classification
    uint16_t classifier_burst_size_;
    //
    rte_mbuf **classifier_pkts_;

    uint32_t num_packets_sent_;
};

/******************************************************************************************/ /*!
 *@class CCommDriverFactory
 *@brief The purpose of this class is :
 *@brief This class holds the communication drivers.
 *********************************************************************************************/
class CCommDriverFactory : public CBaseModule
{
    /***********************************************************************************/
    //Examples of useful data types (optional)
    CProfileCnt comm_prof_; //Example of profiler counter
    CDPDK_Eth_driver dpdk_eth_driver_[MAX_COMM_DPDK_DRIVERS];
    uint32_t num_ip_drivers_;

public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info);
    EResultT IConfigure(EConfigId id, void *in, void **out);
    EResultT IColdStart();
    CCommDriverFactory()
    {
        num_ip_drivers_ = 0;
    }
};

#endif /* DPDK_ETH_H */
