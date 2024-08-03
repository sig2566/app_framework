/******************************************************************
 * i_ldpc_dec_api.h
 * Author: Balamourougan Vaithianathan
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_LDPC_DEC_API_H_
#define API_MODULE_API_I_LDPC_DEC_API_H_

#include "global_defines.inc"
#include "i_base_module_api.h"
#include "i_code_block_rx_main_api.h"
#include "i_code_block_rx_main_api.h"
/*******************************************************************************************//*!
*@class LDPC_DEC_API
*@brief The purpose of this class is :
*@brief API for LDPC decoder call.
***********************************************************************************************/

struct AVX2_LDPC_DEC_API
{
    void virtual avx2_LDPCDecoding(struct LdpcDecoder  *LdpcConfig, struct DeRateMatching *DeRmConf,int8_t *ldpcDec_in,uint8_t *ldpcDec_out,int32_t *numDecBits) = 0;

};
#endif /* API_MODULE_API_I_LDPC_DEC_API_H_ */
