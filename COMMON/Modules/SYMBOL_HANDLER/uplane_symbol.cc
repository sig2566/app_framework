/******************************************************************
 * uplane_symbol.cc
 * Author: E.Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include <stdio.h>
#include <string.h>
#include "ru_global_defines.h"
#include "uplane_symbol.h"

// Init static zero RB vector for all Symbols Objects
Complex CUplaneSymbol::zero_rb_data_[NUMBER_SC_IN_RB] = {0};

//CTOR
CUplaneSymbol::CUplaneSymbol()
{
	ResetSymbol();
}

void CUplaneSymbol::ResetSymbol()
{
	symbol_number_				= 0xFFFF;
	symbol_state_				= SYMBOL_FREE;
	left_receive_window_length_	= 0;
}


/******************************************************************
 *@brief  Function Name: UPlaneSectionsParser
 *@brief  Desc:	need to be called after U-Plane Symbol Classifier for getting all the U-plane SectionIds per this Symbol
 *@brief
 *@brief  Input:	message_ptr - pointer to a new U-plane O-RAN message
 *@brief  Output:
 *@brief
 *******************************************************************/
EResultT CUplaneSymbol::UPlaneSectionsParser(int8_t* message_ptr)
{
	EResultT		ret_value = E_OK;
	DATA_DESCRIPTOR	new_section_id_struct;
	MESSAGE_DESCRIPTOR message_descriptor;

	// make here loop of calling to a new U-Plane SectionId:
	// loop-start
	UplaneSectionIdProcess(&message_descriptor, &new_section_id_struct);
	// loop-end

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: UplaneSectionIdProcess
 *@brief  Desc:	save the received data to the O-RAN Msg Buffer Ring
 *@brief		saving the Ring's pointer of the data on SymbolRbPointers database
 *@brief
 *@brief  Input:	uplane_section_id_ptr - pointer to DATA_DESCRIPTOR structure passed by reference to get a new SectionId structure on it
 *@brief  Output:
 *@brief
 *******************************************************************/
EResultT CUplaneSymbol::UplaneSectionIdProcess(MESSAGE_DESCRIPTOR* message_descriptor, DATA_DESCRIPTOR* uplane_section_id_ptr)
{
	EResultT		ret_value = E_OK;

	Complex* buffer_mbuf_ptr;
	Complex* data_for_mbuf;
	int32_t rb_idx = uplane_section_id_ptr->start_rb_;
	uint16_t data_size;
    //Complex* mbuf_data_ptr;


    // copy pointer to data start in the mbuf buffer
	buffer_mbuf_ptr = CopyMbufPtr(uplane_section_id_ptr->iq_samples_, uplane_section_id_ptr->num_of_rbs_ * NUMBER_SC_IN_RB);

	//need to send the iq_samples to a buffer that we can compare to the results in the PCAP module

	rte_mbuf* incoming_mbuf = (rte_mbuf*)(main_ru_ptr_->getCurrentMbufPointer());

	uint8_t* original_buf_ptr = (uint8_t*)((uint64_t)(incoming_mbuf->buf_addr) + (uint64_t)(incoming_mbuf->data_off)); // start of the input data
	uint32_t section_offset = (uint64_t)buffer_mbuf_ptr - (uint64_t)original_buf_ptr; // calculate offset of the current section relatively to the start of the packet
	//	Get free mbuf from ring:
	rte_mbuf *pkt ;
	uint32_t retVal = rte_ring_dequeue(uplane_mbufs_preallocated_ring_, (void **)&pkt);
	ASSERT(retVal==0)
	//clean mbuf before reuse
	pkt->next=nullptr;
	pkt->nb_segs=1;
	pkt->data_off=0;
	pkt->ol_flags=0;
	pkt->buf_len=0;
	pkt->refcnt=1;

	// increment refcnt of the mbuf for using it again without allocation from mempool, but taking it from our preallocated Ring array
	pkt->refcnt++;
	//need to free the next, beacuse the eth header is part of a chain, and when we take this mbuf again we need that
	// the mbuf will be clean.
	// Enqueue the mbuf ptr back to the ring for next usage.
	// We expect that the Ring will be big enough so that when we will use this mbuf again
	// it already should be released by the DPDK Offline Driver (in case of Offline run) or by HW (in case of Online DPDK Driver usage on Send)
	retVal = rte_ring_enqueue(uplane_mbufs_preallocated_ring_, (void *)(pkt));
	ASSERT(retVal==0)
	// make the indirect mbuf point to the incoming and adjust its offset
	rte_pktmbuf_attach(pkt, incoming_mbuf);

    if (rte_pktmbuf_adj(pkt, section_offset ) == NULL )
    {
    	// if attachment has failed
        rte_pktmbuf_free(pkt);
    }
#ifndef FULL_BYPASS_MODE_DL
//rb_idx+RB_OFFSET_BH_FH - Because FH rbs number is small than BH RB'S number and we need to move all the RB'S by the offset.
	pkt->buf_addr = incoming_mbuf->buf_addr;
	pkt->pkt_len = pkt->data_len = uplane_section_id_ptr->num_of_rbs_*sizeof(Complex)*NUMBER_SC_IN_RB; //update the data length in the mbuf
	symbol_rb_pointers_.single_rb_data_pointers[message_descriptor->stream_id_][rb_idx+RB_OFFSET_BH_FH].pkt_data=pkt;
	symbol_rb_pointers_.single_rb_data_pointers[message_descriptor->stream_id_][rb_idx+RB_OFFSET_BH_FH].run_length_= uplane_section_id_ptr->num_of_rbs_;

#else 	// the following code is currently for Full Bypass mode only, but it can be our operative code in non-full BP mode as well, when we will support run-length feature on the regular BF engine mechanism
	 pkt->buf_addr = incoming_mbuf->buf_addr;
	 pkt->pkt_len = pkt->data_len=uplane_section_id_ptr->num_of_rbs_*sizeof(Complex)*NUMBER_SC_IN_RB;
	 symbol_rb_pointers_.single_rb_data_pointers[message_descriptor->stream_id_][rb_idx+RB_OFFSET_BH_FH].pkt_data=pkt;
	 symbol_rb_pointers_.single_rb_data_pointers[message_descriptor->stream_id_][rb_idx+RB_OFFSET_BH_FH].rb_data_ptr_= buffer_mbuf_ptr;
	 symbol_rb_pointers_.single_rb_data_pointers[message_descriptor->stream_id_][rb_idx+RB_OFFSET_BH_FH].run_length_    = uplane_section_id_ptr->num_of_rbs_;
#endif

	return ret_value;
}



/******************************************************************
 *@brief  Function Name: CopyMbufPtr
 *@brief  Desc: copy the pointer
 *@brief
 *@brief
 *@brief  Input: input_ptr, Length - total number of sub-carriers
 *@brief  Output: buffer_ring_ptr
 *@brief
 *******************************************************************/
Complex* CUplaneSymbol::CopyMbufPtr(Complex* input_ptr, uint32_t Length)
{
	Complex* buffer_ring_ptr = input_ptr;

    return buffer_ring_ptr;
}

/******************************************************************
 *@brief  Function Name: CopyToRing
 *@brief  Desc: copy the pointer
 *@brief
 *@brief
 *@brief  Input: input_ptr, Length - total number of sub-carriers
 *@brief  Output: buffer_ring_ptr
 *@brief
 *******************************************************************/
Complex* CUplaneSymbol::CopyToRing(Complex* input_ptr, uint32_t Length)
{
	Complex* buffer_ring_ptr = input_ptr;

    return buffer_ring_ptr;
}
/******************************************************************
 *@brief  Function Name: checkStateMachineOnSymbolWindowCloseTick
 *@brief  Desc:	Decrement symbols counter of the current state, close the receiving window if needed
 *@brief  		Move the Symbol with left_receve_window_length = 0 to BF state
 *@brief
 *@brief
 *@brief
 *@brief  Input:	*symbol_state - symbol state pointer passed by reference
 *@brief
 *@brief
 *@brief
 *@brief  Output:	symbol_state - current Symbol's state
 *@brief
 *******************************************************************/
EResultT CUplaneSymbol::checkStateMachineOnSymbolWindowCloseTick(UPLANE_SYMBOL_STATE* symbol_state)
{
	EResultT		ret_value = E_OK;

	//decrement the symbols counter, close the receive window if needed and change the state (if needed as well)
	//left_receive_window_length_--;

	if(left_receive_window_length_ == 0)
	{
		*symbol_state = SYMBOL_ACTIVE;
		symbol_state_ = SYMBOL_ACTIVE;
	}

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: DecrementSymbolWindowLength
 *@brief  Desc:	Decrement symbols counter of the current state,
 *@brief
 *@brief  Input:
 *@brief
 *@brief  Output:
 *@brief
 *******************************************************************/
void CUplaneSymbol::DecrementSymbolWindowLength()
{
	//decrement the symbols counter, close the receive window if needed and change the state (if needed as well)
	if(SYMBOL_RECEIVE == symbol_state_)
		left_receive_window_length_--;

}


/******************************************************************
 *@brief  Function Name: InitSymbol
 *@brief  Desc: initialize a symbol
 *@brief
 *@brief
 *@brief
 *@brief  Input:
 *@brief
 *@brief
 *@brief
 *@brief  Output:
 *@brief
 *******************************************************************/
EResultT CUplaneSymbol::InitSymbol(uint32_t symb_num, uint32_t left_rec_win_len)
{
	EResultT		ret_value = E_OK;
	memset(&symbol_rb_pointers_, 0, sizeof(SymbolRbPointers));
	symbol_number_ = symb_num;

	symbol_state_ = SYMBOL_RECEIVE;

	left_receive_window_length_ = left_rec_win_len;
	return ret_value;
}
void* CUplaneSymbol::SetMempoolPtr(rte_mempool* uplane_mempool)
{
	uplane_mempool_ptr_=uplane_mempool;
}

void* CUplaneSymbol::SetRingPtr(rte_ring* uplane_ring)
{
	uplane_mbufs_preallocated_ring_=uplane_ring;
}

/******************************************************************
 *@brief  Function Name: SetMainRuPtr
 *@brief  Desc: Sets the Main RU API pointer to Symbol Handler for future usage
 *@brief
 *@brief
 *@brief
 *@brief  Input:
 *@brief
 *@brief
 *@brief
 *@brief  Output:
 *@brief
 *******************************************************************/
void CUplaneSymbol::SetMainRuPtr(RU_Handler_API* main_ru_ptr)
{
	main_ru_ptr_ = main_ru_ptr;
}


