/******************************************************************
 * i_pcap_parse_api.h
 * Author: Maya Levanovsky
 *  Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef API_MODULE_API_I_PCAP_PARSER_API_H_
#define API_MODULE_API_I_PCAP_PARSER_API_H_

#include <i_build_packets_api.h>
#include "global_defines.inc"
#include "i_api_common.h"
#include "i_cpp_module.h"
#include "i_ru_test_module_api.h"
#include <pcap.h>

#define PCAP_PARSER_MODULE	"PCAP_PARSER"

#define PCAP_INPUT_FIFO "PCAP_INPUT_FIFO"

#define	EMULATOR_NON_CLASSIFIED_QUEUE	   "EMULATOR_NON_CLASSIFIED_QUEUE"

// Name of input FIFO for all GL test-> dummy DPDK driver API (input fifo Ques from Oran packet to DPDK)

#define UNIFIED_ORAN_UPLANE_DL_LAYER_0_QUEUE   "UNIFIED_UPLANE_DL_LAYER_0_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_1_QUEUE   "UNIFIED_UPLANE_DL_LAYER_1_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_2_QUEUE   "UNIFIED_UPLANE_DL_LAYER_2_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_3_QUEUE   "UNIFIED_UPLANE_DL_LAYER_3_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_4_QUEUE   "UNIFIED_UPLANE_DL_LAYER_4_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_5_QUEUE   "UNIFIED_UPLANE_DL_LAYER_5_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_6_QUEUE   "UNIFIED_UPLANE_DL_LAYER_6_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_UPLANE_DL_LAYER_7_QUEUE   "UNIFIED_UPLANE_DL_LAYER_7_QUEUE_TO_DPDK"

#define UNIFIED_ORAN_CPLANE_LAYER_0_QUEUE   "UNIFIED_UPLANE_LAYER_0_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_1_QUEUE   "UNIFIED_UPLANE_LAYER_1_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_2_QUEUE   "UNIFIED_UPLANE_LAYER_2_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_3_QUEUE   "UNIFIED_UPLANE_LAYER_3_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_4_QUEUE   "UNIFIED_UPLANE_LAYER_4_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_5_QUEUE   "UNIFIED_UPLANE_LAYER_5_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_6_QUEUE   "UNIFIED_UPLANE_LAYER_6_QUEUE_TO_DPDK"
#define UNIFIED_ORAN_CPLANE_LAYER_7_QUEUE   "UNIFIED_UPLANE_LAYER_7_QUEUE_TO_DPDK"

#define ADDER_RESULTS_MEM_LAYER_0      "ADDER_RESULTS_MEM_LAYER_0"
#define ADDER_RESULTS_MEM_LAYER_1      "ADDER_RESULTS_MEM_LAYER_1"
#define ADDER_RESULTS_MEM_LAYER_2      "ADDER_RESULTS_MEM_LAYER_2"
#define ADDER_RESULTS_MEM_LAYER_3      "ADDER_RESULTS_MEM_LAYER_3"


enum PcapParserInputType
{
	kIQSamples = 0,
	kOranMessages,
	kOranMessagesFromDU
};


struct Pcap_Parser_API
{
    virtual EResultT PcapParserOranBuilder(char input_file_name[], FILE *f_oran_debug_ptr, PcapParserInputType input_type, uint32_t *msg_counter,uint8_t* flag)=0;
    virtual EResultT CreatePcapPktHdr(struct pcap_pkthdr* pcap_hdr,uint32_t pkt_length,timespec timestamp)=0;
	virtual void SendToFilePcap(uint32_t packet_length, timespec timestamp_includensec,uint8_t *payload ,pcap_dumper_t *dumper_pcap)=0;
	virtual EResultT GetPacketUnified(INGRESS_CLASSIFICATOR_Q_TYPE queue_type, uint8_t*& retrieved_data, DpdkTestModuleType* test_module_type_ptr)=0;
	virtual void GetAdderResultsMem(CMemArea* Adder_Results_Mem[])=0;

};

#endif /* API_MODULE_API_I_PCAP_PARSER_API_H_ */
