/******************************************************************
 * i_dmrs_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_DMRS_API_H_
#define API_MODULE_API_I_DMRS_API_H_

#include "fapi_p.h"
/*
//According to TS_38.211 p:7.4.1.1
enum EDL_DMRS_pos
{
    DMRS_POS0,
    DMRS_POS1,
    DMRS_POS2,
    DMRS_POS3,
    DMRS_POS_SIZE
};

enum EDL_DMRS_config_type
{
    DMRS_TYPE1 =1,
    DMRS_TYPE2 = 2
};

//PDSCH timing mapping definitions 38.214 - 5.1.2.1
//-------------------------------------------------------
enum PDSCH_time_mapping_type //mainly for DMRS , but has another usage
{
    PDSCH_TIME_ALLOCATION_TYPE_A = 0,
    PDSCH_TIME_ALLOCATION_TYPE_B = 1
};

enum EDmrsTypeAPos
{
    DMRS_TA_2 = 2,
    DMRS_TA_3 = 3
};


//<DMRS description='PDSCH Configuration'>
//    <DmrsConfigType description='Reference signal configuration type: 1,2'>1</DmrsConfigType>
//   <DmrsAddPos description='Reference signal add position: 0/1/2/3'>1</DmrsAddPos>
//    <DmrsMappingType description='Reference signal mapping type: A, B'>A</DmrsMappingType>
//    <DmrsTypeAPos description='Type A Position: 2 or 3'>3</DmrsTypeAPos>
//    <EnableDoubleSymbolDMRS description='Enable Double symbols DMRS'>0</EnableDoubleSymbolDMRS>
//</DMRS>

//DMRS configuration data 38.211-7.4.1.1
struct DMRS_config
{
    int32_t                 n_scid;   //Possible values 0,1 if it is -1, then scrambling ID data was not sent from L2
    uint32_t                n_id_scid;
    EDL_DMRS_pos            dmrs_add_pos;
    EDL_DMRS_config_type    dmrs_config_type;
    PDSCH_time_mapping_type dmrs_mapping_type;
    EDmrsTypeAPos           dmrs_type_a_pos;
    uint32_t                enable_double_symbol_dmrs;
};
*/
//Initialization data types
struct T_DMRS_ConfigType
{
    int32_t p;
    int32_t delta;
    int32_t wf_0;
    int32_t wf_1;
    int32_t wt_0;
    int32_t wt_1;

};

#define DMRS_MODULE "DMRS"
struct DMRS_call_api{
    virtual uint32_t Generate_dmrs_sequence(FAPI_P_T_DMRS_CFG *dmrs_cfg, uint32_t cellid, uint32_t layer,
            Complex *symbols, uint32_t SlotNumber, uint32_t l_offset, uint32_t SeqLength, uint32_t SeqOffset) =0;

};


#endif /* API_MODULE_API_I_DMRS_API_H_ */
