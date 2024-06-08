/******************************************************************
 * i_const_data_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_CONST_DATA_API_H_
#define API_MODULE_API_I_CONST_DATA_API_H_

#include "i_api_common.h"
/*******************************************************************************************//*!
*@class CRC_gen_call_api
*@brief The purpose of this class is :
*@brief Interface for access to the common large constant tables
***********************************************************************************************/
#define CONST_DATA_MODULE "CONST_DATA"

struct CConst_Data_api
{
     virtual const int16_t*    get_INV_FULL_LUT(uint32_t *size )=0;
     virtual const uint32_t*   get_exp_table(uint32_t *size ) =0;
     virtual const uint32_t*   get_inv_32bit_table(uint32_t *size ) =0;
     virtual const uint32_t*   get_inv_32bit_table_Q17(uint32_t *size ) =0;
     virtual const int16_t*    get_inv_table(uint32_t *size )= 0;
     virtual const int16_t*    get_ANGLE_LUT(uint32_t *size ) = 0;
     virtual const int16_t*    get_inv_2x_table(uint32_t *size ) = 0;
     virtual const float*	   get_MCS_to_R_table_1(uint32_t *size) = 0;		//return pointer to the beginning of the struct and his length
     virtual const float*	   get_MCS_to_R_table_2(uint32_t *size) = 0;		//return pointer to the beginning of the struct and his length
     virtual const float*	   get_MCS_to_Qm_table_1(uint32_t *size) = 0;		//return pointer to the beginning of the struct and his length
     virtual const float*	   get_MCS_to_Qm_table_2(uint32_t *size) = 0;		//return pointer to the beginning of the struct and his length
     virtual const uint16_t*	   get_T_TBS_Table(uint32_t *size) = 0;
};


#endif /* API_MODULE_API_I_CONST_DATA_API_H_ */
