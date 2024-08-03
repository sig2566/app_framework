/******************************************************************
 * i_tb_crc_detect_api.h
 * Author:Balamourougan Vaithianathan
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_TB_CRC_DETECT_API_H_
#define API_MODULE_API_I_TB_CRC_DETECT_API_H_


#include "i_code_block_rx_main_api.h"

/*******************************************************************************************//*!
*@class TB_CRC_DETECT_API
*@brief The purpose of this class is :
*@brief API for transport block CRC detector call
***********************************************************************************************/

struct TB_CRC_DETECT_API
{
    //****************************************************************************
	void virtual tb_CrcDetect(bool &check_error,uint8_t *crcIn,CbHandlerJobDescriptor *cb_jd) = 0;

};
#endif /* API_MODULE_API_I_SSE_CRC_DETECT_API_H_ */
