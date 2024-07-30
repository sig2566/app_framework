/******************************************************************
 * i_slot_builder_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_SLOT_BUILDER_API_H_
#define API_MODULE_API_I_SLOT_BUILDER_API_H_
#include "i_api_common.h"


//Convertor from SLIV parameter to time start symbol S and numer symbols L
// Due to 38.214-Table 5.1.2.1-1 - Used for DCI only !! (Meanwhile not used !)

struct SLIV_2_S_L_Convertor
{
    int32_t S;
    int32_t L;
    int32_t L_1;
    int32_t S_PLUS_L;
    int32_t SLIV;
    int32_t PDSCH_type;
    int32_t PUSCH_type;
};
//Frequency allocation data
//-------------------------

//TS38.214 - 5.1.2.2
//Like in LTE case MAC provides bit map to allocate corresponding
//RBS. The size of the bit mask is increased to 9 32 bits integers
/*
struct FreqAllocData
{
    //Allocation type 0
    uint32_t                rb_bitmap[RBS_MASK_SIZE];
};
*/

//Definition symbols grid for subframe layers
//------------------------------
struct LayerData
{
    Complex symb[SYMB_IN_SLOT][NUM_RB_CARRIERS* MAX_SLOT_RBS];
};

#define	MAX_NUM_DMRS_IN_RB_PER_LAYER	(6) // 3 DMRS * 2 sequences

// 16 full slots of 275 RBs supported. this number could be decreased later if we will implement usage of memory chunks less than 275 RBs,
// now this value supports up to 16 TBs of maximal length of 275 RBs
#define	MAX_NUM_DMRS_FULL_SLOTS_SUPPORTED	(16)

#define MAX_NUM_DMRS_SYMBOLS_PER_SLOT		(4)

// maximum number of DMRS REs in a single layer in a slot
struct TbDmrsRefData
{
	Complex			dmrs_ref_symb[MAX_NUM_DMRS_SYMBOLS_PER_SLOT][MAX_NUM_DMRS_IN_RB_PER_LAYER * MAX_SLOT_RBS];
};


struct SlotData
{
    LayerData layer_data[NUM_LAYERS];
};

struct SlotTbDmrsRefData
{
	TbDmrsRefData tb_dmrs_ref_data[NUM_LAYERS];
};


//Maximal slots number in the subframe for numerology 0,1
#define MAX_SLOTS_SUBF (2)
struct SubframeData
{
    SlotData   slot_data[MAX_SLOTS_SUBF];
};

struct SubframeTbDmrsRefData
{
	SlotTbDmrsRefData   slot_tb_dmrs_ref_data[MAX_SLOTS_SUBF];
};


//Definition debugging uint32 grid per subframe
//-----------------------------------------------
enum SymbolDebugInfo
{
    SYMB_PDSCH_DMRS= 0x44,
    SYMB_PDSCH = 0x55
};
struct LayerDebugData
{
    uint8_t info[NUM_RB_CARRIERS* MAX_SLOT_RBS][SYMB_IN_SLOT];
};

struct SlotDebugData
{
    LayerDebugData layer_debug_data[NUM_LAYERS];
};

struct SubframeDebugData
{
    SlotDebugData   slot_debug_data[MAX_SLOTS_SUBF];
};

enum MapperType
{
	PDSCH_MAPPER = 0,
	PUSCH_MAPPER
};

//Names
//------------------------------
#define SLOT_MOD_NAME "SLOT_BUILDER"
#define SLOT_AREA   "SLOT_DATA_AREA"
#define SLOT_DEBUG_AREA   "SLOT_DEBUG_AREA"
#define SLOT_DMRS_REF_AREA   "SLOT_RS_DATA_AREA"

/*******************************************************************************************//*!
*@class CB_SlotBuilder_api
*@brief The purpose of this class is :
*@brief API for CB to map the CB data into TB symbol map per layers
*@brief The MapCBData(uint32_t *data, uint32_t cb_length, uint32_t offset ) is used to call the data mapping
*@brief Params:
*@brief uint32_t *data - pointer to data after interleaving and scrambling and before QAM modulation. Data is kept as bit little endian bits stream.
*@brief uint32_t cb_length  - the lenght of the data in num bits
*@brief uint32_t offset - offset of the CB data in bits.
***********************************************************************************************/
struct CB_SlotBuilder_api
{
    virtual uint32_t MapCBData(uint32_t *data, uint32_t cb_length, uint32_t offset ) = 0;
};

struct TB_SlotBuilder_api{
    virtual uint32_t PrepareTBAllocation(FAPI_P_T_DMRS_CFG *dmrs_cfg, FAPI_P_T_SYMB_ALLOC *pdsch_alloc_rule,
                 uint32_t num_of_layers, uint32_t qm, FAPI_P_UINT32 *rb_bitmap, uint32_t SlotsPerSf, uint32_t cellid,
                 uint32_t cell_num_rbs,uint32_t subframe_num) =0;
    virtual CB_SlotBuilder_api  *AllocCB() = 0;


};


#endif /* API_MODULE_API_I_SLOT_BUILDER_API_H_ */
