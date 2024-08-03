/******************************************************************
 * i_cb_crc_detect_api.h
 * Author:Balamourougan Vaithianathan
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_CB_CRC_DETECT_API_H_
#define API_MODULE_API_I_CB_CRC_DETECT_API_H_


#include "i_code_block_rx_main_api.h"

/*******************************************************************************************//*!
*@class CB_CRC_DETECT_API
*@brief The purpose of this class is :
*@brief API for code block CRC detector call
***********************************************************************************************/

struct CB_CRC_DETECT_API
{
    //****************************************************************************
	void virtual cb_crcdetect(bool &check_error,uint8_t *crcIn,int32_t numMsgBits) = 0;

};
#endif /* API_MODULE_API_I_CB_CRC_DETECT_API_H_ */
