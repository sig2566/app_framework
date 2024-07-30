/******************************************************************
 * uplane_symbols_handler.h
 * Author: E.Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef UPLANE_SYMBOLS_HANDLER_H
#define UPLANE_SYMBOLS_HANDLER_H

#include "i_api_common.h"
#include "i_symbols_handler_api.h"
#include "ru_global_defines.h"
#include "uplane_symbol.h"
#include "i_ru_glue_logic_test_api.h"
#include "i_dpdk_eth_api.h"
#include "i_ru_test_module_api.h"
#include "i_dpdk_buffer_manager.h"
#include "i_networking_api.h"
#include "i_main_ru_handler_api.h"

using namespace ns_5g_phy;

#define CHAIN_ZEROS_SIZE (3)


// forward declarations
class CRuHandler;

/******************************************************************************************//*!
 *@class CUplaneSymbolsHandler
 *@brief The purpose of this class is :
 *@brief Pool of Symbols Objects
 *********************************************************************************************/

class CUplaneSymbolsHandler: public IModuleControlAPI, public uplane_symbols_handler_api
{
public:
	//==============   Common methods  ========================

	EResultT IInit(IModuleControlCallBackAPI *callback_ptr,
				ITarget *target_api) ;
	EResultT IColdStart() ;
	EResultT IWarmStart() ;
	EResultT IHotStart();
	EResultT RestartHot(SysTimeT* sys_time_init);
	EResultT IStop(ESeverityT severity);
	EResultT ICall(SysTimeT *sys_time_p, uint32_t param) ;
	EResultT IConfigure(uint32_t id, void *in, void **out);
	EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver,
	uint32_t *build_num, char* add_info) ;
	//CTOR
	CUplaneSymbolsHandler():bf_engines_num_(BF_ENGINES_NUMBER),
						current_active_symbol_(0xFFFF){};

	EResultT Init(
			//CMemArea* data_engines_jobdescriptors_fifo_arr,
			CMemArea** data_raw_buffer,
			IModuleControlCallBackAPI* callback_ptr);

	// need to be called after O-RAN U-Plane App.Layer parser
	EResultT UPlaneSymbolClassifier(MESSAGE_DESCRIPTOR* pMessageDesc, uplane_symbols_api** symbol_object_ptr);

	// return proper Symbol Object pointer from the symbol number
	uplane_symbols_api* getSymbolObjectPtr(uint32_t symbol_id);


	//CTestGlueLogic_api  *glue_logic_test_p_;

	// set proper symbol's object pointer to the table of pointers
	void setSymbolObjectPtr(uint32_t symbol_id, uplane_symbols_api* symbol_obj_ptr);

	// on a new Symbol's tick pass over all the Symbols in the Pool and check (and change if needed) their current status
	EResultT checkSymbolsStateMachine();

	// get an available FREE Symbol's object
	EResultT getAvailableFreeSymbolObject(uplane_symbols_api** symbol_object_ptr);

	void decrementSymbolsWindow();

	// open a new Symbol
	EResultT openNewSymbol(uint32_t current_symbol_RT);

	// get the Symbol Id that should pass the BF processing in RU DL or sorting processing in DU UL
	uint32_t getNextSymbolToHandle();

	//==============   RU methods  ========================
	EResultT ResetSymbolState();
	//EResultT BuildRawDataBuffer();
	void SetProfiler(CProfileCnt* dl_prof, CProfileCnt* dl_symbol_tick_prof,CProfileCnt* ul_prof,CProfileCnt* ul_symbol_tick_prof);
	void setRUsysVars(uint32_t* mtu_cpu_fpga_size_ptr, uint32_t* uplane_symbols_pool_capacity_ptr);

	//============== Build U-Plane Chain for send===========
	EResultT BuildMbufSendToChain(DataDirection direction);
	EResultT CreateMbufEth(rte_mbuf*&  pkt_eth,uint32_t layer_num_counter);
	EResultT PrepareHeaders(rte_mbuf* pkt_eth, uint32_t layer_num_counter);
	EResultT AddToMbufChain( rte_mbuf *new_node,rte_mbuf** last_packet_ptr);
	EResultT CreateChainEthHead(rte_mbuf** last_packet_ptr,rte_mbuf* pkt_eth,rte_mbuf*& pkt_chain_head);
	void PushZerosToChain(uint32_t size_of_data ,rte_mbuf** last_packet_ptr,rte_mbuf* pkt_chain_head);
	void InitZerosVector(Complex zeors_vector[], uint32_t size_of_zeros_data);
	EResultT ResetNewMbufChain(rte_mbuf** last_packet_ptr,rte_mbuf* pkt_eth,rte_mbuf*& pkt_chain_head,uint32_t layer_num_counter);
	EResultT SpearteSectionPerPerMtu(rte_mbuf* pkt_section,rte_mbuf** last_packet_ptr,rte_mbuf*& pkt_chain_head,uint32_t layer_num_counter,EgressQueueAPI* qeue,uint32_t chain_size, DataDirection direction);
	EResultT BuildNewUplaneDataMbuf(rte_mbuf** last_packet_ptr, Complex* start_data_ptr, rte_mbuf*&pkt_send,uint32_t data_size_for_send,rte_mbuf*pkt_chain_head,rte_mbuf* pkt_section);
	EResultT SetDpdkDriverFpgaDlPtr (void* dpdk_eth_driver_fpga_api);

	//==============   DU methods  ========================

	EResultT InitDu();

private:
	//==============   Common private  ========================

	CUplaneSymbol	dl_symbol_[MAX_UPLANE_SYMBOLS_POOL_CAPACITY];
	uint8_t		last_opened_symbol_number_;
	uint32_t	current_active_symbol_;
	uint32_t	current_active_symbol_RT_;

	uplane_symbols_api*	active_symbols_objects_ptrs_[MAX_NUM_OFDM_PER_SLOT];
	uint16_t	sqid_; // for CPU-to-FPGA messages (like RBS field)

	//==============   RU private  ========================

	CRuHandler*	ru_handler_ptr_;
	uint32_t	bf_engines_num_;
	//MemArea*	data_engines_job_descs_send_;	//Job Descriptors FIFO for Jobs sending to Data Engines

	CMemArea*	m_ru_raw_data__send_[MAX_DL_DATA_STREAMS_PER_RB];
	//IModuleControlAPI   *ant_pool_module_p_;
	IModuleControlCallBackAPI* callback_ptr_;

	bool RU_ = true;

	CMemArea	*ant_pool_job_req_;
	CProfileCnt	prof_ant_bf_;

	RU_Handler_API*	main_ru_handler_module_ptr_;
	//DPDK driver module connection
	IModuleControlAPI* m_dpdk_eth_driver_offline_factory_p_;
	IModuleControlAPI* m_dpdk_eth_driver_online_factory_p_;

	//Dpdk connection
	DpdkEthDeviceAPI* dpdk_eth_driver_fpga_dl_api_;
	DpdkEthDeviceAPI* dpdk_eth_driver_fpga_ul_api_;

	//Networking module connection
	IModuleControlAPI* m_networking_p_;
	RU_Networking_API* main_networking_api_;

	//Mbuf Allocate
	int32_t* mbuf_pool_write_index_ptr_;
	rte_mbuf* rx_mbufs_pool_ptr_;
	//	This is temporary until buffer manager will be completed:
	int32_t  mbuf_pool_write_index_;
	rte_mbuf rx_mbufs_pool_[MAX_RX_MBUF_IN_POOL_UPLANE_SYMBOL];
	uint8_t memory_pool_[MAX_RX_MBUF_IN_POOL_UPLANE_SYMBOL][DPDK_UPLANE_MTU_SIZE];

	rte_mbuf *pkt_zeors_data_;

	//pointers to mempool's
	rte_mempool* uplane_mempool_;
	rte_mempool* eth_headers_mempool_;
	rte_mempool* zeros_mempool_;

	//pointer to Ethernet header mbuf ring
	rte_ring*   eth_haders_mbufs_preallocated_ring_;
	rte_ring*   uplane_mbufs_preallocated_ring_;

	//array and counter for U-Plane send burst
	rte_mbuf* chain_heads_to_send_[UPLANE_DL_SIZE];
	uint32_t chain_head_counter_=0;

	// Profilers
    CProfileCnt* dl_prof_;
    CProfileCnt* ul_prof_;
    CProfileCnt* dl_symbol_tick_prof_;
    CProfileCnt* ul_symbol_tick_prof_;

    // Traces
    uint32_t	send_burst_trace_id_ = -1;
    
    uint32_t mtu_cpu_fpga_size_;
    uint32_t uplane_symbols_pool_capacity_;

#if 0
	EResultT	sendJobDescriptorToBfDe(uplane_symbols_api* symbol_obj_ptr,
			CMemArea* de_fifo_pointer);
#endif
	//Raw Buffer for Uplane

};

#endif /* UPLANE_SYMBOLS_HANDLER_H */
