/******************************************************************
 * i_networking_api.h
 * Author: Maya Levanovsky
 *  Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef API_MODULE_API_I_NETWORKING_API_H_
#define API_MODULE_API_I_NETWORKING_API_H_
#include <rte_ether.h>
#include "global_defines.inc"
#include "i_api_common.h"
#include "i_cpp_module.h"

// Module name
#define NETWORKING_MOUDLE	"MAIN_NETWORKING"

#define ETHERNET_ADDRESS_LENGTH   6
#define ETHERNET_ETHERTYPE_LENGTH 2
#define ETHERNET_CRC_LENGTH       4
#define ETHERNET_HDR_LENGTH       (2*ETHERNET_ADDRESS_LENGTH  + ETHERNET_ETHERTYPE_LENGTH)	//	MAC destination address + MAC source address + EtherType
#define ETHERNET_MIN_LENGTH       0
#define ETHERNET_MAX_LENGTH       0
#define ETHER_TYPE_ECPRI          0xAEFE

#define	ETHER_TYPE_SYNTHETIC	  0x0600

#define SOURCE_ADDRESS_OF_WORKER_THREAD      0xf1

//#define CPU_BIG_ENDIAN		1				   //  Big Endian architecture
//#define CPU_LITTLE_ENDIAN	    2			       //  Little Endian architecture
//#define CPU_BYTE_ORDER		CPU_LITTLE_ENDIAN  //  This is the definition of the processor's endianness

// Eth. Dev. Definitions
#define MAX_ETH_DEV_RX_QUE_NUM     (1)
#define MAX_ETH_DEV_TX_QUE_NUM     (1)
#define MAX_ETH_QUEUE_SIZE         (1024)
#define ETH_RX_DEV_CACHE_SIZE      (250)
#define MAX_ETH_DEV_MBUF_NUM       (4)

#ifdef RU_RT_MODE
#define FPGA_DPDK_PORT_ID            (1)
#define FH_DPDK_PORT_ID              (0)
#else
#define FPGA_DPDK_PORT_ID            (0)
#define FH_DPDK_PORT_ID              (1)
#endif

//******************************************************************************************
//	Interface between FPGA and CPU:
//	-------------------------------

#define DA_TYPE_BYTE_INDEX_BE     0	//	Data Address Type byte index in Big Endian (as the
									//	FPGA interface requires) in the destination MAC address
#define	LID_BYTE_INDEX_BE         1	//	Layer ID byte index in Big Endian (as the FPGA
									//	interface requires) in the destination MAC address
#define SRC_ADDR_ID_BYTE_INDEX_BE 0	//	Thread ID of the source MAC address in Big Endian

//	Important Note:
//	---------------
//	The byte index that mentioned in the FPGA-CPU interface described in
//	[https://confluence.airspan.com/display/FPGA/Phase+1+-+10Ge+Back-haul]
//	is in Big Endian format. Meaning that the MSB is written on the left and the LSB is
//	written on the right. Therefore, we need to reverse the index of the bytes in our
//	application (0->5 1->4 and so on).
#define	DA_TYPE_BYTE_INDEX     (ETHER_ADDR_LEN - 1 - DA_TYPE_BYTE_INDEX_BE)
#define	LID_BYTE_INDEX         (ETHER_ADDR_LEN - 1 - LID_BYTE_INDEX_BE)
#define SRC_ADDR_ID_BYTE_INDEX (ETHER_ADDR_LEN - 1 - SRC_ADDR_ID_BYTE_INDEX_BE)
//******************************************************************************************


enum FPGA_DEST_ADDRESS //for future use
 {
 	ADDRESS_UPLANE_LAYER0_Q,
	ADDRESS_UPLANE_LAYER1_Q,
	ADDRESS_UPLANE_LAYER2_Q,
	ADDRESS_UPLANE_LAYER3_Q,
	ADDRESS_UPLANE_LAYER4_Q,
	ADDRESS_UPLANE_LAYER5_Q,
	ADDRESS_UPLANE_LAYER6_Q,
	ADDRESS_UPLANE_LAYER7_Q,

	ADDRESS_CPLANE_LAYER0_Q,
	ADDRESS_CPLANE_LAYER1_Q,
	ADDRESS_CPLANE_LAYER2_Q,
	ADDRESS_CPLANE_LAYER3_Q,
	ADDRESS_CPLANE_LAYER4_Q,
	ADDRESS_CPLANE_LAYER5_Q,
	ADDRESS_CPLANE_LAYER6_Q,
	ADDRESS_CPLANE_LAYER7_Q,
  } ;
///////// CPU to FPGA definitions due to http://confluence-il:8090/display/FPGA/10Ge+Back-haul?focusedCommentId=72220865#comment-72220865 ////////////
#define FIVE_BIT_MASK    0x1F
#define NINE_BIT_MASK    0x1FF
#define FIFTEEN_BIT_MASK 0x7FFF


struct U_plane_CPU2FPGA_L3_Header
{

#if (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN)
	uint16_t Eth_SQID   : 9;   // 9-bit sequence ID
	uint16_t Eth_RSV    : 2;   // 2-bit reserve bits
	uint16_t Eth_SID    : 5;   // 5-bit symbol ID
#else
	uint16_t Eth_SID    : 5;   // 5-bit symbol ID
	uint16_t Eth_RSV    : 2;   // 2-bit reserve bits
	uint16_t Eth_SQID   : 9;   // 9-bit sequence ID
#endif

}PACK_STRUCT;


struct C_plane_CPU2FPGA_L3_Header
{
#if (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN)
	uint16_t Eth_RBSi   : 9;   // 9-bit sequence ID
	uint16_t Eth_R2     : 2;   // 2-bit reserve bits
	uint16_t Eth_SID    : 5;   // 5-bit symbol ID
#else
	uint16_t Eth_SID    : 5;   // 5-bit symbol ID
	uint16_t Eth_R2     : 2;   // 2-bit reserve bits
	uint16_t Eth_RBSi   : 9;   // 9-bit sequence ID
#endif
}PACK_STRUCT;

struct Weights_CPU2FPGA_L3_Header
{
#if (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN)
	uint16_t Eth_BeamID : 15;   // 15-bit Beam ID
	uint16_t Eth_R1     : 1;    // 1 -bit reserve bits
#else
	uint16_t Eth_R1     : 1;    // 1 -bit reserve bits
	uint16_t Eth_BeamID : 15;   // 15-bit Beam ID
#endif
}PACK_STRUCT;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//struct EtherHeadr
//{
//	uint8_t  ether_dhost[ETHERNET_ADDRESS_LENGTH];
//	uint8_t  ether_shost[ETHERNET_ADDRESS_LENGTH];
//	uint16_t ether_type;
//
//};

typedef ether_hdr EtherHeadr;



enum DestAddr : uint8_t
{
	DL_ANT_DATA     = 0x00, // U-Plane DL Data
	BF_DESCRIPTOR   = 0x02, // C-Plane DL
	UL_ANT_DATA     = 0x10, // U-Plane UL Data
	UL_PRACH_DATA   = 0x11, // U-Plane PRACH Data
	PE_DESCRIPTOR   = 0x12, // C-Plane UL
   	DYN_SYMBOL_MAP  = 0x20,
	BF_WEIGHTS      = 0x21  // BeamForming Weights
};



// Uplane - Ethernert Header + FPGA Header
struct U_plane_CPU2FPGA_Headr
{
	union
	{
		uint16_t 	               Uplane_hdr;
		U_plane_CPU2FPGA_L3_Header U_plane_CPU2FPGA;
	};
}PACK_STRUCT;


// Cplane - Ethernert Header + FPGA Header
struct C_plane_CPU2FPGA_Headr
{
	union
	{
		uint16_t 	               Cplane_hdr;
		C_plane_CPU2FPGA_L3_Header C_plane_CPU2FPGA;
	};
}PACK_STRUCT;

// Weights - FPGA Header
struct Weights_CPU2FPGA_Headr{
	union{
		uint16_t 	               Weights_hdr;
		Weights_CPU2FPGA_L3_Header Weights_CPU2FPGA;
	};
}PACK_STRUCT;


struct RU_Networking_API
{
	virtual EResultT ReadEthMacAddr(uint32_t port_id, struct ether_addr *mac_addr) = 0;
	virtual EResultT UpdateEthHader(uint8_t ether_dhost[], uint8_t ether_shost[], EtherHeadr* headr) = 0;
	virtual EResultT RemoveEtherHeader(uint8_t* data_ptr) = 0;
	virtual uint8_t* GetDestAdress(EtherHeadr* headr) = 0;
	virtual uint8_t* GetSourceAdress(EtherHeadr* headr) = 0;
    virtual uint16_t GetEtherHeaderSize() = 0;
    virtual EResultT CopyDataTrailerAndHeader(	uint8_t*output_data_ptr,
												uint8_t* data_ptr,
												uint8_t data_len,
												uint8_t data_type,
												uint8_t  ether_dhost[],
												uint8_t ether_shost[],
												uint32_t FCS) = 0;
    virtual EResultT AddEthernetHeader_FPGA(EtherHeadr* EtherHeadr_ptr, DestAddr da_type, int32_t* Layer_ind = nullptr) = 0;
	virtual EResultT AddEthernetHeader_FH(EtherHeadr* EtherHeadr_ptr) = 0;
};


#endif /* API_MODULE_API_I_MIMO_RECEIVER_API_H_ */
