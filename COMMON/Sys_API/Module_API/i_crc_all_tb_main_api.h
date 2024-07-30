/*
 * i_code_block_main_api.h
 *
 *  Created on: May 3, 2018
 *      Author: dmatas
 */

#ifndef API_MODULE_API_I_CRC_ALL_TB_MAIN_API_H_
#define API_MODULE_API_I_CRC_ALL_TB_MAIN_API_H_

#include "fapi_p.h"
#include "i_crc_gen_api.h"

struct CRCAllTbMainIO
{
	int32_t 	num_tlv; //input-number of tlv for TB
	//int32_t offset_in_bits;  //offset of bits from start of the byte
	FAPI_P_T_TLV*	all_tb_ptr;  //Point to start of TB in case of last code-block - point to 1'st TLV
	CRC_poly	CRC_format;// Format of 24 for long TB_size or 16 for short
	int32_t		*CRC_output;

};


// Name of input FIFO for code_block_main
#define CRC_ALL_TB_MAIN_FIFO_IN   "CRC_ALL_TB_MAIN_FIFO_IN_JOB_REQ"

#endif /* API_MODULE_API_I_CRC_ALL_TB_MAIN_API_H_ */
