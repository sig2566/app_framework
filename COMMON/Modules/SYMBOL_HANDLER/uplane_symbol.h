/******************************************************************
 * uplane_symbol.h
 * Author: E.Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef UPLANE_SYMBOL_H
#define UPLANE_SYMBOL_H

#include "i_api_common.h"
#include "i_main_ru_handler_api.h"
#include "i_split_api.h"
#include "ru_global_defines.h"
#include "i_ant_group_pool_api.h"
#include "i_symbols_handler_api.h"
#include "i_dpdk_buffer_manager.h"

#define	MAX_RX_MBUF_IN_POOL_UPLANE_SYMBOL		30000
#define	MAX_MBUF_IN_POOL_HEADRS_SYMBOL		255
using namespace ns_5g_phy;


#define		SYMBOL_RECEIVE_WINDOW_LENGTH		(3)	// currently defined in whole symbols number, later might be changed by any other time resolution

/******************************************************************************************//*!
 *@class CUplaneSymbol
 *@brief The purpose of this class is :
 *@brief Handle U-Plane OFDM Symbol entity, e.g. it's time scope, receiving window, etc.
 *********************************************************************************************/

class CUplaneSymbol: public uplane_symbols_api
{
public:
	//CTOR
	CUplaneSymbol();

	EResultT			UPlaneSectionsParser(int8_t* message_ptr);
	EResultT			UplaneSectionIdProcess(MESSAGE_DESCRIPTOR* message_descriptor, DATA_DESCRIPTOR* uplane_section_id_ptr);
	uint32_t			getSymbolNumber(){return symbol_number_;};
	UPLANE_SYMBOL_STATE		getSymbolState(){return symbol_state_;};
	EResultT			checkStateMachineOnSymbolWindowCloseTick(UPLANE_SYMBOL_STATE* symbol_state);
	SymbolRbPointers*	getRbsDataPointersTable(){return &symbol_rb_pointers_;};
	void				DecrementSymbolWindowLength();
	EResultT			InitSymbol(uint32_t symb_num, uint32_t left_rec_win_len);
	void				ResetSymbol();
	Complex*			CopyToRing(Complex* input_ptr, uint32_t Length);
	Complex*			CopyMbufPtr(Complex* input_ptr, uint32_t Length);
	static Complex 		zero_rb_data_[NUMBER_SC_IN_RB]; // should be filled with 0-values for using as a pointer to non-used SCs, etc.
	void*               SetMempoolPtr(rte_mempool* uplane_mempool);
	void*               SetRingPtr(rte_ring* uplane_ring);
	void				SetMainRuPtr(RU_Handler_API* main_ru_ptr);


private:

	SymbolRbPointers	symbol_rb_pointers_;
	uint32_t			symbol_number_;
	UPLANE_SYMBOL_STATE	symbol_state_;
	uint32_t			left_receive_window_length_; // currently in integer number of symbols
	rte_mempool* uplane_mempool_ptr_;
	rte_ring*   uplane_mbufs_preallocated_ring_;
	RU_Handler_API*		main_ru_ptr_;
};


#endif /* UPLANE_SYMBOL_H */
