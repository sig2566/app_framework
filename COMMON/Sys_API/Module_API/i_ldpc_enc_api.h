/******************************************************************
 * i_ldpc_enc_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_LDPC_ENC_API_H_
#define API_MODULE_API_I_LDPC_ENC_API_H_

/*******************************************************************************************//*!
*@class LDPC_ENC_API
*@brief The purpose of this class is :
*@brief API for LDPC encoder call.
***********************************************************************************************/

struct LDPC_ENC_API
{
    /******************/
    /*                */
    /* LDPC_encoding  */
    /*                */
    //****************************************************************************
    //
    //  Description: LDPC Encoding
    //  -----------
    //
    //  Input parameters:
    //  -----------------
    //  pinp_data            - ptr. to unsigned char input buffer, input data in length zc * 22
    //  p0ut_data            - ptr. to unsigned char output buffer, Output data in length zc * 46
    //  graph                - index of LDPC graph
    //  zc                  - lifting size
    //  i_Ls                 - set index
    //
    //  Return parameters:
    //  -----------------
    //  none
    //
    //****************************************************************************
    void virtual LDPCEncoding(uint8_t *pinp_data, uint8_t *pout_data, int32_t graph, int32_t zc, int32_t i_Ls) = 0;

};
#endif /* API_MODULE_API_I_LDPC_ENC_API_H_ */
