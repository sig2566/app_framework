/******************************************************************
 * CRC_gen.h
 * Author: L. Livshits
 * Date : 26/4/2018
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef LIBRARY_CRC_GEN_H_
#define LIBRARY_CRC_GEN_H_
#include "i_api_common.h"
#include <i_const_data_api.h>
#include "i_base_module_api.h"
#include "inv_full_lut.h"
#include "lut_tables.h"
#include "angle_lut.h"
#include "inv_32bit_table.h"
#include "tb_size_calculation_const.h"
#include "inv_32bit_table_q17.h"



//Initialization data types

using namespace ai_framework_proj;

extern int16_t INV_FULL_LUT[];

/******************************************************************************************//*!
 *@class CModuleControl
 *@brief The purpose of this class is :
 *@brief It is example of the component implementation.
 *@brief In the case ofcreation new class only MANDATORY code must be copied. Other code is optional.
 *@brief Thet are used for demonstration of the framework services.
 *********************************************************************************************/
class CConstData : public CBaseModule, CConst_Data_api
{
    /***********************************************************************************/
  public:
    EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api);
    EResultT IConfigure(uint32_t id, void *in, void **out);
    const int16_t*    get_INV_FULL_LUT( uint32_t *size )
    {
        if(size!= NULL)
            *size = sizeof(INV_FULL_LUT)/sizeof(int16_t);
        return INV_FULL_LUT;
    }
    const uint32_t*   get_exp_table(uint32_t *size )
    {
        if(size!= NULL)
            *size = sizeof(exp_table)/sizeof(uint32_t);

        return exp_table;
    }
    const uint32_t*   get_inv_32bit_table(uint32_t *size )
    {
        if(size!= NULL)
            *size = sizeof(INV_32BIT_TABLE)/sizeof(uint32_t);

        return INV_32BIT_TABLE;
    }
    const uint32_t*   get_inv_32bit_table_Q17(uint32_t *size )
        {
            if(size!= NULL)
                *size = sizeof(INV_32BIT_TABLE_Q17)/sizeof(uint32_t);

            return INV_32BIT_TABLE_Q17;
        }
    const int16_t*    get_inv_table(uint32_t *size )
    {
        if(size!= NULL)
            *size = sizeof(inv_table)/sizeof(int16_t);

        return inv_table;
    }
    const int16_t*    get_ANGLE_LUT(uint32_t *size)
    {
        if(size!= NULL)
            *size = sizeof(ANGLE_LUT)/sizeof(int16_t);

        return ANGLE_LUT;
    }
    const int16_t*    get_inv_2x_table(uint32_t *size )
    {
        if(size!= NULL)
            *size = sizeof(inv_2x_table)/sizeof(int16_t);

        return inv_2x_table;

    }
    const float*	   get_MCS_to_R_table_1(uint32_t *size)
    {
    	if(size!=NULL)
    		*size = sizeof(MCS_to_R_table_1)/sizeof(float);

    	return MCS_to_R_table_1;
    }

    const float*	   get_MCS_to_R_table_2(uint32_t *size)
    {
		if(size!=NULL)
			*size = sizeof(MCS_to_R_table_2)/sizeof(float);

		return MCS_to_R_table_2;
	}

    const float*	   get_MCS_to_Qm_table_1(uint32_t *size)
    {
    	if(size!=NULL)
    		*size = sizeof(MCS_to_Qm_table_1)/sizeof(float);

    	return MCS_to_Qm_table_1;
    }

    const float*	   get_MCS_to_Qm_table_2(uint32_t *size)
    {
		if(size!=NULL)
			*size = sizeof(MCS_to_Qm_table_2)/sizeof(float);

		return MCS_to_Qm_table_2;
	}

    const uint16_t*	   get_T_TBS_Table(uint32_t *size)
       {
   		if(size!=NULL)
   			*size = sizeof(T_TBS_Table)/sizeof(uint16_t);

   		return T_TBS_Table;
   	}

    CConstData();
    //Library functions
 private:
};


#endif /* LIBRARY_CRC_GEN_H_ */
