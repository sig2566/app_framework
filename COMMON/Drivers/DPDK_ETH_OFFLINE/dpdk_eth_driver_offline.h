/******************************************************************
 * dpdk_eth_driver.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef DPDK_ETH_H
#define DPDK_ETH_H

#include <i_build_packets_api.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_dpdk_eth_api.h"
#include "i_dpdk_buffer_manager.h"
#include "i_ru_test_module_api.h"
#include "i_ant_group_pool_api.h"
#include "i_emulator_loader_api.h"
#include <pcap.h>
#include "i_pcap_parser_api.h"
#include "i_split_api.h"

const uint32_t MSG_CHUNK = 512; //Size of receive message data chunk
const uint32_t CHECK_SUM_SIZE = 1;


using namespace ai_framework_proj;

#define MAX_INGRESS_QUEUE_SIZE	INGRESS_LAST_CLASSIFIED_QUEUE	//	Maximal size of Ingress queue
#define MAX_EGRESS_QUEUE_SIZE	LAST_EGRESS_QUEUE				//	Maximal size of Egress queue


//=========================== DPDK Offline Eth Driver DPDK Buffer Pools Definitions =========================================== //

// ============== COMMON DEFINITIONS FOR ALL MEMPOOLS =====================
#define TOTAL_NUMBER_MEMPOOLS_DPDK_OFFLINE_DRV		(1) //for Input packets

// cache_size:
// If cache_size is non-zero, the rte_mempool library will try to limit the accesses to the common lockless pool,
// by maintaining a per-lcore object cache.
// This argument MUST be lower or equal to CONFIG_RTE_MEMPOOL_CACHE_MAX_SIZE and n / 1.5 (n - number of buffers in the pool).
// It is advised to choose cache_size to have "n modulo cache_size == 0": if this is not the case,
// some elements will always stay in the pool and will never be used.
// The access to the per-lcore table is of course faster than the multi-producer/consumer pool.
// The cache can be disabled if the cache_size argument is set to 0; it can be useful to avoid losing objects in cache.
#define	MEMPOOL_CACHE_SIZE_DPDK_OFFLINE_DRV			(0)
#define	MEMPOOL_SOCKET_ID_DPDK_OFFLINE_DRV			(0) // we have single socket system at the moment
#define POOL_OPS_NAME_DPDK_OFFLINE_DRV				"stack" // - for using stack memory - cannot be used for DPAA2 sending !!!!

// ============ Ingress Mempools Definitions ==========================
// The number of elements in the mempool.
// The optimum size (in terms of memory usage) for a mempool is when n is a power of two minus one: n = (2^q - 1).
// We multiply the amount by INGRESS_LAST_CLASSIFIED_QUEUE to match all ingress queues needs
#define	MAX_RX_MBUF_IN_POOL							(1023)   // (*INGRESS_LAST_CLASSIFIED_QUEUE)
															// should be enough for all input packets for all Layers
#define INPUT_PACKETS_MAX_SIZE						(DPDK_UPLANE_MTU_SIZE) // !!! this value is enough for receive from FH, but cannot be not enough for receive from FPGA (symbol length with full BW)

#define TOTAL_NUMBER_MEMPOOLS_DPDK_OFFLINE_DRV		(1) //for Input packets
#define	MEMPOOL_CACHE_SIZE_DPDK_OFFLINE_DRV			(0)
#define	MEMPOOL_SOCKET_ID_DPDK_OFFLINE_DRV			(0) // we have single socket system at the moment
#define POOL_OPS_NAME_DPDK_OFFLINE_DRV				"stack"

// Input packets Mempool Definitions
#define	INPUT_PACKETS_MEMPOOL_NAME					"IN_PACK_MEMPOOL_DEV_%d"
#define	INPUT_PACKETS_NUM_BUFFERS					(MAX_RX_MBUF_IN_POOL)
//#define	INPUT_PACKETS_BUFF_SIZE						(4*INPUT_PACKETS_MAX_SIZE) // mempool single buffer size - ask Yotam why we need *2 factor ???
#define	INPUT_PACKETS_BUFF_SIZE						(4*INPUT_PACKETS_MAX_SIZE) // mempool single buffer size - 4* factor is needed for c-plane of 36000 in 32 Ants - the number should be factor of NUM_OF_RU_ANTENNAS
#define	INPUT_PACKETS_PRIVATE_DATA_SIZE				(0)


// ========== Egress Mempools Definitions ==========================

#define	MAX_TX_MBUF_IN_POOL				(5000) // ??????? ME - what it's for ???????????


// ============== ring buffer size for saving preallocated mbufs ==========
const uint32_t INPUT_PREALLOC_RING_SIZE = INPUT_PACKETS_NUM_BUFFERS/INGRESS_LAST_CLASSIFIED_QUEUE;
//============================================================================================================================== //

enum DpdkDriverClockMode
{
	kRealTimeClock = 0,
	kNonRealTimeClock,
	kSystemTimeClock,
	kUndefinedTimeClock
};

/*******************************************************************************************//*!
*@class DpdkEthQueue
*@brief The purpose of this class is :
*@brief General implementation for DPDK Ethernet MBUF queue. Different types of queues (Ingress
*@brief and Egress) are derived from this class.
*@brief It includes common methods and members to all different queues.
***********************************************************************************************/
class DpdkEthQueue
{
public:
	DpdkEthQueue();
	~DpdkEthQueue();
	void InitQueue(IModuleControlCallBackAPI* callback_ptr,
			TestModuleAPI* test_module_api_ptr,
			timespec* current_system_time_ptr,
			DpdkTestModuleType* test_module_type_ptr);
	void CheckAndHandleTimer(timespec* received_packet_timestamp_ptr);

protected:
    IModuleControlCallBackAPI* callback_ptr_;          //	Pointer to the DPDK Driver Factory Module
    TestModuleAPI*             test_module_api_p_;     //	Pointer to Test Module API
	DpdkTestModuleType*        test_module_type_ptr_;
    timespec*                  current_timestamp_ptr_; //	Pointer to the DPDK Driver Factory's current timestamp
};


/*******************************************************************************************//*!
*@class ClassifiedQueue
*@brief The purpose of this class is :
*@brief Implementation for DPDK Ethernet MBUF queue.
*@brief The worker threads may access the different queues for put and get data.
*@brief Only one worker thread may access some queue.
*@brief It implements IngressClassifiedQueueAPI. Derived from DpdkEthQueue.
***********************************************************************************************/
class ClassifiedQueue: public DpdkEthQueue , public IngressClassifiedQueueAPI
{
public:

	ClassifiedQueue();
	~ClassifiedQueue(){};

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
	uint16_t recvBurst(rte_mbuf** rx_pkts, uint16_t count);

	//Internal methods
	void Init(IModuleControlCallBackAPI* callback_ptr,
			uint32_t device_id,
			INGRESS_CLASSIFICATOR_Q_TYPE id,
			TestModuleAPI* test_module_api_ptr,
			timespec* current_system_time_ptr,
			timespec* start_time_ptr,
			timespec* last_time_ptr,
			timespec* offset_time_ptr,
			bool* pending_queue_flag_array_ptr,
			bool* is_empty_queue_flag_array_ptr,
		    bool* is_active_queue_flag_array_ptr,
			DpdkDriverClockMode* clock_mode_ptr,
			DpdkTestModuleType* test_module_type_ptr,
			bool* is_first_rx_packet,
			bool* is_test_ready_flag_ptr,
			bool* is_test_finished_flag_ptr,
			CMemArea* m_frontend_check_test_out,
			Dpdk_Buffer_Manager_API* dpdk_buffer_manager_api,
			CProfileCnt* DL_dpdk_send_Prof,
			ISplitAPI* oran_module_ptr);

	void Reset();
 
	struct PendingQueueItem
	{
		timespec  pending_packet_timestamp_;
		rte_mbuf* pending_packet_ptr_;
	};

private:
    INGRESS_CLASSIFICATOR_Q_TYPE ingress_queue_id_;	//	Type of Ingress Queue

    int32_t*  rx_mbuf_pool_write_index_ptr_;
	rte_mbuf* rx_mbufs_pool_ptr_;

    //DPDK mempools
    // direct mbufs pool for Offline Driver getting and saving the input packets from non-Ethernet source (e.g. from Files)
	rte_mempool*	input_packets_mempool_;

    DpdkDriverClockMode*   clock_mode_ptr_;

    //	Used only for kFrontHaul DPDK devices:
    bool*       pending_queue_flag_array_ptr_;
    bool*       is_first_rx_packet_;   //^^^^^
    bool*       is_test_ready_flag_ptr_;
    bool*       is_test_finished_flag_ptr_;
    bool*       is_empty_queue_flag_array_ptr_;
    bool*       is_active_queue_flag_array_ptr_;
    PendingQueueItem  pending_queue_;
    timespec*   offset_time_ptr_;
    timespec*   start_time_ptr_;
    timespec*   last_time_ptr_;
    CMemArea*   m_frontend_check_test_out_;
    CProfileCnt* dl_dpdk_send_prof_;
    ISplitAPI*  oran_module_ptr_;

	rte_ring*   input_mbufs_preallocated_ring_;

	uint32_t    trace_id_;

	bool        is_first_time_;


    EResultT ReceiveFromFronthaul_NonRTC(rte_mbuf*& pkt);
    EResultT ReceiveFromFronthaul_RTC(rte_mbuf*& pkt);
    EResultT ReceiveFromFronthaul_SystemTime(rte_mbuf*& pkt);
    EResultT ReceiveFromFPGA(rte_mbuf*& pkt);
    EResultT ObtainPacket(rte_mbuf*& pkt, timespec& timestamp);
    bool IsEarlyPacket(timespec* received_packet_timestamp, timespec* current_timestamp);
    bool IsPacketOnSystemTime(rte_mbuf* packet);
    bool AreAllQueuesPending();
    timespec GetTimestampFromMbuf(rte_mbuf* mbuf);
    void InitPendingQueue();
    void PrintTsDiff(timespec* packet_ts, timespec* curr_ts);
    bool AreAllQueuesEmpty();
    uint64_t ConvertTimestampInNsec(timespec* ts, timespec* offset_ts = nullptr);
    void GetMessageDescriptorFromPacket(rte_mbuf* packet, MESSAGE_DESCRIPTOR* message_descriptor);
};


/*******************************************************************************************//*!
*@class EgressQueue
*@brief The purpose of this class is :
*@brief Implementation for DPDK Ethernet MBUF queue.
*@brief It implements EgressQueueAPI. Derived from DpdkEthQueue.
***********************************************************************************************/
class EgressQueue: public DpdkEthQueue , public EgressQueueAPI
{
public:
    EgressQueue();
    ~EgressQueue(){};

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
    uint16_t sendBurst(rte_mbuf** tx_pkts, uint16_t count, bool free_unsent = true);


    //Internal methods
    void Init(IModuleControlCallBackAPI* callback_ptr,
    		EGRESS_Q_TYPE id,
			TestModuleAPI* test_module_api_ptr,
			timespec* current_system_time_ptr,
			DpdkTestModuleType* test_module_type_ptr,
			int32_t* tx_pool_write_index_ptr,
			uint8_t tx_pool_ptr[MAX_TX_MBUF_IN_POOL][DPDK_UPLANE_MTU_SIZE],
			CProfileCnt* DL_dpdk_send_Prof);

    void Reset();

private:
    EGRESS_Q_TYPE egress_queue_id_;		//	Type of Egress Queue

    int32_t*  tx_pool_write_index_ptr_;
    uint8_t   (*tx_pool_ptr_)[DPDK_UPLANE_MTU_SIZE];
    CProfileCnt*	dl_dpdk_send_prof_;
};


/******************************************************************************************//*!
 *@class CDPDK_Eth_driver
 *@brief This class represents Offline implementation of DPDK Ethernet device driver.
 *@brief The driver contains different RX and TX queues.
 *@brief The driver spreads the input data into number of queues according to the data plane
 *@brief and layer.
 *@brief It implements DpdkEthDeviceAPI.
 *********************************************************************************************/
class CDPDK_Eth_driver : public DpdkEthDeviceAPI
{
public:
    // Open port by port_id
    // \param[in] port_id       Port id to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
	EResultT open(uint16_t port_id, eth_dev_settings_t* dev_settings = NULL);

    // Open port by MAC
    // \param[in] mac           MAC address to initialize
    // \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
    // \return EResultT
	EResultT open(const uint8_t mac[ETH_ADDR_LEN], eth_dev_settings_t* dev_settings = NULL);

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
    
    EResultT assignSoftwareClassifier (DpdkClassifyAPI *classifier_api, uint16_t classifier_burst_size);
        
    EResultT readAndClassification();
            

    //This function returns the pointer to the corresponding classifier MBUF Ingress queue handler.
	//NULL if it does not exist
    IngressClassifiedQueueAPI* GetIngressClassifiedQueue( INGRESS_CLASSIFICATOR_Q_TYPE classified_q_val );

	//This function returns the pointer to the corresponding classifier MBUF Egress queue handler.
    //NULL if it does not exist
	EgressQueueAPI* GetEgressQueue( EGRESS_Q_TYPE egress_q_val );


    //Local methods

    void Reset();

    void Init(IModuleControlCallBackAPI* ptr,
    		uint32_t id,
			TestModuleAPI* test_module_api_ptr,
			timespec* current_system_time_ptr,
			timespec* start_time_ptr,
			timespec* offset_time_ptr,
			DpdkTestModuleType test_module_type,
			DpdkDriverClockMode* clock_mode_ptr,
			CMemArea* m_frontend_check_test_out,
			CProfileCnt* DL_dpdk_send_Prof,
			ISplitAPI* oran_module_ptr);


    CDPDK_Eth_driver();
    ~CDPDK_Eth_driver();

private:
    IModuleControlCallBackAPI* callback_ptr_;
    uint32_t dev_id_;
    timespec* offset_time_ptr_;
    timespec* current_system_time_ptr_;
    timespec* start_system_time_ptr_;
    timespec  last_test_time_;
    DpdkTestModuleType     test_module_type_;
    DpdkDriverClockMode*   clock_mode_ptr_;
    bool  is_first_rx_packet_;
    bool  is_test_ready_flag_;
    bool  is_test_finished_flag_;
    CMemArea *m_frontend_check_test_out_;
    bool first_packet_;

    ClassifiedQueue  rx_queues_[MAX_INGRESS_QUEUE_SIZE];
    EgressQueue		 tx_queues_[MAX_EGRESS_QUEUE_SIZE];
    bool             pending_queue_flag_array_[MAX_INGRESS_QUEUE_SIZE];
    bool             is_empty_queue_flag_array_[MAX_INGRESS_QUEUE_SIZE];
    bool             is_active_queue_flag_array_[MAX_INGRESS_QUEUE_SIZE];


	//DPDK Buffer Manager module connection
	IModuleControlAPI* 			m_dpdk_buffer_manager_p_;
	Dpdk_Buffer_Manager_API*	dpdk_buffer_manager_api_;
 
    //	This is temporary until buffer manager will be completed:
    int32_t  tx_pool_write_index_;
	uint8_t  tx_memory_pool_[MAX_TX_MBUF_IN_POOL][DPDK_UPLANE_MTU_SIZE];

	// private methods
	// Initiate required mempools settings for DPDK mempool init method
	EResultT	DpdkOfflineInitMempoolsSettings(buffer_manager_settings_t* dpdk_buffer_manager_settings);

};



/******************************************************************************************//*!
 *@class CCommDriverFactory
 *@brief The purpose of this class is :
 *@brief This class holds the communication drivers.
 *********************************************************************************************/
class CCommDriverFactory : public CBaseModule
{
    /***********************************************************************************/
private:
    //Examples of useful data types (optional)
    CProfileCnt comm_prof_;  //Example of profiler counter
    CDPDK_Eth_driver dpdk_eth_driver_[MAX_COMM_DPDK_DRIVERS];
    uint32_t num_ip_drivers_;
    timespec current_system_time_;	//	In case of MultiThreaded, it should be protected by lock
    timespec start_system_time_;
    timespec offset_system_time_;

    TestModuleAPI* glue_logic_test_p_;
    TestModuleAPI* fpga_sim_p_;
    TestModuleAPI* emulator_loader_p_;
    TestModuleAPI* pcap_parser_p_;
    ISplitAPI*     oran_module_p_;

    CMemArea *m_frontend_check_test_out_;

    DpdkDriverClockMode   clock_mode_;

    CProfileCnt dl_dpdk_send_prof_[2];

 public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info);
    EResultT IConfigure(EConfigId id, void *in, void **out);
    EResultT IColdStart();
    EResultT IHotStart();
    EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info);

    CCommDriverFactory()
    {
        num_ip_drivers_            = 0;
        glue_logic_test_p_         = nullptr;
        fpga_sim_p_                = nullptr;
        emulator_loader_p_         = nullptr;
        m_frontend_check_test_out_ = nullptr;
        clock_mode_                = kUndefinedTimeClock;
        oran_module_p_             = nullptr;
    }
};



#endif /* DPDK_ETH_H */
