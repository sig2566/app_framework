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
* @file common_ran_parameters_mmwave.h
* @brief This header file defines those parameters defined in spec.
*******************************************************************************/

#ifndef _COMMON_RAN_PARAMETERS_MMWAVE_  /* the macro is file name in capture */
#define _COMMON_RAN_PARAMETERS_MMWAVE_


#ifdef __cplusplus
extern "C" {
#endif

//numerology and frame structure
#define NUM_MU                          ( 3 ) //the MU number: 120kHz normal
#define NUM_OF_SLOT_PER_SUBFRAME        ( 8 )

#define NUM_OF_SLOT_PER_FRAME           (NUM_OF_SUBFRAME_IN_ONE_FRAME*NUM_OF_SLOT_PER_SUBFRAME)
#define MAX_NR_SLOTS_PER_10MS           ( 320 )

#define MAX_FRAME_NUM   8000
#define MAX_SYM_PER_SLOT NUM_OF_SYMBOL_PER_SLOT

#define TIME_INTERVAL (0.125) //ms

// physical resource
#define NUM_OF_PRB_IN_FULL_BAND (66)
#define MAX_NR_RB (NUM_OF_PRB_IN_FULL_BAND)
#define N_FFT_SIZE 1024
#define N_FULLBAND_SC 792
#define N_OFFSET ((N_FFT_SIZE-N_FULLBAND_SC)/2)
#define N_MAX_CP 136
#define MAX_NUM_RB NUM_OF_PRB_IN_FULL_BAND     //maximum number of resource blocks
#define MAXRESOURCEBLOCKS MAX_NUM_RB

#define MAX_DL_CODEWORD (2)
#define MAX_UL_CODEWORD (1)
#define MAX_CODEWORD MAX_DL_CODEWORD
#define MAX_SCRTABLEN 21600
#define MAX_PUCCH_WIENER_TABLE_LEN (4*12*141*2)

//phy dump usage
#define DUMP_DATA_LENGTH 792*4*14*160
#define DUMP_SLOT_LENGTH 160
#define DUMP_SLOT_DATA_LENGTH 22176
#define DUMP_SYMBOL_DATA_LENGTH 1584
#define DUMP_SC_NUM 792

// HARQ resource
#define MAX_UE_HARQ 16
#define MAX_HARQ_ID 16
#define HARQ_FLAG 0
//FH FPGA buffer

#define FPGA_TO_SW_FTH_RX_BUFFER_LEN     (3200*14)  // in byte
#define FPGA_TO_SW_PRACH_RX_BUFFER_LEN   (8192)
#define SW_TO_FPGA_FTH_TX_BUFFER_LEN     (3200*14)  // in byte
#define PUSCH_2ND_PROC_START_SYMB 7
#define SET_FPGA_CELL_NUM(CONFIG, CELL_NUM)
#define FH_BUFF_NUM 8
#define FH_CONFIG 0x70303
#define FH_BURST_SYM_NUM 7
#define FPGA_CELL_NUM 3

//FEC FPGA buffer
#define FPGA_TO_SW_DECODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define FPGA_TO_SW_ENCODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_ENCODE_IN_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_DECODE_IN_BUFFER_LEN  (4096*256)  // in byte

//CSI
//rho=3:66RB*3(k0/4/8)*2(qpsk)
#define CSI_SEQ_BYTE_LEN 50
//66RB*3(k0,k1,k2)*4
#define CSI_MOD_BUF_BYTESIZE 832
#define CSI_MOD_BUF_COMPLEXSIZE 208


#ifdef __cplusplus
}
#endif

#endif /* _COMMON_RAN_PARAMETERS_MMWAVE_ */

