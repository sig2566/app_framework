/******************************************************************
 * ru_mempool_defines.h
 * Author: Edi M. (moved by D. Matas)
 * Created: June 2019
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef RU_MEMPOOL_DEFINES_H
#define RU_MEMPOOL_DEFINES_H
#include 	"global_defines.inc"

// U-Plane Mempool Definitions ::
// Indirect pool of mbufs , this pool size is combines for Cplane & uplane (& Weights)->  i.e. Uplane indirect pool size + Cplane indirect pool size (+ Weights indirect mbufs)
#define WEIGHTS_NUM_OF_BUFFERS          (MAX_NUM_OF_RBS_FH *2) // 1 mbuf for the Eth_header + 1 mbuf for the weights + FPGA Header

#define	INDIRECT_MEMPOOL_NAME			"INDIRECT_MEMPOOL"                                            //Note:: The optimum size (in terms of memory usage) for a mempool is when n is a power of two minus one: n = (2^q - 1).
#define	INDIRECT_MAX_NUM_BUFFERS			( WEIGHTS_NUM_OF_BUFFERS + (((QUAD_BUF + EXTRA_BUFFERS_BLOCK)*(MAX_RX_MBUF_IN_POOL_CPLANE) * (MAX_NUM_MTUs_CPLANE))*Cplane_implementation_num  +  ((RU_MAX_THEORETICAL_NUM_LAYERS)*MAX_NUM_OF_MTUs_UPLANE* (1 + EXTRA_BUFFERS_BLOCK))*NUM_OF_TESTS*4)     )//Note:: The optimum size (in terms of memory usage) for a mempool is when n is a power of two minus one: n = (2^q - 1).
#define	INDIRECT_BUFF_SIZE				(0) // indirect buffers
#define	INDIRECT_PRIVATE_DATA_SIZE		(0)


// C-Plane Mempool Definitions :: Direct mbufs

//==== # OF MTU ===
#define	CPLANE_DL_Data_SIZE		         ( sizeof(C_plane_CPU2FPGA_Headr) + sizeof(EtherHeadr) + sizeof(BF_Jd_Map)*MAX_NUM_OF_RBS_BH)
#define	CPLANE_UL_Data_SIZE		         ( sizeof(C_plane_CPU2FPGA_Headr) + sizeof(EtherHeadr) + sizeof(PE_Jd_Map)*MAX_NUM_OF_RBS_BH)

#define MTU_CPU_FPGA_MIN_SIZE_DL_ADJUST  ( MTU_CPU_FPGA_MIN_SIZE - (  MTU_CPU_FPGA_MIN_SIZE - sizeof(BF_Jd_Map)*(MTU_CPU_FPGA_MIN_SIZE/sizeof(BF_Jd_Map))  )  )
#define MTU_CPU_FPGA_MIN_SIZE_UL_ADJUST  ( MTU_CPU_FPGA_MIN_SIZE - (  MTU_CPU_FPGA_MIN_SIZE - sizeof(PE_Jd_Map)*(MTU_CPU_FPGA_MIN_SIZE/sizeof(PE_Jd_Map))  )  )

#define	MAX_NUM_OF_MTUs_CPLANE_DL        ((uint16_t)(CPLANE_DL_Data_SIZE / MTU_CPU_FPGA_MIN_SIZE_DL_ADJUST) + (uint8_t)((CPLANE_DL_Data_SIZE % MTU_CPU_FPGA_MIN_SIZE_DL_ADJUST) > 0))
#define	MAX_NUM_OF_MTUs_CPLANE_UL        ((uint16_t)(CPLANE_UL_Data_SIZE / MTU_CPU_FPGA_MIN_SIZE_UL_ADJUST) + (uint8_t)((CPLANE_UL_Data_SIZE % MTU_CPU_FPGA_MIN_SIZE_UL_ADJUST) > 0))
#define MAX_NUM_MTUs_CPLANE              (MAX(MAX_NUM_OF_MTUs_CPLANE_DL, MAX_NUM_OF_MTUs_CPLANE_UL))

#define	CPLANE_MEMPOOL_NAME				"CPLANE_MEMPOOL"

#ifdef RU_RT_MODE
#define	CPLANE_NUM_BUFFERS				2047 //((TRIPPLE_BUF+5) + EXTRA_BUFFERS_BLOCK)*(MAX_RX_MBUF_IN_POOL_CPLANE) // temporal value for RT mode. Take another set of buffers since we have some issue on releasing all the mbuffs on time of the next slot. We have to understand what is the right number of C-Plane buffers
#else
#define	CPLANE_NUM_BUFFERS				(QUAD_BUF + EXTRA_BUFFERS_BLOCK)*(MAX_RX_MBUF_IN_POOL_CPLANE_DL)
#endif

#define	CPLANE_BUFF_SIZE				(CPLANE_DL_Data_SIZE) // Data_length = sizeof(EtherHeadr) + sizeof(FPGAHeadr) + sizeof(BF_weight_map_typ)*MAX_RE_MSK*MAX_NUM_OF_RBS_BH + buffer
#define	CPLANE_PRIVATE_DATA_SIZE		(0)

// Ethernet Mempool Definitions
#define	ETH_HEADERS_MEMPOOL_NAME		"ETH_HEADERS_MEMPOOL"
#ifdef RU_RT_MODE
#define	ETH_HEADER_MAX_NUM_BUFFERS		(INDIRECT_MAX_NUM_BUFFERS)
#define	CPLANE_ETH_HDR_NUM_BUFFERS		(RU_MAX_NUM_OF_DL_LAYERS*MAX_NUM_OF_MTUs_CPLANE_DL*3)   
#else
#define	ETH_HEADER_MAX_NUM_BUFFERS		(INDIRECT_MAX_NUM_BUFFERS*18) // Every Indirect mbuf need (Eth+FPGA) header mbuf to be added  :: The number of elements in the mempool. The optimum size (in terms of memory usage) for a mempool is when n is a power of two minus one: n = (2^q - 1).
#define	CPLANE_ETH_HDR_NUM_BUFFERS		(RU_MAX_NUM_OF_DL_LAYERS*MAX_NUM_OF_MTUs_CPLANE_DL*3)   //  DL_Layer_num * MAX_NUM_OF_MTUs_CPLANE * 3(for safer side)
#endif
// we add to the ETH HEADERS size RTE_PKTMBUF_HEADROOM=128 byte * ((273/8)+1) - (+1 for ceiling to 35)
// this can allow using this amount for chaining up to 273 mbufs (16 bytes per 1 mbuf), i.e. 1 mbuf per RB (worst case)
// This is required for using the NXP patch of avoiding additional mbuf allocation by Driver when we use chain of mbufs
// according to NXP recommendation the whole buffer's size should be multiplication of 128 bytes (2 cache lines)
//#define ETH_HEADER_SIZE					(16) // space for the ETH Header
//#define HEADER_SIZE_FOR_CHAINING		(RTE_PKTMBUF_HEADROOM*((MAX_NUM_OF_RBS_BH/8)+1))
//#define	ETH_HEADER_BUFF_SIZE			(RTE_PKTMBUF_HEADROOM + HEADER_SIZE_FOR_CHAINING + ETH_HEADER_SIZE) // mbuf size including RTE_PKTMBUF_HEADROOM
#define	ETH_HEADER_BUFF_SIZE			(16)
#define	ETH_HEADER_PRIVATE_DATA_SIZE	(0)



// C-UL - Plane Mempool Definitions :: Direct mbufs
#define	CPLANE_UL_MEMPOOL_NAME			"CPLANE_UL_MEMPOOL"
#define	CPLANE_UL_NUM_BUFFERS			((QUAD_BUF + EXTRA_BUFFERS_BLOCK)*(MAX_RX_MBUF_IN_POOL_CPLANE_UL))
#define	CPLANE_UL_BUFF_SIZE				(CPLANE_UL_Data_SIZE) // Note:: the data size of the Cplane UL Buffer  != DL because we want to use different mempool => in the current implementation we need different data sizes for that.
//																			      // Data_length = sizeof(EtherHeadr) + sizeof(FPGAHeadr) + sizeof(BF_weight_map_typ)*MAX_RE_MSK*MAX_NUM_OF_RBS_BH + buffer
#define	CPLANE_UL_PRIVATE_DATA_SIZE		(0)

// Zeros mempool use for empty uplane
#define	ZEROS_MEMPOOL_NAME			"ZEROS_MEMPOOL"
#define	ZEROS_NUM_BUFFERS		    (2)
#define	ZEROS_BUFF_SIZE		        (MAX_NUM_OF_RBS_BH*NUMBER_SC_IN_RB*sizeof(Complex))
#define	ZEROS_PRIVATE_DATA_SIZE		(0)

// Eth headers mempool use for  uplane chain - ring allocation
//#define USE_NXP_PATCH_FOR_CHAIN_USAGE_OPTIMIZATION
#define	UPLANE_DL_ETH_MEMPOOL_NAME			"UPLANE_DL_ETH_MEMPOOL"
#define	UPLANE_DL_ETH_HEADRS_MAX_NUM_BUFFERS		(ETHERNET_HEADER_UPLANE_DL_MAX_SIZE*DBL_BUF)

#ifdef USE_NXP_PATCH_FOR_CHAIN_USAGE_OPTIMIZATION

#define ETH_HEADER_SIZE								(16) // space for the ETH Header
#define HEADER_SIZE_FOR_CHAINING					(RTE_PKTMBUF_HEADROOM*((MAX_NUM_OF_RBS_BH/8)+1))
//#define HEADER_SIZE_FOR_CHAINING					(RTE_PKTMBUF_HEADROOM*5) // we need up to 5 blocks of 8 sections when each section will be 8 RBs (64 RBs total per block)
#define	UPLANE_DL_ETH_HEADRS_BUFF_SIZE				(RTE_PKTMBUF_HEADROOM + HEADER_SIZE_FOR_CHAINING + ETH_HEADER_SIZE) // mbuf size including RTE_PKTMBUF_HEADROOM

#else

#define	UPLANE_DL_ETH_HEADRS_BUFF_SIZE		        (16) // mbuf size including RTE_PKTMBUF_HEADROOM

#endif



#define	UPLANE_DL_ETH_HEADRS_PRIVATE_DATA_SIZE		(0)


//#define ETH_HEADER_SIZE					(16) // space for the ETH Header
//#define HEADER_SIZE_FOR_CHAINING		(RTE_PKTMBUF_HEADROOM*((MAX_NUM_OF_RBS_BH/8)+1))
//#define	ETH_HEADER_BUFF_SIZE			(RTE_PKTMBUF_HEADROOM + HEADER_SIZE_FOR_CHAINING + ETH_HEADER_SIZE) // mbuf size including RTE_PKTMBUF_HEADROOM
////#define	ETH_HEADER_BUFF_SIZE			(16)
//#define	ETH_HEADER_PRIVATE_DATA_SIZE	(0)

#define	UPLANE_DL_MEMPOOL_NAME			"UPLANE_DL_MEMPOOL"
#define	UPLANE_DL_NUM_BUFFERS		    (UPLANE_DL_SIZE*DBL_BUF)
#define	UPLANE_DL_BUFF_SIZE		        (0)
#define	UPLANE_DL_PRIVATE_DATA_SIZE		(0)



#endif







