/******************************************************************
 * tcp_ip_driver.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "dpdk_eth_driver_offline.h"
#include "i_networking_api.h"

#define MAXPENDING (5)


//#define RU_EMULATOR_DEBUG_FLAG	//	Enable prints for debugging of the Emulator
//#define TRACE_FLAG

#ifdef ARCH_ARM

#include <arm_neon.h>

uint64_t LUT_SRC[32] =   { 0x0000000000000000ULL,
                           0x0000000000000000ULL,
						   0xFF00000000000000ULL,
						   0x0000000000000000ULL,
                           0xFFFF000000000000ULL,
						   0x0000000000000000ULL,
						   0xFFFFFF0000000000ULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFF00000000ULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFF000000ULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFFFF0000ULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFFFFFF00ULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFF00000000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFFFF000000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFFFFFF0000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFFFFFFFF00000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFFFFFFFFFF000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0xFFFFFFFFFFFF0000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
                           0xFFFFFFFFFFFFFF00ULL
                         };

uint64_t LUT_DST[32] = 	 { 0xFFFFFFFFFFFFFFFFULL,
                           0xFFFFFFFFFFFFFFFFULL,
                           0x00FFFFFFFFFFFFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x0000FFFFFFFFFFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x000000FFFFFFFFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x00000000FFFFFFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x0000000000FFFFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x000000000000FFFFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x00000000000000FFULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0xFFFFFFFFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0x00FFFFFFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0x0000FFFFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0x000000FFFFFFFFFFULL,
						   0x0000000000000000ULL,
						   0x00000000FFFFFFFFULL,
						   0x0000000000000000ULL,
						   0x0000000000FFFFFFULL,
						   0x0000000000000000ULL,
						   0x000000000000FFFFULL,
						   0x0000000000000000ULL,
						   0x00000000000000FFULL
                         };

//	Written by: Lev Livshitz
void mem_cpy_neon(uint8_t *dst_ptr, uint8_t *src_ptr, uint32_t length)
{
  uint8x16_t  src_vec;               // 128-bit type (Q-register)
  uint8x16_t  dst_vec;               // 128-bit type (Q-register)
  uint8x16_t  temp1;               // 128-bit type (Q-register)
  uint8x16_t  temp2;               // 128-bit type (Q-register)
  uint32_t    num_128bit_InpVecs;
  uint32_t    residual_num;
  uint32_t    i;           // loop cntr
  uint8x8_t   v1; // define v as a vector with 8 lanes of 8-bit data
  uint8x8_t   v2; // define v as a vector with 8 lanes of 8-bit data
  uint8x16_t   v3; // define v as a vector with 16 lanes of 8-bit data
  uint8x16_t   v4; // define v as a vector with 16 lanes of 8-bit data

      num_128bit_InpVecs = length >> 4;                 // num of full 128-bit (16 bytes) vectors in the input buffer
      residual_num = length - num_128bit_InpVecs * 16;  // number bytes to copy in last 128-bit vec.

	  for(i = 0; i <  num_128bit_InpVecs; i++)
	  {
        // load 16 of 8-bit values from source memory
        src_vec = vld1q_u8((uint8_t*)src_ptr);

        // write to destination address
        vst1q_u8((uint8_t*)dst_ptr, src_vec);

        // increment the array pointer to the next element
        src_ptr += 16;     // increment source ptr. by 16 bytes
        dst_ptr += 16;     // increment destination ptr. by 16 bytes

	  } // End of for()

	  v1 = vcreate_u8(LUT_SRC[residual_num*2]); // create a vector
	  v2 = vcreate_u8(LUT_SRC[residual_num*2+1]); // create a vector
	  v3 = vcombine_u8(v1, v2);

	  v1 = vcreate_u8(LUT_DST[residual_num*2]); // create a vector
	  v2 = vcreate_u8(LUT_DST[residual_num*2+1]); // create a vector
	  v4 = vcombine_u8(v1, v2);

      // load last 16 of 8-bit values from  source memory
      src_vec = vld1q_u8((uint8_t*)src_ptr);
      // load last 16 of 8-bit values from destination memory
      dst_vec = vld1q_u8((uint8_t*)dst_ptr);
//      temp1 = vandq_u8(src_vec, LUT_SRC[residual_num]);
      temp1 = vandq_u8(src_vec, v3);
 //     temp2 = vandq_u8(dst_vec, LUT_DST[residual_num]);
      temp2 = vandq_u8(dst_vec, v4);
      src_vec = vorrq_u8(temp1, temp2);
	  // write to destination address
      vst1q_u8((uint8_t*)dst_ptr, src_vec);

}  // End of mem_cpy_neon() func

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												DpdkEthQueue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//	ctor
DpdkEthQueue::DpdkEthQueue()
{
	callback_ptr_          = nullptr;
	test_module_api_p_     = nullptr;
	current_timestamp_ptr_ = nullptr;
	test_module_type_ptr_  = nullptr;
}


//	dtor
DpdkEthQueue::~DpdkEthQueue()
{

}


/******************************************************************
 *@brief	This function initializes common fields of a DPDK
 *@brief	Ethernet device queue.
 *@brief
 *@brief	Input:	callback_ptr - pointer to the callback pointer of
 *@brief				the DPDK Driver factory module.
 *@brief			gl_test_api_ptr - pointer to GL Test module.
 *@brief			current_system_time_ptr - pointer to current system
 *@brief				time of the DPDK Driver factory module.
 *@brief
 *@brief	Output:	None.
 *@brief
 *******************************************************************/
void DpdkEthQueue::InitQueue(IModuleControlCallBackAPI* callback_ptr,
		TestModuleAPI* test_module_api_ptr,
		timespec* current_system_time_ptr,
		DpdkTestModuleType* test_module_type_ptr)
{
	callback_ptr_= callback_ptr;
	test_module_api_p_ = test_module_api_ptr;
	current_timestamp_ptr_ = current_system_time_ptr;
	test_module_type_ptr_ = test_module_type_ptr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												ClassifiedQueue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//	ctor
ClassifiedQueue::ClassifiedQueue()
{
	ingress_queue_id_              = INGRESS_LAST_CLASSIFIED_QUEUE;
	rx_mbuf_pool_write_index_ptr_  = nullptr;
	rx_mbufs_pool_ptr_             = nullptr;
	clock_mode_ptr_                = nullptr;
	is_empty_queue_flag_array_ptr_ = nullptr;
	is_test_ready_flag_ptr_        = nullptr;
	start_time_ptr_                = nullptr;
	pending_queue_flag_array_ptr_  = nullptr;
	input_packets_mempool_         = nullptr;
	input_mbufs_preallocated_ring_ = nullptr;
}


/******************************************************************
 *@brief	This function initializes members of a DPDK
 *@brief	Ethernet device Ingress queue.
 *@brief
 *@brief	Input:	callback_ptr - pointer to the callback pointer of
 *@brief				the DPDK Driver factory module.
 *@brief			device_id - DPDK driver id
 *@brief			id - Ingress queue ID
 *@brief			gl_test_api_ptr - pointer to GL Test module.
 *@brief			current_system_time_ptr - pointer to current system
 *@brief				time of the DPDK Driver factory module.
 *@brief			rx_mbuf_pool_write_index_ptr - pointer to rx mbuf pool's
 *@brief				current available index to be written
 *@brief			rx_mbufs_pool_ptr = pointer to rx mbuf pool
 *@brief
 *@brief	Output:	None.
 *@brief
 *******************************************************************/
void ClassifiedQueue::Init(IModuleControlCallBackAPI* callback_ptr,
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
		CProfileCnt* dl_dpdk_send_prof,
		ISplitAPI* oran_module_ptr)
{
	InitQueue(callback_ptr, test_module_api_ptr, current_system_time_ptr, test_module_type_ptr);
	uint32_t input_packets_max_buff_size = INPUT_PACKETS_MAX_SIZE;

	input_packets_mempool_ = dpdk_buffer_manager_api->getMempool(input_packets_max_buff_size);

	ASSERT(input_packets_mempool_)
	printf("ClassifiedQueue::Init new mempool allocated from Buffer Manager(0x%llx): Name = %s\tBufSize = %d\tNumBuffers = %d\tCacheSize = %d\n",
								dpdk_buffer_manager_api,
								(const char*)input_packets_mempool_->name,
								input_packets_mempool_->elt_size,
								input_packets_mempool_->size,
								input_packets_mempool_->cache_size);


	ingress_queue_id_ = id;
	pending_queue_flag_array_ptr_   = pending_queue_flag_array_ptr;
	Reset();
	is_empty_queue_flag_array_ptr_  = is_empty_queue_flag_array_ptr;
	is_active_queue_flag_array_ptr_ = is_active_queue_flag_array_ptr;
	start_time_ptr_ 				= start_time_ptr;
	last_time_ptr_ 			        = last_time_ptr;
	offset_time_ptr_ 		        = offset_time_ptr;
	clock_mode_ptr_ 	            = clock_mode_ptr;
	is_first_rx_packet_             = is_first_rx_packet;
	is_test_ready_flag_ptr_         = is_test_ready_flag_ptr;
	is_test_finished_flag_ptr_      = is_test_finished_flag_ptr;
	m_frontend_check_test_out_      = m_frontend_check_test_out;
	dl_dpdk_send_prof_ 		        = dl_dpdk_send_prof;
	oran_module_ptr_                = oran_module_ptr;


    //Create a Ring of preallocated mbufs in order to do the allocations offline and then use these buffers in run-time
	char ringName[128];
	uint32_t buffers_number = 0;
    sprintf(ringName, "inputMbufsPreallocRing_%d_%d", device_id, ingress_queue_id_);

    //closest_pow2(uint32_t buffers_number){

    	//return (buffers_number > 0) ? pow(2,ceil(log2(buffers_number))) - 1 : 0 ;
   // }
    buffers_number = (INPUT_PREALLOC_RING_SIZE > 0) ? pow(2,ceil(log2(INPUT_PREALLOC_RING_SIZE))) : 0 ;
    input_mbufs_preallocated_ring_ = rte_ring_create(ringName, buffers_number, SOCKET_ID_ANY, RING_F_SP_ENQ | RING_F_SC_DEQ);
    if (!input_mbufs_preallocated_ring_)
    {
      printf("Could not create input mbufs ring. ERRNO: %s\n", rte_strerror(rte_errno));
      ASSERT(0);
    }

	rte_mbuf* preallocated_mbufs_arr[INPUT_PREALLOC_RING_SIZE];
    int32_t ret_val = rte_pktmbuf_alloc_bulk(input_packets_mempool_, preallocated_mbufs_arr, INPUT_PREALLOC_RING_SIZE);
    if(ret_val)
    {
    	printf("Could not preallocate input mbufs. ERRNO = %d\n", ret_val);
    	ASSERT(0);
    }


    uint32_t 	free_space = 0;
    if (rte_ring_enqueue_bulk(	input_mbufs_preallocated_ring_,
    							(void * const*)(preallocated_mbufs_arr),
								INPUT_PREALLOC_RING_SIZE,
								&free_space) !=
								INPUT_PREALLOC_RING_SIZE)
    {
      printf("Could not enqueue %d packets to Ring. Packets dropped. Free space on Ring = %d\n", INPUT_PREALLOC_RING_SIZE, free_space);
    }

#ifdef TRACE_FLAG
    char trace_format[] = "OfflineDpdkdDriver:, TS=,\t%lld";
	callback_ptr_->IRegistryTraceEntry(trace_format, &trace_id_);
#endif

	is_first_time_ = true;
}


/******************************************************************
 *@brief	This function resets on Hot Start
 *@brief	Output:	None.
 *@brief
 *******************************************************************/
void ClassifiedQueue::Reset()
{
	InitPendingQueue();
}


/******************************************************************
 *@brief	This function receives an mbuf packet from a Test module's
 *@brief	correct Ingress queue.
 *@brief	It returns E_OK in case there is a packet to obtain, or
 *@brief	E_FAIL otherwise.
 *@brief
 *@brief	Input:	pkt (by-ref) - reference to pointer to rte_mbuf
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT ClassifiedQueue::recv(rte_mbuf*& pkt)
{
	EResultT ret_val;

	switch(*test_module_type_ptr_)
	{
		case kFpgaSimulator:
		{
			ret_val = ReceiveFromFPGA(pkt);
			break;
		}
		case kGlueLogicTest:
		case kEmulatorLoader:
		{
			//	Both GL Test and Emulator Loader represents the FrontHaul
			switch(*clock_mode_ptr_)
			{
				case kRealTimeClock:
				{		
#ifdef PROFILING_TEST
	dl_dpdk_send_prof_[1].Start();
#endif
					ret_val = ReceiveFromFronthaul_RTC(pkt);
#ifdef PROFILING_TEST
	dl_dpdk_send_prof_[1].Stop();
#endif
					break;
				}
				case kNonRealTimeClock:
				{
					ret_val = ReceiveFromFronthaul_NonRTC(pkt);
					break;
				}
				case kSystemTimeClock:
				{
					ret_val = ReceiveFromFronthaul_SystemTime(pkt);
					break;
				}
				default:
				{
					ASSERT(0);	//	Unsupported DPDK device type
				}
			}
			break;
		}
		default:
		{
			ASSERT(0);	//	Unsupported Test Module type
		}
	}

	return ret_val;
}


/******************************************************************
 *@brief	This function receives a burst of "count" mbuf packets from
 *@brief	TestModuleAPI's correct Ingress queue into "rx_pkts".
 *@brief	It returns the number of  actual received mbufs.
 *@brief
 *@brief	Input:	tx_pkts (by-ref) - pointer to rte_mbuf array to receive
 *@brief			count - mbufs count to receive
 *@brief
 *@brief	Output:	Number of received mbufs
 *@brief
 *******************************************************************/
uint16_t ClassifiedQueue::recvBurst(rte_mbuf** rx_pkts, uint16_t count)
{
	EResultT ret_val = E_OK;
	uint16_t received_packets_counter = 0;

	for(int32_t i=0; ( (i<count)  &&  (E_OK==ret_val) ); i++)
	{
		ret_val = recv(rx_pkts[i]);
		received_packets_counter++;
	}

	return received_packets_counter;
}


/******************************************************************
 *@function	ReceiveFromFronthaul_NonRTC
 *@brief	This function implements ReceiveFromFronthaul in case
 *@brief	that the there is no reference clock (system time
 *@brief	starts from 0).
 *@brief
 *@brief	Input:	pkt (by-ref)- reference to a pointer to a
 *@brief				mbuf that will store the received packet.
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT ClassifiedQueue::ReceiveFromFronthaul_NonRTC(rte_mbuf*& pkt)
{
	EResultT ret_val;

	//	1. Check if all rx queues are pending (only if the test is ready):
	if(  (true == *is_test_ready_flag_ptr_)  &&
	     AreAllQueuesPending()  )
	{
		// Check for the case of end of test
		if(AreAllQueuesEmpty())
		{
			// The first time in the current test that we reach here
			if(last_time_ptr_->tv_nsec == 0 && last_time_ptr_->tv_sec == 1000000)
			{
				// We take two slots from the last timestamp in order to operate at least 14 symbols
				// after the last clock preceeding - to allow the last slot proccesing
				last_time_ptr_->tv_nsec = current_timestamp_ptr_->tv_nsec + PROGRESS_NS_BETWEEN_SLOTS*2;
				last_time_ptr_->tv_sec  = current_timestamp_ptr_->tv_sec;
			}

			// The case where two slots are passed from the first time all the queues are empty (the point where
			// we need to finish the test)
			if(false == IsEarlyPacket(last_time_ptr_, current_timestamp_ptr_))
			{
				uint16_t *output_ptr = (uint16_t*)m_frontend_check_test_out_->AllocFIFO();
				*output_ptr = 0;
				m_frontend_check_test_out_->PushFIFO();
				last_time_ptr_->tv_nsec = 0;
				last_time_ptr_->tv_sec = 1000000; // large number which must be longer than test length
			}
			// If the test is not finished now we reset all the empty queues flags in order to proceed the time stamps
			// only once for each calling to receive C-Plane and U-Plane
			else
			{
				for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
				{
					is_empty_queue_flag_array_ptr_[i] = false;
				}
			}
		}
		//	1.1 Update system timer:
		*current_timestamp_ptr_ = Preceeding(*current_timestamp_ptr_, TIME_BETWEEN_TIME_SYNC);	//	Incrementing the current
																									//	timestamp by symbol duration
		callback_ptr_->ISyncTime(nullptr, current_timestamp_ptr_);

		//	1.2 Reset all pending queue flags:
		for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
		{
			pending_queue_flag_array_ptr_[i] = false;
		}

		// The last condition is to ensure that when we are in the end of the test mode
		// we don't need to check the pending queue
		if( (true == is_active_queue_flag_array_ptr_[ingress_queue_id_]) &&
			(false == is_empty_queue_flag_array_ptr_[ingress_queue_id_])  &&
			(false == IsEarlyPacket(&pending_queue_.pending_packet_timestamp_, current_timestamp_ptr_)) &&
			(last_time_ptr_->tv_nsec == 0 && last_time_ptr_->tv_sec == 1000000))
		{
			//	Since the queue is NOT empty and the pending packet's timestamp is NOT too early,
			//	return the packet that is stored in the pending queue:
			ASSERT(nullptr != pending_queue_.pending_packet_ptr_);	//	Make sure that pending_queue_ is NOT NULL at this point
			pkt = pending_queue_.pending_packet_ptr_;
			InitPendingQueue();
			return E_OK;
		}
		else
		{
			//	In case the queue is either empty or that the pending packet's timestamp is too early,
			//	return E_FAIL and pkt=NULL:
			pkt = nullptr;
			return E_FAIL;
		}
	}

	//	2. Check whether the queue is pending:
	if(true == pending_queue_flag_array_ptr_[ingress_queue_id_])
	{
		//	2.1 If it is pending - return an empty mbuf and E_FAIL:
		pkt = nullptr;
		return E_FAIL;
	}
	else
	{
		//	2.1 If it is NOT pending - check if the pending queue is not empty:
		if(nullptr != pending_queue_.pending_packet_ptr_)
		{
			if(false == IsEarlyPacket(&pending_queue_.pending_packet_timestamp_, current_timestamp_ptr_))
			{
				//	2.1.1	In case that the packet's timestamp is NOT too early, return
				//			the packet that is stored and return E_OK:
				pkt = pending_queue_.pending_packet_ptr_;
				InitPendingQueue();
				return E_OK;
			}
			else
			{
				//	2.1.2	In case that the timestamp of the pending packet is too early,
				//			set the pending flag to true and return NULL:
				pending_queue_flag_array_ptr_[ingress_queue_id_] = true;
				pkt = nullptr;
				return E_FAIL;
			}
		}
	}

	//	3. Since the pending queue is empty, obtain a new packet:
	rte_mbuf* obtained_packet;
	timespec obtained_packet_timestamp;
	timespec obtained_packet_timestamp_Decreasing;

	timespec slot_before;
	ret_val = ObtainPacket(obtained_packet, obtained_packet_timestamp);
	if((true == *is_first_rx_packet_)&&(ret_val==E_OK)) //Sync Linux time to time of first C-plane packet
	{
		if((ingress_queue_id_>=INGRESS_CPLANE_LAYER0_Q)&&(ingress_queue_id_<=INGRESS_CPLANE_LAYER7_Q)) //check if it's C-plane
		{

			// Decrease system time in order to be able to sync the app before first packet arrives.
			if (obtained_packet_timestamp.tv_nsec > NUMBER_SLOTS_START_TIME_RU_APPLICATION_BEFORE_FIRST_ORAN_PACKET*500000)
	//			*current_timestamp_ptr_ = Decreasing(obtained_packet_timestamp, TIME_TO_SHIFT_PCAP_DU );	//	Decreasing the current time stamp
	  		    *current_timestamp_ptr_ = Decreasing(obtained_packet_timestamp, NUMBER_SLOTS_START_TIME_RU_APPLICATION_BEFORE_FIRST_ORAN_PACKET*500000 );	//	Incrementing the current
			else
				*current_timestamp_ptr_ = obtained_packet_timestamp;
			//--------- Sync the system time with incoming time from the pcap ------
			//=> first synchronization by time stamp of the first pcap packet
			SysTimeT sfn_systime;
			//This is modulu process of 2.56
		    double	 timesttamp_in_sec          = (double)current_timestamp_ptr_->tv_sec+(( double)current_timestamp_ptr_->tv_nsec)/( double	)BILLION;
			double	 modulu_number              = ((double)timesttamp_in_sec / (double)(MAX_NUM_OF_FRAMES/(double)100)); // 2.56 seconds is the period of frames (256* 0.02) in 5G
			uint32_t modulu_number_floor        = (uint32_t)modulu_number;
		    double   modulu_timesttamp_in_sec   = (double)timesttamp_in_sec-(double)modulu_number_floor*((double)(MAX_NUM_OF_FRAMES/( double)100));

			sfn_systime.nf                      = (uint16_t)((double)modulu_timesttamp_in_sec*100); //100 frames in sec
			double modulu_timesttamp_sfn_in_sec = modulu_timesttamp_in_sec-( double)(sfn_systime.nf/(double)100);
			modulu_number                       = (double)modulu_timesttamp_sfn_in_sec / ( double)(0.001); //0.001 is length of subframe in sec
			modulu_number_floor                 = (uint32_t)modulu_number;
			sfn_systime.nsf                     = (uint16_t)(modulu_number_floor); //1000 frames in sec
			double modulu_subframes_in_sec      = (double)modulu_timesttamp_sfn_in_sec-( double)modulu_number_floor*((double)(0.001));
			sfn_systime.offset                  = (uint32_t)((double)modulu_subframes_in_sec*(double)MILLION); //MILLION is the number of micro-sec in second

			callback_ptr_->ISyncTime(nullptr, current_timestamp_ptr_);
			callback_ptr_->ISyncTime(&sfn_systime, nullptr);

			*is_first_rx_packet_ = false;
		}
	}
	if(E_FAIL == ret_val)
	{
		//	No data retrieved
		if(true == *is_test_ready_flag_ptr_)
		{
			is_empty_queue_flag_array_ptr_[ingress_queue_id_] = true;
		}
		pkt = nullptr;
		return ret_val;
	}

	//	Since a packet was obtain, set to true
	//	is_test_ready_flag_ptr_ in case it is false:
	if(false == (*is_test_ready_flag_ptr_))
	{
		*is_test_ready_flag_ptr_ = true;
	}

	//	Since a packet has been obtained, mark this current queue as active:
	is_active_queue_flag_array_ptr_[ingress_queue_id_] = true;

	//	If packet was obtained, mark this queue as NOT empty:
	is_empty_queue_flag_array_ptr_[ingress_queue_id_] = false;

	//	Check whether the obtain packet's timestamp is early or not:
	if( IsEarlyPacket(&obtained_packet_timestamp, current_timestamp_ptr_) )
	{
		//	3.1	In case the obtained packet's timestamp has NOT yet
		//		arrived - store it's pointer in the pending queue and raise the pending flag:
		pending_queue_flag_array_ptr_[ingress_queue_id_] = true;
		ASSERT(nullptr == pending_queue_.pending_packet_ptr_);	//	Make sure that pending_queue_ is NULL at this point
		pending_queue_.pending_packet_ptr_ = obtained_packet;
		pending_queue_.pending_packet_timestamp_ = obtained_packet_timestamp;
		pkt = nullptr;
		return E_FAIL;
	}
	else
	{
		//	3.2	In case the obtained packet's timestamp has arrived - return
		//		it:
		ASSERT(nullptr != obtained_packet);	//	Make sure that obtained_packet is NOT NULL at this point
		pkt = obtained_packet;
		return E_OK;
	}
}


/******************************************************************
 *@function	ReceiveFromFronthaul_RTC
 *@brief	This function implements ReceiveFromFronthaul in case
 *@brief	that the reference clock is the Linux time.
 *@brief
 *@brief	Input:	pkt (by-ref)- reference to a pointer to a
 *@brief				mbuf that will store the received packet.
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT ClassifiedQueue::ReceiveFromFronthaul_RTC(rte_mbuf*& pkt)
{
#pragma inline
	EResultT ret_val;

	//	1. First, get the current Linux time:
	clock_gettime(CLOCK_MONOTONIC, current_timestamp_ptr_);

#ifdef TRACE_FLAG
	timespec tik, tok;
	uint64_t tik_in_nsec, tok_in_nsec, diff_in_nsec;
#endif

	if(is_first_time_)
	{
		is_first_time_ = false;
		pkt = nullptr;
		return E_FAIL;
	}

	if(true == *is_test_finished_flag_ptr_)
	{
		// The case where two slots are passed from the first time all the queues are empty (the point where
		// we need to finish the test)
		if(false == IsEarlyPacket(last_time_ptr_, current_timestamp_ptr_))
		{
			uint16_t *output_ptr = (uint16_t*)m_frontend_check_test_out_->AllocFIFO();
			*output_ptr = 0;
			m_frontend_check_test_out_->PushFIFO();
			last_time_ptr_->tv_nsec = 0;
			last_time_ptr_->tv_sec = 1000000; // large number which must be longer than test length
		}

		//	If the test is finished - exit:
		pkt = nullptr;
		return E_FAIL;
	}
	else if( (true == (*is_test_ready_flag_ptr_) )	&&	(true == AreAllQueuesEmpty() )		)
	{
		//	In case that the test has already started and all the queues are empty,
		//	it means that the test has ended. Raise is_test_finished_flag_ptr_ and exit:
		*is_test_finished_flag_ptr_ = true;
#ifdef RU_EMULATOR_DEBUG_FLAG
		uint64_t curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_/*, offset_time_ptr_*/);
		printf("Finished Test! @ %lld\n", curr_ts_in_nsec);
#endif

		// The first time in the current test that we reach here
		if(last_time_ptr_->tv_nsec == 0 && last_time_ptr_->tv_sec == 1000000)
		{
			// We take two slots from the last timestamp in order to operate at least 14 symbols
			// after the last clock preceeding - to allow the last slot proccesing
			last_time_ptr_->tv_sec = 0;
			last_time_ptr_->tv_nsec = PROGRESS_NS_BETWEEN_SLOTS*2;
		}

		// If the test is not finished now we reset all the empty queues flags in order to proceed the time stamps
		// only once for each calling to receive C-Plane and U-Plane
		for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
		{
			is_empty_queue_flag_array_ptr_[i] = false;
		}


		pkt = nullptr;
		return E_FAIL;
	}

	//	2. Check if the pending queue is not empty:
	if(nullptr != pending_queue_.pending_packet_ptr_)
	{
#ifdef TRACE_FLAG
		clock_gettime(CLOCK_MONOTONIC, &tik);
#endif
		timespec ts_in_pending_packet = pending_queue_.pending_packet_timestamp_;
		//	Check pending queue packet's timestamp:
		if( IsEarlyPacket(&ts_in_pending_packet, current_timestamp_ptr_) )
		{
			//	If the pending packet is too early, return E_FAIL:
#ifdef RU_EMULATOR_DEBUG_FLAG
			uint64_t curr_ts_in_nsec;
			if(kEmulatorLoader == *test_module_type_ptr_)
			{
				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
			}
			else
			{
				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
			}
			printf("### %lld Pending queue %d is full, the TS is still too early [%lld]\n",
					curr_ts_in_nsec,
					ingress_queue_id_,
					ConvertTimestampInNsec(&ts_in_pending_packet, start_time_ptr_) );
#endif
			pkt = nullptr;
			return E_FAIL;
		}
		else
		{
			//	If the pending packet is NOT too early, return
			//	the packet that is stored and return E_OK:
			pkt = pending_queue_.pending_packet_ptr_;
			InitPendingQueue();
#ifdef RU_EMULATOR_DEBUG_FLAG
			uint64_t curr_ts_in_nsec;
			if(kEmulatorLoader == *test_module_type_ptr_)
			{
				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
			}
			else
			{
				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
			}
			printf("### %lld Returning packet from pending queue %d [%lld]\n",
					curr_ts_in_nsec,
					ingress_queue_id_,
					ConvertTimestampInNsec(&ts_in_pending_packet, start_time_ptr_) );
#endif
			return E_OK;
		}
#ifdef TRACE_FLAG
		clock_gettime(CLOCK_MONOTONIC, &tok);
		tik_in_nsec  = tik.tv_sec*BILLION + tik.tv_nsec;
		tok_in_nsec  = tok.tv_sec*BILLION + tok.tv_nsec;
		diff_in_nsec = tok_in_nsec - tik_in_nsec;
		callback_ptr_->ITraceData(trace_id_, __LINE__, diff_in_nsec);
#endif
	}

	//	3. Since the pending queue is empty, obtain a new packet:
	rte_mbuf* obtained_packet;
	timespec obtained_packet_timestamp;
#ifdef TRACE_FLAG
	clock_gettime(CLOCK_MONOTONIC, &tik);
#endif
	ret_val = ObtainPacket(obtained_packet, obtained_packet_timestamp);
#ifdef TRACE_FLAG
	clock_gettime(CLOCK_MONOTONIC, &tok);
	tik_in_nsec  = tik.tv_sec*BILLION + tik.tv_nsec;
	tok_in_nsec  = tok.tv_sec*BILLION + tok.tv_nsec;
	diff_in_nsec = tok_in_nsec - tik_in_nsec;
	callback_ptr_->ITraceData(trace_id_, __LINE__, diff_in_nsec);
#endif
	if(E_FAIL == ret_val)
	{
		//	No data retrieved
		pkt = nullptr;
		if(true == (*is_test_ready_flag_ptr_))
		{
			// If the test has started, mark this queue as empty:
			is_empty_queue_flag_array_ptr_[ingress_queue_id_] = true;
		}
		return ret_val;
	}

	//	4. Since a packet was obtain, set to true
	//	   is_test_ready_flag_ptr_ in case it is false:
	if(false == (*is_test_ready_flag_ptr_))
	{
		*offset_time_ptr_ = obtained_packet_timestamp;
		*is_test_ready_flag_ptr_ = true;
	}

	//	5. Since a packet was obtained, mark this queue as NOT empty:
	is_empty_queue_flag_array_ptr_[ingress_queue_id_] = false;

	//	6. Since a packet has been obtained, mark this current queue as active:
	is_active_queue_flag_array_ptr_[ingress_queue_id_] = true;

	//	7. Check whether the obtain packet's timestamp is too early or not:
	if( IsEarlyPacket(&obtained_packet_timestamp, current_timestamp_ptr_) )
	{
		//	7.1	In case the obtained packet's timestamp has NOT yet
		//		arrived - store it's pointer in the pending queue:
		ASSERT(nullptr == pending_queue_.pending_packet_ptr_);	//	Make sure that pending_queue_ is NULL at this point
		pending_queue_.pending_packet_timestamp_ = obtained_packet_timestamp;
		pending_queue_.pending_packet_ptr_ = obtained_packet;
#ifdef RU_EMULATOR_DEBUG_FLAG
		uint64_t curr_ts_in_nsec;
		if(kEmulatorLoader == *test_module_type_ptr_)
		{
			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
		}
		else
		{
			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
		}
		printf("### %lld Got an early packet, push to pending queue %d [%lld]\n",
				curr_ts_in_nsec,
				ingress_queue_id_,
				ConvertTimestampInNsec(&obtained_packet_timestamp, start_time_ptr_) );
#endif
		pkt = nullptr;
		return E_FAIL;
	}
	else
	{
		//	7.2 Update start_time_ptr_ value only in case it is the first packet:
		if(true == *is_first_rx_packet_)
		{
			*start_time_ptr_  = *current_timestamp_ptr_;
			*is_first_rx_packet_ = false;
		}

		//	7.3	In case the obtained packet's timestamp has arrived - return
		//		it:
		pkt = obtained_packet;
#ifdef RU_EMULATOR_DEBUG_FLAG
		uint64_t curr_ts_in_nsec;
		if(kEmulatorLoader == *test_module_type_ptr_)
		{
			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
		}
		else
		{
			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
		}
		uint64_t pkt_ts_in_nsec = ConvertTimestampInNsec(&obtained_packet_timestamp, start_time_ptr_);
		printf("### %lld Got a valid packet! in queue %d [%lld]\n",
				curr_ts_in_nsec,
				ingress_queue_id_,
				pkt_ts_in_nsec);
#endif
#ifdef TRACE_FLAG
		uint64_t start_ts_in_nsec  = start_time_ptr_->tv_sec*BILLION + start_time_ptr_->tv_nsec;
		uint64_t packet_ts_in_nsec = obtained_packet_timestamp.tv_sec*BILLION + obtained_packet_timestamp.tv_nsec;
		uint64_t ts_in_nsec = start_ts_in_nsec + packet_ts_in_nsec;
		callback_ptr_->ITraceData(trace_id_, __LINE__, ts_in_nsec);
#endif
		return E_OK;
	}
}


/******************************************************************
 *@function	ReceiveFromFronthaul_SystemTime
 *@brief	This function implements ReceiveFromFronthaul in case
 *@brief	that the reference clock is the system time
 *@brief	(frame, slot, symbol).
 *@brief
 *@brief	Input:	pkt (by-ref)- reference to a pointer to a
 *@brief				mbuf that will store the received packet.
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT ClassifiedQueue::ReceiveFromFronthaul_SystemTime(rte_mbuf*& pkt)
{
	EResultT ret_val;

	//	1. First, get the current Linux time:
	clock_gettime(CLOCK_MONOTONIC, current_timestamp_ptr_);


	if(true == *is_test_finished_flag_ptr_)
	{
		// The case where two slots are passed from the first time all the queues are empty (the point where
		// we need to finish the test)
		if(false == IsEarlyPacket(last_time_ptr_, current_timestamp_ptr_))
		{
			uint16_t *output_ptr = (uint16_t*)m_frontend_check_test_out_->AllocFIFO();
			*output_ptr = 0;
			m_frontend_check_test_out_->PushFIFO();
			last_time_ptr_->tv_nsec = 0;
			last_time_ptr_->tv_sec = 1000000; // large number which must be longer than test length
		}

		//	If the test is finished - exit:
		pkt = nullptr;
		return E_FAIL;
	}
	else if( (true == (*is_test_ready_flag_ptr_) )	&&	(true == AreAllQueuesEmpty() )		)
	{
		//	In case that the test has already started and all the queues are empty,
		//	it means that the test has ended. Raise is_test_finished_flag_ptr_ and exit:
		*is_test_finished_flag_ptr_ = true;
#ifdef RU_EMULATOR_DEBUG_FLAG
		uint64_t curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_/*, offset_time_ptr_*/);
		printf("Finished Test! @ %lld\n", curr_ts_in_nsec);
#endif

		// The first time in the current test that we reach here
		if(last_time_ptr_->tv_nsec == 0 && last_time_ptr_->tv_sec == 1000000)
		{
			// We take two slots from the last timestamp in order to operate at least 14 symbols
			// after the last clock preceeding - to allow the last slot proccesing
			last_time_ptr_->tv_sec = 0;
			last_time_ptr_->tv_nsec = PROGRESS_NS_BETWEEN_SLOTS*2;
		}

		// If the test is not finished now we reset all the empty queues flags in order to proceed the time stamps
		// only once for each calling to receive C-Plane and U-Plane
		for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
		{
			is_empty_queue_flag_array_ptr_[i] = false;
		}


		pkt = nullptr;
		return E_FAIL;
	}



	//	3. Since the pending queue is empty, obtain a new packet:
	rte_mbuf* obtained_packet;
	timespec obtained_packet_timestamp;
	ret_val = ObtainPacket(obtained_packet, obtained_packet_timestamp);
	if(E_FAIL == ret_val)
	{
		//	No data retrieved
		pkt = nullptr;
		if(true == (*is_test_ready_flag_ptr_))
		{
			// If the test has started, mark this queue as empty:
			is_empty_queue_flag_array_ptr_[ingress_queue_id_] = true;
		}
		return ret_val;
	}

	//	4. Since a packet was obtain, set to true
	//	   is_test_ready_flag_ptr_ in case it is false:
	if(false == (*is_test_ready_flag_ptr_))
	{
		*is_test_ready_flag_ptr_ = true;
	}

	//	5. Since a packet was obtained, mark this queue as NOT empty:
	is_empty_queue_flag_array_ptr_[ingress_queue_id_] = false;

	//	6. Since a packet has been obtained, mark this current queue as active:
	is_active_queue_flag_array_ptr_[ingress_queue_id_] = true;

	//	7. Check whether the obtain packet's timestamp is too early or not:
	//	7.2 Update start_time_ptr_ value only in case it is the first packet:
	if(true == *is_first_rx_packet_)
	{
		*start_time_ptr_  = *current_timestamp_ptr_;
		*is_first_rx_packet_ = false;
	}

	//	7.3	In case the obtained packet's timestamp has arrived - return
	//		it:
	pkt = obtained_packet;
	return E_OK;

}


///******************************************************************
// *@function	ReceiveFromFronthaul_SystemTime
// *@brief	This function implements ReceiveFromFronthaul in case
// *@brief	that the reference clock is the system time
// *@brief	(frame, slot, symbol).
// *@brief
// *@brief	Input:	pkt (by-ref)- reference to a pointer to a
// *@brief				mbuf that will store the received packet.
// *@brief
// *@brief	Output:	EResultT
// *@brief
// *******************************************************************/
//EResultT ClassifiedQueue::ReceiveFromFronthaul_SystemTime(rte_mbuf*& pkt)
//{
//	EResultT ret_val;
//
//	//	1. First, get the current Linux time:
//	clock_gettime(CLOCK_MONOTONIC, current_timestamp_ptr_);
//
//
//	if(true == *is_test_finished_flag_ptr_)
//	{
//		// The case where two slots are passed from the first time all the queues are empty (the point where
//		// we need to finish the test)
//		if(false == IsEarlyPacket(last_time_ptr_, current_timestamp_ptr_))
//		{
//			uint16_t *output_ptr = (uint16_t*)m_frontend_check_test_out_->AllocFIFO();
//			*output_ptr = 0;
//			m_frontend_check_test_out_->PushFIFO();
//			last_time_ptr_->tv_nsec = 0;
//			last_time_ptr_->tv_sec = 1000000; // large number which must be longer than test length
//		}
//
//		//	If the test is finished - exit:
//		pkt = nullptr;
//		return E_FAIL;
//	}
//	else if( (true == (*is_test_ready_flag_ptr_) )	&&	(true == AreAllQueuesEmpty() )		)
//	{
//		//	In case that the test has already started and all the queues are empty,
//		//	it means that the test has ended. Raise is_test_finished_flag_ptr_ and exit:
//		*is_test_finished_flag_ptr_ = true;
//#ifdef RU_EMULATOR_DEBUG_FLAG
//		uint64_t curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_/*, offset_time_ptr_*/);
//		printf("Finished Test! @ %lld\n", curr_ts_in_nsec);
//#endif
//
//		// The first time in the current test that we reach here
//		if(last_time_ptr_->tv_nsec == 0 && last_time_ptr_->tv_sec == 1000000)
//		{
//			// We take two slots from the last timestamp in order to operate at least 14 symbols
//			// after the last clock preceeding - to allow the last slot proccesing
//			last_time_ptr_->tv_sec = 0;
//			last_time_ptr_->tv_nsec = PROGRESS_NS_BETWEEN_SLOTS*2;
//		}
//
//		// If the test is not finished now we reset all the empty queues flags in order to proceed the time stamps
//		// only once for each calling to receive C-Plane and U-Plane
//		for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
//		{
//			is_empty_queue_flag_array_ptr_[i] = false;
//		}
//
//
//		pkt = nullptr;
//		return E_FAIL;
//	}
//
//
//	//	2. Check if the pending queue is not empty:
//	if(nullptr != pending_queue_.pending_packet_ptr_)
//	{
//		//	Check pending queue packet's system time:
//		if( IsPacketOnSystemTime(pending_queue_.pending_packet_ptr_) )
//		{
//			//	If the pending packet is NOT too early, return
//			//	the packet that is stored and return E_OK:
//			pkt = pending_queue_.pending_packet_ptr_;
//			InitPendingQueue();
//#ifdef RU_EMULATOR_DEBUG_FLAG
//			uint64_t curr_ts_in_nsec;
//			if(kEmulatorLoader == *test_module_type_ptr_)
//			{
//				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
//			}
//			else
//			{
//				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
//			}
//			printf("### %lld Returning packet from pending queue %d [%lld]\n",
//					curr_ts_in_nsec,
//					ingress_queue_id_,
//					ConvertTimestampInNsec(&ts_in_pending_packet, start_time_ptr_) );
//#endif
//			return E_OK;
//		}
//		else
//		{
//			//	If the pending packet is too early, return E_FAIL:
//#ifdef RU_EMULATOR_DEBUG_FLAG
//			uint64_t curr_ts_in_nsec;
//			if(kEmulatorLoader == *test_module_type_ptr_)
//			{
//				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
//			}
//			else
//			{
//				curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
//			}
//			printf("### %lld Pending queue %d is full, the TS is still too early [%lld]\n",
//					curr_ts_in_nsec,
//					ingress_queue_id_,
//					ConvertTimestampInNsec(&ts_in_pending_packet, start_time_ptr_) );
//#endif
//			pkt = nullptr;
//			return E_FAIL;
//		}
//	}
//
//
//	//	3. Since the pending queue is empty, obtain a new packet:
//	rte_mbuf* obtained_packet;
//	timespec obtained_packet_timestamp;
//	ret_val = ObtainPacket(obtained_packet, obtained_packet_timestamp);
//	if(E_FAIL == ret_val)
//	{
//		//	No data retrieved
//		pkt = nullptr;
//		if(true == (*is_test_ready_flag_ptr_))
//		{
//			// If the test has started, mark this queue as empty:
//			is_empty_queue_flag_array_ptr_[ingress_queue_id_] = true;
//		}
//		return ret_val;
//	}
//
//	//	4. Since a packet was obtain, set to true
//	//	   is_test_ready_flag_ptr_ in case it is false:
//	if(false == (*is_test_ready_flag_ptr_))
//	{
//		*is_test_ready_flag_ptr_ = true;
//	}
//
//	//	5. Since a packet was obtained, mark this queue as NOT empty:
//	is_empty_queue_flag_array_ptr_[ingress_queue_id_] = false;
//
//	//	6. Since a packet has been obtained, mark this current queue as active:
//	is_active_queue_flag_array_ptr_[ingress_queue_id_] = true;
//
//	//	7. Check whether the obtain packet's timestamp is too early or not:
//	if( IsPacketOnSystemTime(obtained_packet) )
//	{
//		//	7.2 Update start_time_ptr_ value only in case it is the first packet:
//		if(true == *is_first_rx_packet_)
//		{
//			*start_time_ptr_  = *current_timestamp_ptr_;
//			*is_first_rx_packet_ = false;
//		}
//
//		//	7.3	In case the obtained packet's timestamp has arrived - return
//		//		it:
//		pkt = obtained_packet;
//#ifdef RU_EMULATOR_DEBUG_FLAG
//		uint64_t curr_ts_in_nsec;
//		if(kEmulatorLoader == *test_module_type_ptr_)
//		{
//			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
//		}
//		else
//		{
//			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
//		}
//		uint64_t pkt_ts_in_nsec = ConvertTimestampInNsec(&obtained_packet_timestamp, start_time_ptr_);
//		printf("### %lld Got a valid packet! in queue %d [%lld]\n",
//				curr_ts_in_nsec,
//				ingress_queue_id_,
//				pkt_ts_in_nsec);
//#endif
//		return E_OK;
//	}
//	else
//	{
//		//	7.1	In case the obtained packet's timestamp has NOT yet
//		//		arrived - store it's pointer in the pending queue:
//		ASSERT(nullptr == pending_queue_.pending_packet_ptr_);	//	Make sure that pending_queue_ is NULL at this point
//		MESSAGE_DESCRIPTOR msg_desc;
//		GetMessageDescriptorFromPacket(obtained_packet, &msg_desc);
//		pending_queue_.pending_packet_frame_id_  = msg_desc.frame_id_;
//		pending_queue_.pending_packet_slot_id_   = msg_desc.slot_id_;
//		pending_queue_.pending_packet_symbol_id_ = msg_desc.symbol_id_;
//		pending_queue_.pending_packet_ptr_       = obtained_packet;
//#ifdef RU_EMULATOR_DEBUG_FLAG
//		uint64_t curr_ts_in_nsec;
//		if(kEmulatorLoader == *test_module_type_ptr_)
//		{
//			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_, offset_time_ptr_);
//		}
//		else
//		{
//			curr_ts_in_nsec = ConvertTimestampInNsec(current_timestamp_ptr_);
//		}
//		printf("### %lld Got an early packet, push to pending queue %d [%lld]\n",
//				curr_ts_in_nsec,
//				ingress_queue_id_,
//				ConvertTimestampInNsec(&obtained_packet_timestamp, start_time_ptr_) );
//#endif
//		pkt = nullptr;
//		return E_FAIL;
//	}
//}


EResultT ClassifiedQueue::ReceiveFromFPGA(rte_mbuf*& pkt)
{
	EResultT ret_val;

	//	Try to obtain a new packet:
	rte_mbuf* obtained_packet;
	timespec obtained_packet_timestamp;
	ret_val = ObtainPacket(obtained_packet, obtained_packet_timestamp);
	if(E_FAIL == ret_val)
	{
		//	No data retrieved
		return ret_val;
	}

	ASSERT(nullptr != obtained_packet);	//	Make sure that obtained_packet is NOT NULL at this point
	pkt = obtained_packet;
	return E_OK;
}


inline EResultT ClassifiedQueue::ObtainPacket(rte_mbuf*& pkt, timespec& timestamp)
{
	uint8_t* data_from_test_module;
	uint8_t* ethernet_frame;
	OfflineDataHeader* offline_data_header;
	struct pcap_pkthdr *pcap_header;
	EResultT ret_val;
	if(nullptr != test_module_api_p_)
	{
		ret_val = test_module_api_p_->GetPacket(ingress_queue_id_, data_from_test_module, test_module_type_ptr_);
	}
	else
	{
		//	In case this is a stub test module - skip GetPacket and return E_FAIL:
		ret_val = E_FAIL;
	}

	if(E_FAIL == ret_val)
	{
		//	No data retrieved
		return ret_val;
	}
#ifdef PCAP_HEADER_MODE
	//	Parse PCAP header from retrieved data:
	pcap_header = reinterpret_cast<pcap_pkthdr*>(data_from_test_module);
	//	Skip PCAP header:
	ethernet_frame = data_from_test_module + sizeof(pcap_pkthdr);
#else
	//	Parse OfflineDataHeader from retrieved data:
	offline_data_header = reinterpret_cast<OfflineDataHeader*>(data_from_test_module);

	//	Skip Offline data header:
	ethernet_frame = data_from_test_module + sizeof(OfflineDataHeader);
#endif
	//	Get free mbuf from pool:
	//	TODO: future work - get mbuf from buffer_manager
	// pkt = &rx_mbufs_pool_ptr_[*rx_mbuf_pool_write_index_ptr_];
	// *rx_mbuf_pool_write_index_ptr_ = (*rx_mbuf_pool_write_index_ptr_ + 1) % MAX_RX_MBUF_IN_POOL;

	//	Get free mbuf from pool:
	//pkt = rte_pktmbuf_alloc(input_packets_mempool_);
	 uint32_t retVal = rte_ring_dequeue(input_mbufs_preallocated_ring_, (void **)&pkt);
	 if(unlikely(retVal))
	 {
		 printf("Could not dequeue packet from the input ring. ERRNO = %d\n", retVal);
		 ASSERT(0)
	 }
	 else
	 {
		 // increment refcnt of the mbuf for using it again without allocation from mempool, but taking it from our preallocated Ring array
		 pkt->refcnt++;
		 // Enqueue the mbuf ptr back to the ring for next usage.
		 // We expect that the Ring will be big enough so that when we will use this mbuf again
		 // it already should be released by the DPDK Offline Driver (in case of Offline run) or by HW (in case of Online DPDK Driver usage on Send)
		 retVal = rte_ring_enqueue(input_mbufs_preallocated_ring_, (void *)(pkt));
		 if (unlikely(retVal))
		 {
			 printf("Could not enqueue packet back to the input ring. ERRNO = %d\n", retVal);
			 ASSERT(0)
		 }
	 }

#ifdef RU_RT_MODE
#ifdef PRINT_MEMPOOL_LOG
	uint32_t available_buffers_number  = rte_mempool_avail_count(input_packets_mempool_);
	uint32_t in_use_buffers_number = rte_mempool_in_use_count(input_packets_mempool_);
	printf("input_packets mempool %llx: available_buffers_number=%d, in_use_buffers_number=%d\n",input_packets_mempool_, available_buffers_number, in_use_buffers_number);
#endif
#endif

   // uint16_t free_mbufs =  rte_mempool_avail_count(input_packets_mempool_);
    //printf("Number of free bufs %d\n", free_mbufs);

	// validate allocated buffer is non-NULL
	ASSERT(pkt);


	uint8_t* data_in_mbuf = rte_pktmbuf_mtod(pkt, uint8_t*);
#ifdef PCAP_HEADER_MODE
	//	Copy data to mbuf:
	memcpy(data_in_mbuf, ethernet_frame, pcap_header->len);
	//	Update mbuf's data length:
	pkt->pkt_len = pkt->data_len = pcap_header->len;
	timestamp.tv_sec=pcap_header->ts.tv_sec;
	timestamp.tv_nsec=pcap_header->ts.tv_usec*1000;
#else
	//	Copy data to mbuf:
//#ifdef  ARCH_ARM
//	mem_cpy_neon(data_in_mbuf, ethernet_frame, offline_data_header->data_length);
//#elif defined ARCH_X86
	memcpy(data_in_mbuf, ethernet_frame, offline_data_header->data_length);
//#endif
	//	Update mbuf's data length:
//	rte_pktmbuf_data_len(pkt) = offline_data_header->data_length;
	pkt->pkt_len = pkt->data_len = offline_data_header->data_length;
	timestamp = offline_data_header->timestamp;
#endif
	pkt->nb_segs = 1;
	pkt->next    = nullptr;
	return ret_val;
}


bool ClassifiedQueue::AreAllQueuesPending()
{
	bool result = true;
	bool is_queue_pending_or_empty;
	int32_t start_queue_id;

	start_queue_id = INGRESS_NON_CLASSIFIED_Q;
	for(int32_t i=start_queue_id; ( (i<MAX_INGRESS_QUEUE_SIZE)  && (true==result) ); i++)
	{
		if(true == is_active_queue_flag_array_ptr_[i])
		{
			//	Check if the current queue is either empty or pending
			//	only in case the queue is active:
			is_queue_pending_or_empty = (pending_queue_flag_array_ptr_[i] ||
										 is_empty_queue_flag_array_ptr_[i]   );
			result = result && is_queue_pending_or_empty;
		}
	}

	return result;
}


bool ClassifiedQueue::IsEarlyPacket(timespec* received_packet_timestamp, timespec* current_timestamp)
{
	uint64_t offset_ts_in_nsec= offset_time_ptr_->tv_sec*BILLION + offset_time_ptr_->tv_nsec;
	uint64_t pkt_ts_in_nsec   = received_packet_timestamp->tv_sec*BILLION + received_packet_timestamp->tv_nsec;
	uint64_t start_ts_in_nsec = start_time_ptr_->tv_sec*BILLION + start_time_ptr_->tv_nsec;
	uint64_t curr_ts_in_nsec  = current_timestamp->tv_sec*BILLION + current_timestamp->tv_nsec;
	if( (pkt_ts_in_nsec + start_ts_in_nsec) > (curr_ts_in_nsec + offset_ts_in_nsec) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void ClassifiedQueue::GetMessageDescriptorFromPacket(rte_mbuf* packet, MESSAGE_DESCRIPTOR* message_descriptor)
{
	uint8_t* payload_ptr;
	uint8_t* pcap_ptr;
	uint8_t* oran_packet_header_ptr;

	payload_ptr = rte_pktmbuf_mtod(packet, uint8_t*);
	oran_packet_header_ptr = payload_ptr + sizeof(EtherHeadr);		//	Skip Ethernet header

	EResultT ret_val = oran_module_ptr_->Classify(oran_packet_header_ptr, message_descriptor);
}


bool ClassifiedQueue::IsPacketOnSystemTime(rte_mbuf* packet)
{
	bool result;
	uint32_t system_frame_id, system_subframe_id, system_slot_id, system_symbol_id;
	uint32_t packet_frame_id, packet_subframe_id, packet_slot_id, packet_symbol_id;

	SysTimeT curr_system_time;
	callback_ptr_->IGetSysTime(&curr_system_time);

	system_frame_id  = curr_system_time.nf;
	system_subframe_id  = curr_system_time.nsf;
	system_slot_id   = curr_system_time.GetSlotNum();
	system_symbol_id = (curr_system_time.offset % SLOT_DURATION) / SYMBL_DURATION;

	MESSAGE_DESCRIPTOR msg_desc;
	GetMessageDescriptorFromPacket(packet, &msg_desc);

	packet_frame_id  = msg_desc.frame_id_;
	packet_subframe_id  = msg_desc.subframe_id_;
	packet_slot_id   = msg_desc.slot_id_;
	packet_symbol_id = msg_desc.symbol_id_;

	if(msg_desc.plane_type_ == kUPlane)
	{
		if(	(packet_frame_id == system_frame_id) &&
			(packet_subframe_id == system_subframe_id) &&
			(packet_slot_id == system_slot_id) 	 &&
			(packet_symbol_id == system_symbol_id)   )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else	//	C-Plane
	{
		system_slot_id = system_slot_id + 1;
		if(system_slot_id >= MAX_NUM_SLOT_PER_SF)
		{
			system_slot_id = 0;
			system_subframe_id = system_subframe_id + 1;
			if(system_subframe_id == MAX_NUM_OF_SF)
			{
				system_subframe_id = 0;
				system_frame_id = system_frame_id & 0xFF;  // modulo of MAX_NUM_OF_F - 1
			}
		}

		if(	(packet_frame_id == system_frame_id) &&
			(packet_subframe_id == system_subframe_id) &&
//			( packet_slot_id == ( (system_slot_id + 1) % MAX_NUM_SLOT_PER_FRAME) )
			( packet_slot_id == system_slot_id )
			)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}

	return result;
}


timespec ClassifiedQueue::GetTimestampFromMbuf(rte_mbuf* mbuf)
{
	ASSERT( (nullptr != mbuf) && (nullptr != mbuf->buf_addr) );

	timespec time_stamp;
#ifdef PCAP_HEADER_MODE
	//	Parse PCAP header from retrieved data:
	struct pcap_pkthdr *pcap_header;
	pcap_header = reinterpret_cast<pcap_pkthdr*>(mbuf->buf_addr);
	time_stamp.tv_nsec=pcap_header->ts.tv_usec*1000;
	time_stamp.tv_sec=pcap_header->ts.tv_sec;
	return time_stamp;
#else
	//	Parse OfflineDataHeader from retrieved data:
	OfflineDataHeader* offline_data_header = reinterpret_cast<OfflineDataHeader*>(mbuf->buf_addr);
	return offline_data_header->timestamp;
#endif
}


void ClassifiedQueue::InitPendingQueue()
{
	pending_queue_.pending_packet_ptr_ = nullptr;
	pending_queue_.pending_packet_timestamp_ = {0,0};
}


/******************************************************************
 *@function	AreAllQueuesEmpty
 *@brief	This function returns true in case all ingress queues
 *@brief	are empty or false otherwise.
 *@brief
 *@brief	Input:	None
 *@brief
 *@brief	Output:	True  - all queues are empty
 *@brief			False - otherwise
 *@brief
 *******************************************************************/
bool ClassifiedQueue::AreAllQueuesEmpty()
{
	bool result = false;
	uint32_t num_of_active_queues = 0;
	uint32_t num_of_active_empty_queues  = 0;
	int32_t start_queue_id;

	start_queue_id = INGRESS_NON_CLASSIFIED_Q;

	//	Count how many active queues and empty queues are there:
	for(int32_t i=start_queue_id; i < MAX_INGRESS_QUEUE_SIZE; i++)
	{
		//	Check if the current queue is active:
		if(true == is_active_queue_flag_array_ptr_[i])
		{
			num_of_active_queues++;
			//	Check if the current queue is also empty:
			if(true == is_empty_queue_flag_array_ptr_[i])
			{
				num_of_active_empty_queues++;
			}
		}
	}

	if(num_of_active_queues == num_of_active_empty_queues)
	{
		//	If the amount of active queues is equal to the amount of active and empty
		//	queues, it means that all the queues are empty:
		result = true;
	}

	return result;
}


/******************************************************************
 *@function	ConvertTimestampInNsec
 *@brief	This function converts timestamp from timespec type to
 *@brief	uint64_t (time in nano-seconds). If offset_ts is sent,
 *@brief	it adds its value to the result.
 *@brief
 *@brief	Input:	ts - pointer to timepsec
 *@brief			offset_ts (not mandatory) - pointer to an offset
 *@brief				timespec
 *@brief
 *@brief	Output:	representation of ts (or ts+offset_ts) in
 *@brief			nano-sec.
 *@brief
 *******************************************************************/
uint64_t ClassifiedQueue::ConvertTimestampInNsec(timespec* ts, timespec* offset_ts)
{
	uint64_t ts_in_nsec = ts->tv_sec*BILLION + ts->tv_nsec;
	if(nullptr != offset_ts)
	{
		uint64_t offset_in_nsec = offset_ts->tv_sec*BILLION + offset_ts->tv_nsec;
		ts_in_nsec += offset_in_nsec;
	}

	return ts_in_nsec;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												EgressQueue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


EgressQueue::EgressQueue()
{
	egress_queue_id_ = LAST_EGRESS_QUEUE;
}


/******************************************************************
 *@brief	This function initializes members of a DPDK
 *@brief	Ethernet device Egress queue.
 *@brief
 *@brief	Input:	callback_ptr - pointer to the callback pointer of
 *@brief				the DPDK Driver factory module.
 *@brief			id - Egress queue ID
 *@brief			test_module_api_ptr - pointer to Test module API.
 *@brief			current_system_time_ptr - pointer to current system
 *@brief				time of the DPDK Driver factory module.
 *@brief			tx_pool_write_index_ptr - pointer to tx memory pool's
 *@brief				current available index to be written
 *@brief			tx_pool_ptr = pointer to tx memory pool
 *@brief
 *@brief	Output:	None.
 *@brief
 *******************************************************************/
void EgressQueue::Init(IModuleControlCallBackAPI* callback_ptr,
		EGRESS_Q_TYPE id,
		TestModuleAPI* test_module_api_ptr,
		timespec* current_system_time_ptr,
		DpdkTestModuleType* test_module_type_ptr,
		int32_t* tx_pool_write_index_ptr,
		uint8_t tx_pool_ptr[MAX_TX_MBUF_IN_POOL][DPDK_UPLANE_MTU_SIZE],
		CProfileCnt* dl_dpdk_send_prof)
{
	InitQueue(callback_ptr,
			test_module_api_ptr,
			current_system_time_ptr,
			test_module_type_ptr);
	tx_pool_write_index_ptr_ = tx_pool_write_index_ptr;
	tx_pool_ptr_ = tx_pool_ptr;
	egress_queue_id_ = id;
	dl_dpdk_send_prof_ = dl_dpdk_send_prof;
}


void EgressQueue::Reset()
{

}


/******************************************************************
 *@brief	This function sends an mbuf packet to a Test module's
 *@brief	correct Egress queue.
 *@brief	It returns E_OK in case the packet was sent successfully, or
 *@brief	E_FAIL otherwise.
 *@brief
 *@brief	Note:
 *@brief	* In case of sending data to FPGA simulator, pkt points
 *@brief	to a chain of indirect mbufs. Each one of them points to
 *@brief	a different data section.
 *@brief	* In case of sending data to GL Test (FrontHaul Simulator),
 *@brief	pkt points to a chain of direct mbufs that contains an
 *@brief	ORAN packet.
 *@brief
 *@brief	Input:	pkt - pointer to rte_mbuf
 *@brief			free_unsent - True:  frees the the mbuf if it
 *@brief								 wasn't sent.
 *@brief						  False: does nothing to the mbuf.
 *@brief						  [Since this is an offline implementation
 *@brief						  there is no reference to this parameter]
 *@brief
 *@brief	Output: EResultT
 *@brief
 *******************************************************************/
EResultT EgressQueue::send(rte_mbuf* pkt, bool free_unsent)
{
#ifdef PROFILING_TEST
	dl_dpdk_send_prof_[0].Start();
#endif
	rte_mbuf* curr_mbuf;
	rte_mbuf* head_mbuf;
	rte_mbuf* prev_mbuf;
	uint8_t* destination_memory_ptr;
	uint8_t* curr_section_ptr;
	uint8_t* curr_section_ptr1;
	int32_t curr_section_length;
	uint8_t* start_pos_in_memory_ptr = nullptr;
	uint8_t* curr_pos_in_memory_ptr = nullptr;
	OfflineDataHeader offline_data_header;
	struct pcap_pkthdr pcap_header;
	//	TODO: future work - get mbuf from buffer_manager
	//	Get free memory from pool:
	destination_memory_ptr = &tx_pool_ptr_[*tx_pool_write_index_ptr_][0];
	*tx_pool_write_index_ptr_ = (*tx_pool_write_index_ptr_ + 1) % MAX_TX_MBUF_IN_POOL;
	start_pos_in_memory_ptr = static_cast<uint8_t*>(destination_memory_ptr);
	curr_pos_in_memory_ptr = start_pos_in_memory_ptr;
#ifdef PCAP_HEADER_MODE
	pcap_header.len=0;
	pcap_header.caplen=0;
	pcap_header.ts.tv_sec=0;
	pcap_header.ts.tv_usec=0;
	//	Leave space in beginning of the memory for PCAP header:
	curr_pos_in_memory_ptr += sizeof(pcap_pkthdr);
#else
	offline_data_header.data_length = 0;
	//	Leave space in beginning of the memory for OfflineDataHeader:
	curr_pos_in_memory_ptr += sizeof(OfflineDataHeader);
#endif


	//	Go through all mbufs, and copy their data
	//	into a single memory area:
	curr_mbuf = pkt;
	head_mbuf=pkt;
	do
	{
		curr_section_length = curr_mbuf->data_len;
		//curr_section_ptr = static_cast<uint8_t*>((uint64_t)(curr_mbuf->buf_addr) + (uint64_t)(curr_mbuf->data_off));
		curr_section_ptr = (uint8_t*)((uint64_t)(curr_mbuf->buf_addr) + (uint64_t)(curr_mbuf->data_off));
		memcpy(curr_pos_in_memory_ptr, curr_section_ptr, curr_section_length);
		curr_pos_in_memory_ptr += curr_section_length;
#ifdef PCAP_HEADER_MODE
		pcap_header.len += curr_section_length;	//	Update data_length
		pcap_header.caplen+=curr_section_length;
#else
		offline_data_header.data_length += curr_section_length;	//	Update data_length
#endif
		prev_mbuf=curr_mbuf;
		curr_mbuf=curr_mbuf->next;
	}while(nullptr != curr_mbuf);
	//Release the mbuf
	rte_pktmbuf_free(head_mbuf);
#ifdef PCAP_HEADER_MODE
	//	Write PCAP header at beginning of the memory:
	memcpy(start_pos_in_memory_ptr, &pcap_header, sizeof(pcap_header) );
#else
	//	Write OfflineDataHeader at beginning of the memory:
	memcpy(start_pos_in_memory_ptr, &offline_data_header, sizeof(offline_data_header) );
#endif
#ifdef PROFILING_TEST
	dl_dpdk_send_prof_[0].Stop();
#endif
	//	Push the pointer to the memory into Test Module's FIFO:
#ifndef RU_RT_MODE
	EResultT ret_val = test_module_api_p_->SendPacket(egress_queue_id_, start_pos_in_memory_ptr);
#else
	//	In case we are in RU_RT_MODE, there is no actual meaning to the DPDK offline driver.
	//	In order to support the DPDK offline driver, we simply skip the actual "send" operation
	//	and return E_OK.
	EResultT ret_val = E_OK;
#endif
	return ret_val;
}


/******************************************************************
 *@brief	This function sends a burst of "count" mbuf packets to
 *@brief	the correct Egress queue of a TestModuleAPI.
 *@brief	It returns the number of actual sent mbufs.
 *@brief
 *@brief	Input:	tx_pkts - mbuf pointers array to send.
 *@brief					  The *tx_pkts is freed by send() for sent packets.
 *@brief			count -   mbufs count to send
 *@brief			free_unsent - True:  frees the the mbuf if it
 *@brief								 wasn't sent.
 *@brief						  False: does nothing to the mbuf.
 *@brief						  [Since this is an offline implementation
 *@brief						  there is no reference to this parameter]
 *@brief
 *@brief	Output:	Number of sent mbufs
 *@brief
 *******************************************************************/
uint16_t EgressQueue::sendBurst(rte_mbuf** tx_pkts, uint16_t count, bool free_unsent)
{
	EResultT ret_val = E_OK;
	uint16_t sent_packets_counter = 0;

	for(int32_t i=0; ( (i<count)  &&  (E_OK==ret_val) ); i++)
	{
		ret_val = send(tx_pkts[i], free_unsent);
	//	ASSERT(ret_val);
		sent_packets_counter++;
	}

	return sent_packets_counter;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												CDPDK_Eth_driver
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//	ctor
CDPDK_Eth_driver::CDPDK_Eth_driver()
{
    //Reset();
    callback_ptr_ = nullptr;
    dev_id_= -1;
    current_system_time_ptr_ = nullptr;
}


//	dtor
CDPDK_Eth_driver::~CDPDK_Eth_driver()
{

}


/******************************************************************
 *@brief	This function initializes members of a DPDK
 *@brief	Ethernet device. Each queue is initialized separately.
 *@brief
 *@brief	Input:	callback_ptr - pointer to the callback pointer of
 *@brief				the DPDK Driver factory module.
 *@brief			id - Egress queue ID
 *@brief			gl_test_api_ptr - pointer to GL Test module.
 *@brief			current_system_time_ptr - pointer to current system
 *@brief				time of the DPDK Driver factory module.
 *@brief
 *@brief	Output:	None.
 *@brief
 *******************************************************************/
void CDPDK_Eth_driver::Init(IModuleControlCallBackAPI* ptr,
		uint32_t id,
		TestModuleAPI* test_module_api_ptr,
		timespec* current_system_time_ptr,
		timespec* start_time_ptr,
		timespec* offset_time_ptr,
		DpdkTestModuleType test_module_type,
		DpdkDriverClockMode* clock_mode_ptr,
		CMemArea* m_frontend_check_test_out,
		CProfileCnt* DL_dpdk_send_Prof,
		ISplitAPI* oran_module_ptr)
{
	EResultT retValue = E_OK;
	callback_ptr_= ptr;
	dev_id_ = id;
	test_module_type_ = test_module_type;
	clock_mode_ptr_ = clock_mode_ptr;
	is_first_rx_packet_ = true;   //^^^^^
	m_frontend_check_test_out_ = m_frontend_check_test_out;
	first_packet_=true;
	//Take an instance of DPDK Buffer Manager (for further mempool allocation and being able to allocate mbufs from it)
	char dpdk_buffer_manager_mod_name[]= "DPDK_BUFFER_MANAGER_OFFLINE";
	ASSERT(callback_ptr_->IGetModule(dpdk_buffer_manager_mod_name, &m_dpdk_buffer_manager_p_) == E_OK);
	void *buffer_manager_api;
	m_dpdk_buffer_manager_p_->IConfigure(e_GET_API, NULL, &buffer_manager_api);

	dpdk_buffer_manager_api_ = (Dpdk_Buffer_Manager_API*)buffer_manager_api;

	// Initiate Mempools settings
	buffer_manager_settings_t bm_settings;
	DpdkOfflineInitMempoolsSettings(&bm_settings);
	// call to DPDK Buffer Manager Init - initiates all the mempools
	retValue = dpdk_buffer_manager_api_->Init(&bm_settings);
	ASSERT(E_OK == retValue);
	
	//	At initialization, the queues are not filled with data yet.
	//	Therefore, the test is not ready:
	is_test_ready_flag_ = false;

	is_test_finished_flag_ = false;

	tx_pool_write_index_ = 0;
	memset(tx_memory_pool_, 0, sizeof(tx_memory_pool_));


	bool* pending_queue_flag_array_ptr;
	bool* is_empty_queue_flag_array_ptr;
    bool* is_active_queue_flag_array_ptr;
	if( (kGlueLogicTest  == test_module_type_)  ||
	    (kEmulatorLoader == test_module_type_) )
	{
		//	pending_queue_flag_array_ and is_empty_queue_flag_array_ is in use only in kFrontHaul devices:
		pending_queue_flag_array_ptr   = pending_queue_flag_array_;
		is_empty_queue_flag_array_ptr  = is_empty_queue_flag_array_;
		is_active_queue_flag_array_ptr = is_active_queue_flag_array_;
	}
	else
	{
		pending_queue_flag_array_ptr   = nullptr;
		is_empty_queue_flag_array_ptr  = nullptr;
		is_active_queue_flag_array_ptr = nullptr;
	}

	// Initializing Ingress queues:
	for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
	{
		INGRESS_CLASSIFICATOR_Q_TYPE ingrees_q_type = static_cast<INGRESS_CLASSIFICATOR_Q_TYPE>(i);
		rx_queues_[i].Init(callback_ptr_,
				id,
				ingrees_q_type, 
				test_module_api_ptr,
				current_system_time_ptr, 
				start_time_ptr,
				&last_test_time_,
				offset_time_ptr,
				pending_queue_flag_array_ptr, 
				is_empty_queue_flag_array_ptr,
				is_active_queue_flag_array_ptr,
				clock_mode_ptr_,
				&test_module_type_,
				&is_first_rx_packet_,
				&is_test_ready_flag_,
				&is_test_finished_flag_,
				m_frontend_check_test_out_,
				dpdk_buffer_manager_api_,
				DL_dpdk_send_Prof,
				oran_module_ptr);
	}

	// Initializing Egress queues:
	for(int32_t i=0; i<MAX_EGRESS_QUEUE_SIZE; i++)
	{
		EGRESS_Q_TYPE egrees_q_type = static_cast<EGRESS_Q_TYPE>(i);
		tx_queues_[i].Init(callback_ptr_,
				egrees_q_type, test_module_api_ptr,
				current_system_time_ptr, &test_module_type_,
				&tx_pool_write_index_,
				tx_memory_pool_, DL_dpdk_send_Prof);
	}

}


void CDPDK_Eth_driver::Reset()
{

	//	Initializing pending_queue_flag_array_ and is_empty_queue_flag_array_:
	for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
	{
		pending_queue_flag_array_[i]   = false;
		is_empty_queue_flag_array_[i]  = false;
		is_active_queue_flag_array_[i] = false;
	}
	//	At initialization, the queues are not filled with data yet.
	//	Therefore, the test is not ready:
	is_test_ready_flag_ = false;
	last_test_time_.tv_nsec = 0;
	last_test_time_.tv_sec = 1000000;
	is_first_rx_packet_ = true;  //^^^^^

	// Initializing Ingress queues:
	for(int32_t i=0; i<MAX_INGRESS_QUEUE_SIZE; i++)
	{
		INGRESS_CLASSIFICATOR_Q_TYPE ingrees_q_type = static_cast<INGRESS_CLASSIFICATOR_Q_TYPE>(i);
		rx_queues_[i].Reset();
	}
}


//DPDK device API
// Open port by port_id
// \param[in] port_id       Port id to initialize
// \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
// \return AS_INFRA_RETCODE_ROK or AS_INFRA_RETCODE_RFAILED
EResultT CDPDK_Eth_driver::open(uint16_t port_id, eth_dev_settings_t* dev_settings)
{
	return E_OK;
}


// Open port by MAC
// \param[in] mac           MAC address to initialize
// \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
// \return AS_INFRA_RETCODE_ROK or AS_INFRA_RETCODE_RFAILED
EResultT CDPDK_Eth_driver::open(const uint8_t mac[ETH_ADDR_LEN], eth_dev_settings_t* dev_settings )
{
	return E_OK;
}


// Close port
// \return AS_INFRA_RETCODE_ROK or AS_INFRA_RETCODE_RFAILED
EResultT CDPDK_Eth_driver::close()
{
	return E_OK;
}



// Check if ETH_LINK UP
// \return true if link up
bool CDPDK_Eth_driver::isLinkUp()
{
	return true;
}


// Busy wait till ETH_LINK UP using timeout
// \param[in] timeout_ms        Time to wait (ms)
// \param[in] check_period_ms   Check interval (ms)
// \return true if link up
bool CDPDK_Eth_driver::waitLinkUp(uint32_t timeout_ms, uint32_t check_period_ms)
{
	return true;
}


// Port id
uint16_t CDPDK_Eth_driver::getPortId()
{
	return 0;
}

EResultT CDPDK_Eth_driver::assignSoftwareClassifier (DpdkClassifyAPI *classifier_api, uint16_t classifier_burst_size)
{
    return E_OK;
}
    
EResultT CDPDK_Eth_driver::readAndClassification()
{
    return E_OK;
}

/******************************************************************
 *@brief	This function returns the pointer to the corresponding
 *@brief	classifier MBUF queue handler (Ingress Queue) or NULL
 *@brief	if it does not exist.
 *@brief
 *@brief	Input:	classified_q_val - enum that specify the type of
 *@brief				the ingress queue.
 *@brief
 *@brief	Output:	pointer to IngressClassifiedQueueAPI.
 *@brief
 *******************************************************************/
IngressClassifiedQueueAPI* CDPDK_Eth_driver::GetIngressClassifiedQueue( INGRESS_CLASSIFICATOR_Q_TYPE classified_q_val )
{
	ASSERT(classified_q_val < INGRESS_LAST_CLASSIFIED_QUEUE);
	return static_cast<IngressClassifiedQueueAPI*>(&rx_queues_[classified_q_val]);
}


/******************************************************************
 *@brief	This function returns the pointer to the corresponding
 *@brief	MBUF queue handler (Egress Queue) or NULL
 *@brief	if it does not exist.
 *@brief
 *@brief	Input:	classified_q_val - enum that specify the type of
 *@brief				the ingress queue.
 *@brief
 *@brief	Output:	pointer to IngressClassifiedQueueAPI.
 *@brief
 *******************************************************************/
EgressQueueAPI* CDPDK_Eth_driver::GetEgressQueue( EGRESS_Q_TYPE egress_q_val )
{
	ASSERT(egress_q_val < LAST_EGRESS_QUEUE);
	return static_cast<EgressQueueAPI*>(&tx_queues_[egress_q_val]);
}

// ============================= CDPDK_Eth_driver Private Methods ========================================
/******************************************************************
 *@brief	Function name: InitMempoolsSettings
 *@brief
 *@brief	Description: Private method. Initiates required mempools settings per whole Eth Driver
 *@brief				 for DPDK mempool init method.
 *@brief
 *@brief	Input:	buffer_manager_settings_t*
 *@brief	Output:	E_OK/E_FAIL
 *@brief
 *******************************************************************/
EResultT	CDPDK_Eth_driver::DpdkOfflineInitMempoolsSettings(buffer_manager_settings_t* dpdk_buffer_manager_settings)
{
	EResultT ret_value = E_OK;

	char pool_name[MAX_POOLNAME_LEN];
	char pool_ops_name[MAX_POOL_OPSNAME_LEN];

	dpdk_buffer_manager_settings->number_mempools	= TOTAL_NUMBER_MEMPOOLS_DPDK_OFFLINE_DRV;
	sprintf((char*)&dpdk_buffer_manager_settings->mempool_setup[0].pool_name[0], INPUT_PACKETS_MEMPOOL_NAME, dev_id_);
	dpdk_buffer_manager_settings->mempool_setup[0].buffers_number		= INPUT_PACKETS_NUM_BUFFERS;
	dpdk_buffer_manager_settings->mempool_setup[0].buffers_size			= INPUT_PACKETS_BUFF_SIZE;
	dpdk_buffer_manager_settings->mempool_setup[0].private_data_size	= INPUT_PACKETS_PRIVATE_DATA_SIZE;
	dpdk_buffer_manager_settings->mempool_setup[0].cache_size			= MEMPOOL_CACHE_SIZE_DPDK_OFFLINE_DRV;
	dpdk_buffer_manager_settings->mempool_setup[0].socket_id			= MEMPOOL_SOCKET_ID_DPDK_OFFLINE_DRV;
	sprintf((char*)&dpdk_buffer_manager_settings->mempool_setup[0].pool_ops_name[0], (const char*)POOL_OPS_NAME_DPDK_OFFLINE_DRV);

	return ret_value;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												CCommDriverFactory
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



EResultT CCommDriverFactory::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api)
{

	CBaseModule::IInit(callback_ptr, target_api);
    //Example of initialization of the profiler counter
    char prof_name[] = "COMM";
    comm_prof_.Init(prof_name, callback_ptr); //INIT Profiler counter
    LOG(E_MEDIUM,  "init passed");

    DEFINE_MEMAREA(4, sizeof(uint16_t), RU_CHECK_TEST_END, 0, m_frontend_check_test_out_);

    INIT_PROF_CNT("Offline DPDK Profiling: dpdk send", callback_ptr, dl_dpdk_send_prof_[0]);
    INIT_PROF_CNT("Offline DPDK Profiling: dpdk recv", callback_ptr, dl_dpdk_send_prof_[1]);
    
    return E_OK;
}


EResultT CCommDriverFactory::IColdStart()
{
	void *api_p;
	IModuleControlAPI  *module_api_p;
	char module_name[256];

	//	Connect to Emulator Loader module:
	sprintf(module_name, EMULATOR_LOADER_MODULE);
	ASSERT(callback_ptr_->IGetModule(module_name, &module_api_p) == E_OK);
	module_api_p->IConfigure(e_GET_TEST_MODULE_API, NULL, &api_p);
	emulator_loader_p_ = static_cast<TestModuleAPI*>(api_p);

	//	Connect to Pcap parser Test module:
	sprintf(module_name, "PCAP_PARSER");
	ASSERT(callback_ptr_->IGetModule(module_name, &module_api_p) == E_OK);
	module_api_p->IConfigure(e_GET_TEST_MODULE_API, NULL, &api_p);
	pcap_parser_p_ = static_cast<TestModuleAPI*>(api_p);
	
#ifndef RU_RT_MODE
	//	Connect to Glue Logic Test module:
	sprintf(module_name, "RU_HANDLER_TEST_MOD");
	ASSERT(callback_ptr_->IGetModule(module_name, &module_api_p) == E_OK);
	module_api_p->IConfigure(e_GET_TEST_MODULE_API, NULL, &api_p);
	glue_logic_test_p_ = static_cast<TestModuleAPI*>(api_p);

	//	Connect to FPGA Simulator module:
	sprintf(module_name, ANT_GROUP_POOL_MODULE);
	ASSERT(callback_ptr_->IGetModule(module_name, &module_api_p) == E_OK);
	module_api_p->IConfigure(e_GET_TEST_MODULE_API, NULL, &api_p);
	fpga_sim_p_ = static_cast<TestModuleAPI*>(api_p);

	MEMAREA_CONNECT(RU_CHECK_TEST_END, &m_frontend_check_test_out_, E_READ);
#endif

	MEMAREA_CONNECT(RU_CHECK_TEST_END, &m_frontend_check_test_out_, E_READ);

	//	Connect to ORAN module:
	sprintf(module_name, ORAN_SPLIT_HANDLER_MODULE);
	ASSERT(callback_ptr_->IGetModule(module_name, &module_api_p) == E_OK);
	module_api_p->IConfigure(e_GET_TEST_MODULE_API, NULL, &api_p);
	oran_module_p_ = static_cast<ISplitAPI*>(api_p);

	return E_OK;
}


EResultT CCommDriverFactory::IHotStart()
{

    for(uint16_t driver_cnt=0; driver_cnt<num_ip_drivers_; driver_cnt++)
    {
      	dpdk_eth_driver_[driver_cnt].Reset();
    }
    
	switch(clock_mode_)
	{
		case kRealTimeClock:
		{
			//	In case that the clock is a Real Time Clock - reset the start_system_time_ to current linux time:
			clock_gettime(CLOCK_MONOTONIC, &start_system_time_);
			break;
		}
		case kNonRealTimeClock:
		{
			//	In case that the clock is NOT a Real Time Clock - reset the start_system_time_ to 0:
			start_system_time_.tv_sec  = 0;
			start_system_time_.tv_nsec = 0;
			callback_ptr_->ISyncTime(nullptr, &start_system_time_);
			break;
		}
		case kSystemTimeClock:
		{
			//	In this case - do nothing. The system time is configured externally.
			break;
		}
		default:
		{
			ASSERT(0);	//	Unsupported type
		}
	}

	//	Update current_system_time_ to be start_system_time_ at initialization:
	current_system_time_ = start_system_time_;

	return E_OK;
}


EResultT CCommDriverFactory::IConfigure(uint32_t id, void *in, void **out)
{
	if(id==e_PUT_INIT_DATA)
	{
		char *add_info = (char*)in;
		if(strcmp(add_info, "tst") == 0)
		{
			//	Default value:
			clock_mode_ = kNonRealTimeClock;
		}
		else if(strcmp(add_info, "real_time_clock") == 0)
		{
			//	The clock mode is Real Time Clock (Linux clock):
			clock_mode_ = kRealTimeClock;
		}
		else if(strcmp(add_info, "non_real_time_clock") == 0)
		{
			//	The clock mode is NON Real Time Clock (time starts from 0):
			clock_mode_ = kNonRealTimeClock;
		}
		else if(strcmp(add_info, "system_time_clock") == 0)
		{
			//	The clock mode is System Time Clock (operates according to FrameID, SlotID, SymbolID):
			clock_mode_ = kSystemTimeClock;
		}
		else
		{
			//	Any other input is unsupported
			ASSERT(0);
		}

	    return E_OK;

	}
	
	DpdkTestModuleType* test_module_type = static_cast<DpdkTestModuleType*>(in);


	//	Determine which Test Module the driver will communicate with,
	//	according to the device type and the offline driver mode:
	TestModuleAPI* test_module_api;

	switch(*test_module_type)
	{
		case kGlueLogicTest:
		{
#ifdef RU_RT_MODE
			//	GL Test as test module is not supported in RU_RT_MODE:
			ASSERT(0);
#else
			//	FrontHaul is simulated by GL Test in offline mode:
			test_module_api = glue_logic_test_p_;
#endif
			break;
		}
		case kFpgaSimulator:
		{
#ifdef RU_RT_MODE
			//	In RU_RT_MODE the test module f the FPGA simulator is a stub:
			test_module_api = nullptr;
#else
			//	FPGA is simulated by FPGA simulator in offline mode:
			test_module_api = fpga_sim_p_;
#endif
			break;
		}
		case kEmulatorLoader:
		{
			//	Emulator loader generates traffic in offline mode:
			test_module_api = emulator_loader_p_;
			break;
		}
		default:
		{
			ASSERT(0);	//	Unsupported type
		}
	}
	ASSERT(clock_mode_ != kUndefinedTimeClock);

	//	Initialize driver instances:
	dpdk_eth_driver_[num_ip_drivers_].Init(callback_ptr_, 
			num_ip_drivers_, 
			test_module_api,
			&current_system_time_, 
			&start_system_time_,
			&offset_system_time_,
			*test_module_type,
			&clock_mode_,
			m_frontend_check_test_out_,
			dl_dpdk_send_prof_,
			oran_module_p_);
	*out = (void*)(static_cast<DpdkEthDeviceAPI*>(&(dpdk_eth_driver_[num_ip_drivers_])));

	num_ip_drivers_++;
	ASSERT(num_ip_drivers_ <= MAX_COMM_DPDK_DRIVERS);
	

	return E_OK;
}


EResultT CCommDriverFactory::IGetInfo(char* module_name, uint32_t *major_ver,
        uint32_t *minor_ver, uint32_t *build_num, char* add_info)
{
	if(clock_mode_ == kUndefinedTimeClock)
	{
		if(strcmp(add_info, "tst") == 0)
		{
			//	Default value:
			clock_mode_ = kNonRealTimeClock;
		}
		else if(strcmp(add_info, "real_time_clock") == 0)
		{
			//	The clock mode is Real Time Clock (Linux clock):
			clock_mode_ = kRealTimeClock;
		}
		else if(strcmp(add_info, "non_real_time_clock") == 0)
		{
			//	The clock mode is NON Real Time Clock (time starts from 0):
			clock_mode_ = kNonRealTimeClock;
		}
		else if(strcmp(add_info, "system_time_clock") == 0)
		{
			//	The clock mode is System Time Clock (operates according to FrameID, SlotID, SymbolID):
			clock_mode_ = kSystemTimeClock;
		}
		else
		{
			//	Any other input is unsupported
			//ASSERT(0);
		}
	}
    return CBaseModule::IGetInfo(module_name, major_ver, minor_ver, build_num, add_info);
}


class CCommDriverFactory     *g_module_ptr = NULL;


//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new CCommDriverFactory();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}
