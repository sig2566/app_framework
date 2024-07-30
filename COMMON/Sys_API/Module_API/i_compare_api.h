/*****************************************************************
 * i_compare_api.h
 *
 *  Created on: May 5, 2020
 *      Author: m.levanovsky
 *      *
 * Copyright (c) 2020 - 2022 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *
 *****************************************************************/
#ifndef COMPARE_API_H_
#define COMPARE_API_H_
#include "global_defines.inc"
#include "i_api_common.h"
#include "i_cpp_module.h"
#include "i_split_api.h"
#include "i_ru_test_module_api.h"
struct CompareResult
 {
 	uint32_t worst_snr_cnt;
 	 float worst_snr_db;
 	 float average_snr_db;
 };
enum COMPARE_UPLANE_RESULT
{
	SEQ_ID_ERROR,
	DU_PORT_ID_ERROR,
	BAND_SECTOR_ID_ERROR,
	CARRIER_COMPONENT_ID_ERROR,
	STREAM_ID_ERROR,
	FRAME_ID_ERROR,
	SUBFRAME_ID_ERROR,
	SLOT_ID_ERROR,
	SYMBOL_ID_ERROR,
	//compare of section payload
	ALLOCATION_ID_ERROR,
	EVERY_OTHER_RB_ERROR,
	SYM_INC_ERROR,
	START_RB_ERROR,
	NUM_OF_RBS_ERROR,
	SAMPLE_ERROR,//message descriptor is same, but IQ sample is different
	SAMPLE_SAME //Everything is bit-exact
	};
 struct Compare_Resluts_Api{
	 virtual CompareResult CompareComplex(Complex *out_fix,Complex *ref_float, uint32_t vector_length_for_compare)=0;
	 virtual EResultT ResCompareAntGrpDL(uint16_t slot_number,uint32_t symbol_number, uint16_t ant_group_num,Complex *out_of_ant_group, Complex ref_data[],char mod_name[],char test_suffix[])=0;
	 virtual void PrintUlUplaneClassifiersInfo(MESSAGE_DESCRIPTOR* msg_classifier_c,MESSAGE_DESCRIPTOR* msg_classifier_ref,enum COMPARE_UPLANE_RESULT uplane_comp)=0;
	 virtual enum COMPARE_UPLANE_RESULT ComparePacketUplaneUL(MESSAGE_DESCRIPTOR *msg_classifier_c,MESSAGE_DESCRIPTOR *msg_classifier_ref,DATA_DESCRIPTOR *current_data_message_c,
	    		DATA_DESCRIPTOR *current_data_message_ref,char mod_name[],char test_suffix[])=0;
	 virtual void PrintMissingPackets(int32_t c_plane_ul, int32_t c_plane_dl, int32_t u_plane_dl)=0;

 };


#endif /* COMPARE_API_H_ */
