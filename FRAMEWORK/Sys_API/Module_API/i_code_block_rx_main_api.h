/*
 * i_code_block_rx_main_api.h
 *
 *  Created on:June 15, 2019
 *      Author: Dikshant Chitkara
 */

#ifndef API_MODULE_API_I_CODE_BLOCK_RX_MAIN_API_H_
#define API_MODULE_API_I_CODE_BLOCK_RX_MAIN_API_H_

#include "i_slot_builder_api.h"

#define CB_HANDLER_RX_FIFO_IN "CB_HANDLER_RX_FIFO_IN_JOB_REQ"


// **********************************************************************************************
// *@struct CbHandlerJobDescriptor
// ********************************************************************************************

struct Descrambler
{
	uint8_t  *prbs_ptr;//Pointer to PRBS sequence per TB
	uint32_t  c_init;
	uint8_t 	 *llr_ptr; //Pointer to LLR data
	uint32_t  len; // Length of sequence to De-Scramble
	uint8_t  rem_bits;
};


struct DeRateMatching
{
	uint32_t	Qm; //Modulation order
	uint32_t	Er; //No. of Rate Matched Bits

	uint32_t 	Ncb;
	uint32_t	k0;
	uint32_t    K;
	uint32_t    K_tag;
};
struct LdpcDecoder
{
	uint32_t	K;
	uint32_t	K_tag;
	uint32_t  	Zc;
	uint32_t	N;
	uint32_t	Kb;
	uint32_t	i_Ls;
	uint32_t	LDPC_graph; //base graph

};

struct CbHandlerJobDescriptor
{
	uint8_t  *in;  //input-Pointer to TB data (TX_REQUEST) of relevant codeblock
	uint32_t cb_size;
	uint32_t num_codeblocks;
	uint32_t cb_index;
	uint8_t  *out_cb;  //out-Pointer to CodeBlock out
	uint32_t tbsize;
	struct Descrambler Descr;
	struct DeRateMatching Derate;
	struct LdpcDecoder LdpcDec;
};



#endif



