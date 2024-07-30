/******************************************************************
 * i_build_oran_packets_api.h
 * Author:D. Matas 6.2019
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef I_BUILD_ORAN_PACKETS_API_H_
#define I_BUILD_ORAN_PACKETS_API_H_
#include "i_api_common.h"
#include "i_handler_api.h"

#define ORAN_PACKETS_OUT	   	   "packets_include_oran_"				  //out file include header(length+time) + oran packet (DL: C-Plane and U-Plane; UL: C-Plane only)
#define ORAN_PACKETS_OUT_PCAP	   "packets_pcap_"				          //PCAP file that include oran packet (DL: C-Plane and U-Plane; UL: C-Plane only)
#define ORAN_PACKETS_CPLANE_OUT_PCAP	   "packets_pcap_cplane_"				          //PCAP file that include oran packet (DL: C-Plane and U-Plane; UL: C-Plane only)
#define ORAN_PACKETS_UPLANE_OUT_PCAP	   "packets_pcap_uplane_"				          //PCAP file that include oran packet (DL: C-Plane and U-Plane; UL: C-Plane only)
#define ORAN_PACKETS_OUT_UL_PCAP   "packets_ul_pcap_"				          //PCAP file that include oran packet (DL: C-Plane and U-Plane; UL: C-Plane only)
#define ORAN_PACKETS_UL_UPLANE	   "packets_include_oran_ul_uplane_"	  //out file include header(length+time) + oran packet (UL: U-Plane only)    - separated from packets_include_oran_ file for UL path tests validation
#define ORAN_PACKETS_PRACH_UPLANE  "packets_include_oran_PRACH_uplane_"   //out file include header(length+time) + oran packet (PRACH: U-Plane only) - separated from above files for PRACH path tests validation

//#define DIFFERENT_LENGTH_PER_SYMBOL_DEPEND_CP

#define TIME_BETWEEN_TIME_SYNC			       	     5000 //12000 //for eth dpdk offline to proceed in case no packet is ready for transmission
#define PROGRESS_NS_BETWEEN_SYMBOLS_AVERAGE	         35715
#define PROGRESS_NS_BETWEEN_SYMBOLS_SYMNOL_0_7_LONG	 36193 //for symbol 0/7
#define PROGRESS_NS_BETWEEN_SYMBOLS_SYMNOL_NORMAL	 35673
#define TIME_TO_SHIFT_PCAP_DU   			     	 1000000   // 1msec

#define MAX_RANDOM_NUMBER_FOR_CPLANE_PROGRESS		(2500)
#define MAX_RANDOM_NUMBER_FOR_UPLANE_PROGRESS		(350)
#define ONE_SECOND_IN_USEC                  (1000000000)                                 // 100 Frame  = 1sec  = 1000 msec

#define PROGRESS_NS_BETWEEN_FRAMES          (10000000)                                   // 1 Frame    = 10msec
#define PROGRESS_NS_BETWEEN_SUBFRAMES       (1000000)                                    // 1 SubFrame = 1msec    = 2 Slots (mui = 1)
#define PROGRESS_NS_BETWEEN_SLOTS       	PROGRESS_NS_BETWEEN_SYMBOLS_AVERAGE*14
#define PROGRESS_NS_BETWEEN_SLOTS_ACCURATE	(PROGRESS_NS_BETWEEN_SLOTS - 10)             // 1 Slot     = 0.5 msec = 14 Symbols

#define PROGRESS_NS_BETWEEN_ORAN_UPLANES	10
#define PROGRESS_NS_WHILE_SLOT_TICK_DL	    13
#define PROGRESS_NS_WHILE_SLOT_TICK_UL	    17
#define PROGRESS_NS_WHILE_SLOT_TICK_PRACH   19

enum PacketType
{
	PayloadPacket = 0,  //common for C/U  plane DL/UL
	SlotTickDL,
	SlotTickUL,
	SymbolTickDL,
	SymbolTickUL,
	// PRACH
	SlotTickULPRACH,
	SymbolTickULPRACH,
};

/*******************************************************************************************//*!
*@class packet header for file
*@brief The purpose of this struct is :
*@brief envelope for O-RAN packet prepared by O-RAN module in order to write to file
***********************************************************************************************/
struct Packet_header_for_file //Oran or ASFH
{
	enum PacketType	packet_file_type;
	uint32_t 	packet_length;
	timespec  TimeStamp;
};


typedef struct {
	uint32_t slot_id;         //#1
	uint32_t subframe_id;     //#2
	uint32_t frame_id;        //#3
	uint32_t RB_offset;       //#4
	uint32_t RB_size;         //#5
	uint32_t layer_id;        //#6
	uint32_t RE_mask1;        //#7
	uint32_t beam_id1;        //#7   //new number if it's new, previous number if it was used
	uint32_t RE_mask2;        //#8
	uint32_t beam_id2;        //#9   //new number if it's new, previous number if it was used
	uint32_t symbol_offset;   //#10
	uint32_t prachOccurrence; // 11
//	uint32_t num_of_symbols; //#14

} Configuration_from_file_c_plane_ASFH;

typedef struct {
	uint32_t slot_id;        //#1
	uint32_t subframe_id;    //#2
	uint32_t frame_id;       //#3
	uint32_t RB_offset;      //#4
	uint32_t RB_size;        //#5
	uint32_t layer_id;       //#6
	uint32_t RE_mask;        //#7
	uint32_t beam_id;        //#8   //new number if it's new, previous number if it was used
	uint32_t beam_pointer;   //#9   //point to offset in complex! , 0xffff if use previous beam
	uint32_t section_id;     //#10
	uint32_t symbol_offset;  //#11
	uint32_t num_of_symbols; //#12
	uint32_t prachOccurrence;// #13

} Configuration_from_file_c_plane;

typedef struct
{
	uint32_t slot_id;          //#1
	uint32_t subframe_id;        //#1
	uint32_t frame_id;         //#2
	uint32_t RB_offset;        //#3
	uint32_t RB_size;          //#4
	uint32_t layer_id;         //#5
	uint32_t RE_mask;          //#6
	uint32_t beam_id;          //#7  //new number if it's new, previous number if it was used
	uint32_t beam_pointer;     //#8  //point to offset in complex! , 0xffff if use previous beam
	uint32_t section_id;       //#9
	uint32_t symbol_offset;    //#10
	uint32_t num_of_symbols;   //#11
//	uint32_t subframe_id;      //#12     // Add for PRACH use
//	uint32_t frequency_offset; //#13  TBD // Additional param in PRACH mode (KType3)
	// HEADER_ADDITIONAL_DATA
	//uint32_t time_offset;    //#14 TBD
	//uint32_t fft_size;	   //#15 TBD
	//uint32_t mu;             //#16 TBD
	//uint32_t cp_length;      //#17 TBD
	//uint32_t ud_comp_header; //#18 TBD

}Configuration_from_file_c_plane_PRACH;

typedef struct {
	uint32_t slot_id;
	uint32_t subframe_id;
	uint32_t frame_id;
	uint32_t RB_offset;
	uint32_t RB_size;
	uint32_t layer_id;
	uint32_t data_pointer; //offset in complex
	uint32_t section_id;
	uint32_t symbol_num;
} Configuration_from_file_u_plane;

typedef struct
{
    uint32_t beam_id;   //new number if it's new, previous number if it was used
    uint32_t beam_pointer;  //point to offset in complex! , 0xffff if use previous beam
    //int32_t num_of_weights_;        //    Number of Beamforming weights
    uint32_t  DataDirection; //    kUL = 0,kDL = 1 - for decide if ti take weights from BF_extensions or PE_extensions file. If you want -they can be joined to 1 file
}Configuration_from_file_weight_ASFH;

/*******************************************************************************************//*!
*@class CEthDriver_api
*@brief The purpose of this class is :
*@brief API for Ethernet driver
***********************************************************************************************/
struct CBuildOranPackets_api
{
    virtual void DLTestRunPrepare(char *dir) = 0;
     virtual void GetDLMessage(char **DL_config, char **TX_req) = 0;
    //virtual EResultT ResCompare() = 0;
};

struct CBuildASFHPackets_api
{
    virtual void DLTestRunPrepare(char *dir) = 0;
     virtual void GetDLMessage(char **DL_config, char **TX_req) = 0;
    //virtual EResultT ResCompare() = 0;
};

#endif /* ETH_DRIVER_API_H_ */
