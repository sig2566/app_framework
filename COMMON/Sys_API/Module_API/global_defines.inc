/******************************************************************
 * global_defines.inc
 * Author: M. Ambar, D.Matas
 * Created: March 2018
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef MODULES_SAMPLE_MODULE_GLOBAL_DEFINES_INC_
#define MODULES_SAMPLE_MODULE_GLOBAL_DEFINES_INC_

#include "math.h"

typedef unsigned int uint32_t;
typedef unsigned short UINT16;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed int int32_t;

typedef unsigned char UINT8;
//typedef char INT8;

//enum cond {FALSE, TRUE};


//#define ASSERT(condition) while(condition)
#define MAX_NUM_OF_5G_CELLS		1	/*can be up to 16 */
#define MAX_NUM_OF_BW_PARTS		4	/*only 1 bandwidth part can active in a given time */

//Constants for Code-Block
#define MAX_SIZE_OF_CODE_BLOCK_IN_BITS		   (8448)	/*8448 bits/block */
#define	MAX_SIZE_OF_CODE_BLOCK_IN_BITS_P144    (MAX_SIZE_OF_CODE_BLOCK_IN_BITS+144) /*for CRC out of all TB for last CB (24 bits) +CRC out of CB (24 bits) +10 bytes for optimized interleaving issue */
#define MAX_SIZE_OF_CODE_BLOCK_IN_BYTES_P144   (MAX_SIZE_OF_CODE_BLOCK_IN_BITS_P144/8);

#define MAX_NUM_OF_CODE_BLOCKS_PER_TB		304	/*???? 8448 bits/block, achieved at 275 RBs and 256QAM modulation */

#define MAX_SIZE_OF_LDPC_OUT_IN_BYTES        ((26112-8448)/8)   /*25344 = 8448*3 + 768 */
//Er - interleaving output (cannot get to 26112, but can be close ~25000)
#define MAX_SIZE_OF_CODEBLOCK_OUT_IN_BYTES   (26112/8)

////////Constants to be changed for different testing of RU///////
///for 1->1 all constants =1,for 2->2 all constants =2, for 4->4 all constants=4,for 8->32 NUM_OF_RU_ANTENNAS=32,RU_MAX_NUM_OF_DL_LAYERS=8 and rest =4
#ifdef _1x1_
#define NUM_OF_RU_ANTENNAS         (1)
#define RU_MAX_NUM_OF_DL_LAYERS    (1)
#define RU_MAX_NUM_OF_UL_LAYERS    (1)
#endif
#ifdef _2x2_
#define NUM_OF_RU_ANTENNAS         (2)  //(32) ByPass Mode /
#define RU_MAX_NUM_OF_DL_LAYERS    (2)  //(8) ByPass Mode
#define RU_MAX_NUM_OF_UL_LAYERS    (2)
#endif
#ifdef _4x4_
#define NUM_OF_RU_ANTENNAS         (4)  //(32) ByPass Mode /
#define RU_MAX_NUM_OF_DL_LAYERS    (4)  //(8) ByPass Mode
#define RU_MAX_NUM_OF_UL_LAYERS    (4)
#endif
#ifdef _8x32_
#define NUM_OF_RU_ANTENNAS         (32)  //(32) ByPass Mode /
#define RU_MAX_NUM_OF_DL_LAYERS    (8)  //(8) ByPass Mode
#define RU_MAX_NUM_OF_UL_LAYERS    (4)
#endif

#define MAX_NUM_OF_LAYERS          (MAX(RU_MAX_NUM_OF_DL_LAYERS, RU_MAX_NUM_OF_UL_LAYERS))
#define MAX_NUM_OF_RU_ANTENNAS         (32)
#define RU_MAX_MAX_NUM_OF_DL_LAYERS    (8)
#define RU_MAX_MAX_NUM_OF_UL_LAYERS    (4)
/////////////////////////////////////////////////////////////////
#define MU                         (1)

#define MAX_NUM_OF_RX_ANTENNAS     (16)
#define MAX_NUM_OF_TBS	           (275)
//#define MAX_NUM_OF_RBS	       (275) //not in use any more
#define MAX_NUM_OF_RBS_FH	       (273)
#define MAX_NUM_OF_RBS_BH	       (275)
#define RB_OFFSET_BH_FH            ((MAX_NUM_OF_RBS_BH-MAX_NUM_OF_RBS_FH)/2)

#define MAX_NUM_OF_SF      	       (10)
#define MAX_NUM_OF_F      	       (256)
#define MAX_NUM_OF_TB_SUBFRAMES    (3)  /*TB can be used during 3 subframes */
#define MAX_NUMBER_OF_UE           (1000)
#define MAX_NUM_OF_RNTI	           (65536)

#define MAX_NUM_OF_BITS_IN_SYMBOL  (8)  /*Enable256Qam : 256Qam = 8bits */
#define MAX_NUM_OFDM_PER_SLOT      (14)
#define MAX_NUM_SLOT_PER_SF        (1<<MU)
//#define MAX_NUM_SLOT_PER_SF        (2)

#define MAX_NUM_SLOT_PER_FRAME     (MAX_NUM_OF_SF * MAX_NUM_SLOT_PER_SF)
#define MAX_NUM_OF_FRAMES           (256)
#define MAX_NUM_OF_SLOT_IN_FRAMES   (MAX_NUM_OF_FRAMES * MAX_NUM_SLOT_PER_FRAME)

#define MAX_NUM_TBS_PER_SLOT       (13)
#define MIN_DMRS_NUMBER            (1)  /*only 1 DMRS in slot (difficult in more than 1 layer) */
#define NUMBER_SC_IN_RB            (12) /*12 SC in 1 RB */
#define DU_MAX_NUM_OF_LAYERS       (8)
#define MAX_DATA_STREAMS_PER_RB_DU (32)                    // 32 stands for the possibility of TX Antennas streaming (e.g. for CSI-RS) instead of 8 Layers that we have to support in DL (e.g. for PDSCH)
#define MAX_NUM_SUB_CURRIERS_BH    (MAX_NUM_OF_RBS_BH * NUMBER_SC_IN_RB)
#define MAX_NUM_SUB_CURRIERS_FH    (MAX_NUM_OF_RBS_FH * NUMBER_SC_IN_RB)

#define MAX_BF_WEIGHT_MEM_VHDL_SIZE	32767

#define MAX_CODED_TB_SIZE          ((MAX_NUM_OF_RBS_FH*MAX_NUM_OF_BITS_IN_SYMBOL*(MAX_NUM_OFDM_PER_SLOT-MIN_DMRS_NUMBER)*NUMBER_SC_IN_RB*MAX_NUM_OF_LAYERS))
#define MAX_TB_SIZE                ((uint32_t)(CEIL( (float)(MAX_CODED_TB_SIZE)* (948.0/1024.0)  ) )  )   /*use maximum Rate ratio (MCS=27) */
#define MAX_TB_SIZE_IN_32BITS      (uint32_t(CEIL(float(MAX_TB_SIZE)/32.0)))

#define CRC_24_SIZE                (24) /*in bits */
#define CRC_16_SIZE                (16) /*in bits */
#define MAX_QM				       (8)
#define MAX_TB_SIZE_IN_BYTES       (uint32_t(4*MAX_TB_SIZE_IN_32BITS))

//For DMRS
#define MAX_DMRS_SEQUENCES_NUM_PER_SYMBOL	(MAX_NUM_OF_LAYERS) // for each pair of Layers we have 2 DMRS Sequences (for k_tag=0,1).
#define MAX_NUM_DMRS_SYMBOLS                (4)
#define MAX_NUM_OF_DMRS_SEQ                 (MAX_DMRS_SEQUENCES_NUM_PER_SYMBOL * MAX_NUM_DMRS_SYMBOLS) //4 seq * 4 symbols possible for DMRS
#define MAX_NUM_OF_DMRS_PER_RB_PER_SEQ      (3)
#define MAX_DMRS_REF_SIZE                   (MAX_NUM_OF_RBS_FH * MAX_NUM_OF_DMRS_SEQ * MAX_NUM_OF_DMRS_PER_RB_PER_SEQ* MAX_NUM_OF_LAYERS)
//For FFT
#define MAX_FFT_OUTPUT_SIZE_PER_SYMBOL      (4096 +352) /*352 is max Cyclic Prefix*/
#define FFT_OUTPUT_SIZE_PER_SYMBOL_CP_LONG  (4096 +352) /*352 is long Cyclic Prefix */
#define FFT_OUTPUT_SIZE_PER_SYMBOL_CP_SHORT (4096 +288) /*288 is short Cyclic Prefix */

#define MAX_HANDLER_DATA_IO_SIZE            (1000000)
#define MAX_MESSAGE_LENGTH                  1500000

#define DBL_BUF				2		// number of buffers in double buff
#define TRIPPLE_BUF			3		// number of buffers in tripple buff
#define QUAD_BUF			4		// number of buffers in quads buff

#define	MAX_RE_MSK			2		// max number of RE mask words on CPLAN / UPLAN

#define OFFSET_BETWEEN_DL_UL_TICK	0 //  usec between DL & UL  ticks
//const uint32_t MU1_SYM_OFFSETS_USECS[] = {0, 35, 70, 105, 140, 175, 210,
//											 245, 280, 315, 350, 385, 420, 455 };
const uint32_t SCALE_FACTOR  = 1;// = 1E6/MAX_NUM_OFDM_PER_SLOT;
const uint32_t MU1_SYM_OFFSETS_USECS[] = {	0*SCALE_FACTOR,
											35*SCALE_FACTOR,
											70*SCALE_FACTOR,
											105*SCALE_FACTOR,
											140*SCALE_FACTOR,
											175*SCALE_FACTOR,
											210*SCALE_FACTOR,
											245*SCALE_FACTOR,
											280*SCALE_FACTOR,
											315*SCALE_FACTOR,
											350*SCALE_FACTOR,
											385*SCALE_FACTOR,
											420*SCALE_FACTOR,
											455*SCALE_FACTOR };

const uint32_t MU1_SLOT_OFFSETS_USECS[] = {	0   *SCALE_FACTOR,   //0
											500 *SCALE_FACTOR,   //1
											1000*SCALE_FACTOR,   //2
											1500*SCALE_FACTOR,   //3
											2000*SCALE_FACTOR,   //4
											2500*SCALE_FACTOR,   //5
											3000*SCALE_FACTOR,   //6
											3500*SCALE_FACTOR,   //7
											4000*SCALE_FACTOR,   //8
											4500*SCALE_FACTOR,   //9
											5000*SCALE_FACTOR,   //10
											5500*SCALE_FACTOR,   //11
											6000*SCALE_FACTOR,   //12
											6500*SCALE_FACTOR,   //13
											7000*SCALE_FACTOR,   //14
											7500*SCALE_FACTOR,   //15
											8000*SCALE_FACTOR,   //16
											8500*SCALE_FACTOR,   //17
											9000*SCALE_FACTOR,   //18
											9500*SCALE_FACTOR,   //19
};

const uint32_t MU1_SLOT_OFFSETS_USECS_SINGLE[] = {0 };
const uint32_t SYMBOL_NUM[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const uint32_t SLOT_NUM[]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 ,15 ,16 ,17 ,18 ,19};

//#ifdef RU_RT_MODE
//	const int32_t  SYMBOLS_OFFEST_USEC = -30 ;   // Note - Need to set to 0 in offline mode - neg values not supported
//	const int32_t  SLOT_OFFEST_USEC    = -20 ;   // Note - Need to set to 0 in offline mode - neg values not supported
//#else
//	const int32_t  SYMBOLS_OFFEST_USEC = 0   ;   // Note - Need to set to 0 in offline mode - neg values not supported
//	const int32_t  SLOT_OFFEST_USEC    = 0   ;   // Note - Need to set to 0 in offline mode - neg values not supported
//#endif




#define CPU_BIG_ENDIAN		1					    //  Big Endian architecture
#define CPU_LITTLE_ENDIAN	2					    //  Little Endian architecture
#define	CPU_BYTE_ORDER		CPU_LITTLE_ENDIAN	//  This is the definition of the processor's endianness
#define	CPU_BYTE_ORDER_ASFH		CPU_BIG_ENDIAN	//  This is the definition of the processor's endianness


//#define PROFILING_TEST

#endif /* MODULES_SAMPLE_MODULE_GLOBAL_DEFINES_INC_ */

