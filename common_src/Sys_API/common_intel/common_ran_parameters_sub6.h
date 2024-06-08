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
* @file common_ran_parameters_sub6.h
* @brief This header file defines those parameters defined in spec.
*******************************************************************************/

#ifndef _COMMON_RAN_PARAMETERS_SUB6_  /* the macro is file name in capture */
#define _COMMON_RAN_PARAMETERS_SUB6_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef PHY_20M_15K
//numerology and frame structure
#define NUM_MU                          ( 0 ) //the MU number: 15kHz normal
#define NUM_OF_SLOT_PER_SUBFRAME        ( 1 )
#define NUM_OF_SLOT_PER_FRAME           (NUM_OF_SUBFRAME_IN_ONE_FRAME*NUM_OF_SLOT_PER_SUBFRAME)
#define MAX_NR_SLOTS_PER_10MS           ( 10 )

#define MAX_FRAME_NUM   1000            //need further modification
#define MAX_SYM_PER_SLOT NUM_OF_SYMBOL_PER_SLOT

#define TIME_INTERVAL ( 1 ) //ms

// physical resource
#define NUM_OF_PRB_IN_FULL_BAND (106)
#define MAX_NR_RB (NUM_OF_PRB_IN_FULL_BAND)
#define N_FFT_SIZE 2048
#define N_FULLBAND_SC 1272
#define N_OFFSET ((N_FFT_SIZE-N_FULLBAND_SC)/2)
#define N_MAX_CP 160
#define MAX_NUM_RB NUM_OF_PRB_IN_FULL_BAND     //maximum number of resource blocks
#define MAXRESOURCEBLOCKS MAX_NUM_RB

#else
//numerology and frame structure
#define NUM_MU                          ( 1 ) //the MU number: 30kHz normal
#define NUM_OF_SLOT_PER_SUBFRAME        ( 2 )
#define NUM_OF_SLOT_PER_FRAME           (NUM_OF_SUBFRAME_IN_ONE_FRAME*NUM_OF_SLOT_PER_SUBFRAME)
#define MAX_NR_SLOTS_PER_10MS           ( 20 )

#define MAX_FRAME_NUM   2000            //need further modification
#define MAX_SYM_PER_SLOT NUM_OF_SYMBOL_PER_SLOT

#define TIME_INTERVAL (0.5) //ms

// physical resource
#define NUM_OF_PRB_IN_FULL_BAND (273)
#define MAX_NR_RB (NUM_OF_PRB_IN_FULL_BAND)
#define N_FFT_SIZE 4096
#define N_FULLBAND_SC 3276
#define N_OFFSET ((N_FFT_SIZE-N_FULLBAND_SC)/2)
#define N_MAX_CP 352
#define MAX_NUM_RB NUM_OF_PRB_IN_FULL_BAND     //maximum number of resource blocks
#define MAXRESOURCEBLOCKS MAX_NUM_RB
#endif

#define MAX_DL_CODEWORD (2)
#define MAX_UL_CODEWORD (1)
#define MAX_CODEWORD MAX_DL_CODEWORD
#define MAX_SCRTABLEN 90000
#define MAX_PUCCH_WIENER_TABLE_LEN (4*12*141*2)

// HARQ resource
#define MAX_UE_HARQ 200
#define MAX_HARQ_ID 16
#define HARQ_FLAG 1

//phy dump usage
#define DUMP_DATA_LENGTH 3276*4*14*160
#define DUMP_SLOT_LENGTH 160
#define DUMP_SLOT_DATA_LENGTH 91728
#define DUMP_SYMBOL_DATA_LENGTH 6552
#define DUMP_SC_NUM 3276

//FH FPGA buffer
#define FPGA_TO_SW_FTH_RX_BUFFER_LEN     (13152*14)  // in byte
#define FPGA_TO_SW_PRACH_RX_BUFFER_LEN   (8192)
#define SW_TO_FPGA_FTH_TX_BUFFER_LEN     (13152*14)  // in byte
#define PUSCH_2ND_PROC_START_SYMB 8
#define SET_FPGA_CELL_NUM(CONFIG, CELL_NUM)((CONFIG->nSectorNum)=(CELL_NUM))
#define FH_BUFF_NUM 4
#define FH_CONFIG 0x10303
#define FH_BURST_SYM_NUM 2
#define FPGA_CELL_NUM 1

//FEC FPGA buffer
#define FPGA_TO_SW_DECODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define FPGA_TO_SW_ENCODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_ENCODE_IN_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_DECODE_IN_BUFFER_LEN  (4096*256)  // in byte

//CSI
//rho=3:273RB*3(k0/4/8)*2(qpsk)
#define CSI_SEQ_BYTE_LEN 205
//273RB*3(k0,k1,k2)*4
#define CSI_MOD_BUF_BYTESIZE 3328
#define CSI_MOD_BUF_COMPLEXSIZE 832

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_RAN_PARAMETERS_SUB6_ */

