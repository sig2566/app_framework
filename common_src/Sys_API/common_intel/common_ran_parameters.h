/*******************************************************************************
*
* INTEL CONFIDENTIAL
* Copyright 2009-2018 Intel Corporation All Rights Reserved.
* 
* The source code contained or described herein and all documents related to the
* source code ("Material") are owned by Intel Corporation or its suppliers or
* licensors. Title to the Material remains with Intel Corporation or its
* suppliers and licensors. The Material may contain trade secrets and proprietary
* and confidential information of Intel Corporation and its suppliers and
* licensors, and is protected by worldwide copyright and trade secret laws and
* treaty provisions. No part of the Material may be used, copied, reproduced,
* modified, published, uploaded, posted, transmitted, distributed, or disclosed
* in any way without Intel's prior express written permission.
* 
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Intel in writing.
* 
* Unless otherwise agreed by Intel in writing, you may not remove or alter this
* notice or any other notice embedded in Materials by Intel or Intel's suppliers
* or licensors in any way.
* 
*  version: RefPHY-flexran_18.12
*
*******************************************************************************/

/*******************************************************************************
* @file common_ran_parameters.h
* @brief This header file defines those parameters defined in spec.
*******************************************************************************/

#ifndef _COMMON_RAN_PARAMETERS_  /* the macro is file name in capture */
#define _COMMON_RAN_PARAMETERS_
#define _SUB6_
#include "common_config.h"

#ifdef PHY_APP
#include "bbupool_api.h"
#endif

#ifdef _SUB6_
#include "common_ran_parameters_sub6.h"
#else
#include "common_ran_parameters_mmwave.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//numerology and frame structure
#define NUM_NUMEROLOGY                  ( 5 ) //the total number of numerology
#define NUM_OF_SYMBOL_PER_SLOT          ( 14 )
#ifndef N_SYMB_PER_SF
#define N_SYMB_PER_SF                   ( 14 )
#endif
#ifndef NUM_OF_SUBFRAME_IN_ONE_FRAME
#define NUM_OF_SUBFRAME_IN_ONE_FRAME    (10)
#endif

// physical resource
#define N_SC_PER_PRB 12
#define MAX_DMRS_SYMBOL (4)
#define MAX_SYMB_PER_SL_DMRS 2
#define MAX_DATA_SYMB_PER_SL_UL 13
#define MAX_DATA_SYMB_PER_SL_DL 12
#define MAX_DATA_SYMB_PER_SL MAX_DATA_SYMB_PER_SL_UL

#define MAX_NUM_MCS                     ( 32 )      //maximum number of MCS Values assigned [0 .. 31]
// (max 12 symbols per slot)*(max 273 PRBs in 100MHz)*(12 SCs per PRB)*(max 4 layers)*(max 256 QAM)*(max 1 code rate)
// = 12*273*12*4*8=1257984 bits. According to 38.214 5.1.3.4, the TBS is 1245544 bits.
// = 155712 bytes with 64bytes aligned
#define MAX_DL_TB_SIZE 159808

// (max 13 symbols per slot)*(max 273 PRBs in 100MHz)*(12 SCs per PRB)*(max 4 layers)*(max 256 QAM)*(max 1 code rate)
// = 13*273*12*4*8=1362816 bits. According to 38.214 5.1.3.4, the TBS is 1376264 bits.
// = 172096 bytes with 64bytes aligned
#define MAX_UL_TB_SIZE 172096
#define PRB_NUM_PER_RBG   2

#define MOD_QPSK 2
#define MOD_16QAM 4
#define MOD_64QAM 6
#define MOD_256QAM 8
#define MAX_BITS_NUM_PER_RE (MOD_256QAM)

// system resource para
#define MAX_PATH_LEN 256

#define MAX_SECTOR_NR (3)
#define MAX_PHY_INSTANCES 8

#define MAX_SECTOR MAX_SECTOR_NR  /*used in L1 buffer management*/
#define SECTOR_LOAD (1)
#define SECTOR_USE (MAX_SECTOR * SECTOR_LOAD)

#define MAX_UE_NUM 8

#define LOW_LAYER_EVENT_QUEUE_LENGTH (40)

#define MAX_PHY_BUFFER_POOL (4)

#define MAX_MUMIMO_GROUP_NUM        (8)
#define MAX_PARIED_UE_NUM           (8)

#define MAX_NUM_OF_SF_CTX               ( 4 )

// Antenna configuration
#define MAX_UE_STREAM 8
#define MAX_BS_ANT_TX 8
#define MAX_BS_ANT_RX 8
#define MAX_UE_ANT_TX 8
#define MAX_UE_ANT_RX 8
#define MAX_RX_ANT_NUM MAX_BS_ANT_RX
#define MAX_TX_LAYER_NUM MAX_UE_ANT_TX
#define MAX_NUM_ANT MAX_UE_STREAM
#define MAX_NUM_UL_LAYER 2 //maximun UL layers per UE
#define MAX_NUM_DL_LAYER 4 //maximun DL layers per UE

// DMRS
#define MAX_DMRS_PER_PRB 6
#define N_DMRS_TYPE1_SC_PER_RB 6
#define N_DMRS_TYPE2_SC_PER_RB 4
#define MAX_DMRS_SC_NUM (NUM_OF_PRB_IN_FULL_BAND*MAX_DMRS_PER_PRB)
#define DMRS_FOR_PDSCH 0
#define DMRS_FOR_PUSCH 1
#define MAX_NUM_OF_DELTA (3)
#define N_DMRS_TYPE1_DELTA (2)
#define N_DMRS_TYPE2_DELTA (3)

//PDCCH
#define MAX_PDCCH_PAYLOAD          32          //256 bit, 32byte
#define MAX_PDCCH_SYMBOL           3
#define NUM_PDCCH_DMRS_PRE_PRB     3
#define NUM_PDCCH_DATA_PRE_PRB     9
#define MAX_PDCCH_DMRS             (NUM_OF_PRB_IN_FULL_BAND * MAX_DMRS_PER_PRB) //each PRB has 3 DMRS, each has re/im.
#define MAX_PDCCH_AllO_NUM_PER_TTI 16
#define MAX_PDCCH_POLAR_CODE_BYTE  128         //Maximum number of data after Polar Code is 1024bit, 128Byte.
#define MAX_PDCCH_POLAR_TEMP       1024
#define NUM_REGS_PER_CCE           6           //A coutrol-channel element consists of Six resource-element groups (REGs)
#define NUM_DATA_PER_CCE           NUM_REGS_PER_CCE * NUM_PDCCH_DATA_PRE_PRB * 2 //2 means each data has re/im
#define MAX_PDCCH_AllO_NUM_PER_TTI 16
//PDCCH Cases Parameter
#define PDCCHVectorInitial 1
#define PDCCH_USING_VEC_NUM 0
#define PDCCH_TOTAL_VEC_NUM 1
#define PDCCH_REGRESSION 0
#define PRINT_WHEN_PASS 0
#define QUANTIZATION_ERR_THRE  32
#define CORESET_SPLIT_NUM_TO_32BIT 2

//PDSCH
#define MAX_DL_RBG_BIT_NUM              (5)  // bitmap, For resource allocation type 0.The maximum number is 138 with 275 RBs and RBG size 2.
#define MAX_DL_PER_UE_DMRS_PORT_NUM     (8)
#define MAX_DL_PER_UE_PTRS_PORT_NUM     (2)


// RX link module related configruation
// SW mode: assume received signal is 13 bits
#define RX_DATA_FIXED_POINT (13)
#define CE_NOISE_EST_FACTOR ((RX_DATA_FIXED_POINT-1)*2)
#define MMSE_X_LEFT_SHIFT (RX_DATA_FIXED_POINT)
#define MMSE_LEMMA_SCALING ((RX_DATA_FIXED_POINT)*2)
#define MMSE_NOISE_POWER ((RX_DATA_FIXED_POINT)*2)
#define DE_MOD_RIGHT_SHIFT ((RX_DATA_FIXED_POINT)-5)

// polar encoder and decoder
#define MAX_E 1728
#define MAX_N 1024
#define INFO_TAB_LEN 4736726
#define INFOSORT_TAB_LEN 4736726
#define TAB_INDEX_LEN 52028
#define INTERLEAVER_TAB_LEN 46240
#define NUM_E 100
#define NUM_K 400

// ssb para common
#define SSBNUM_4 4
#define SSBNUM_8 8
#define SSBNUM_64 64
#define BYTE_4   4

//CSI dimension common
#define MAX_CSI_NZP 4
#define MAX_CSI_ZP 4
#define MAX_TRS 4

//PRACH para
#define MAX_PRACH_CONFIG_IDX            ( 255 )
#define MAX_PRACH_ZERO_ZONE_CONFIG      ( 15 )
#define MAX_PRACH_ROOT_SEQ_IDX          ( 837 )

/* define HARQ process number*/
#define MAX_UL_HARQ_PROCESS (8)
#define MAX_DL_HARQ_PROCESS (8)

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_RAN_PARAMETERS_ */

