
/*-----------------------------------------------------------
 * i_fapi.h
 *
 * DAN Physical interface internal header
 *
 * The definitions are compatible with API version 1.1
 * as described in "LTE eNB L1 API Definition" docuemnt revision 1.11
 *
 * Copyright(c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * OWNER: Yosi Saggi
 *-----------------------------------------------------------
 */
#ifndef _I_FAPI_VS_GLOBAL_H
#define _I_FAPI_VS_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif
/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

#include "I_fapi.h"

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
 

/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
*/

/*********************************
*  Vendor Specific Tag Message Types
*********************************
*/
typedef enum
{
	FAPI_E_RPT_ULSCH_VSM	  			= 0x01,	// Associated with RX_ULSCH.Indication
	FAPI_E_HARQ_PUCCH_VSM	  			= 0x02,	// Associated with HARQ.Indication
	FAPI_E_HARQ_PUSCH_VSM	  			= 0x03,	// Associated with HARQ.Indication
	FAPI_E_RPT_SR_VSM		  			= 0x04,	// Associated with RX_SR.Indication
	FAPI_E_RPT_CQI_PUCCH_VSM  			= 0x05,	// Associated with RX_CQI.Indication
	FAPI_E_RPT_CQI_PUSCH_VSM  			= 0x06,	// Associated with RX_CQI.Indication
	FAPI_E_RPT_RACH_VSM	  				= 0x07, // Associated with RACH.Indication
	FAPI_E_RPT_SRS_VSM					= 0x08, // Associated with SRS.Indication
	FAPI_E_CFG_CFI_VSM 		  			= 0x0e,	// Associated with CONFIG.request
	FAPI_E_CFG_VERSION_VSM	  			= 0x0f, // Associated with CONFIG.request
	FAPI_E_CFG_ETM_VSM		  			= 0x10, // Associated with CONFIG.request (for Test Mode)
	FAPI_E_PARAM_RSP_MAX_TTI_VSM	  	= 0x11, // Associated with PARAM.response 
	FAPI_E_NI_REQ_VSM					= 0x12, // Associated with UL_CONFIG.request	
	FAPI_E_CFG_PHY_CHANGE_LIST_VSM		= 0x13, // change list num
	FAPI_E_CFG_PHY_COMPILATION_MODE_VSM	= 0x14, // compilation mode (mirroring and system mode)
	FAPI_E_CFG_API_VER_VSM	  			= 0x15, // Obsolete. Associated with CONFIG.request
	FAPI_E_PARAM_RSP_CAR_ID_VSM	  	    = 0x16, // Associated with PARAM.response
	FAPI_E_CFG_PHY_VERSION_VSM			= 0x17, // PHY SW/HW version
	FAPI_E_VSM_TAG_LAST
} FAPI_E_VSM_TYPE;

typedef enum
{
	FAPI_E_CAT_GEN  			= 0x00,	// FAPI general params
	FAPI_E_CAT_RF  				= 0x01,	// FAPI RF related params
	FAPI_E_CAT_PHICH  			= 0x02,	// FAPI PHICH related params
	FAPI_E_CAT_SCH  			= 0x03,	// FAPI SCH related params
	FAPI_E_CAT_PRACH  			= 0x04,	// FAPI PRACH related params
	FAPI_E_CAT_PUSCH  			= 0x05,	// FAPI PUSCH related params
	FAPI_E_CAT_PUCCH			= 0x06, // FAPI PUCCH related params
	FAPI_E_CAT_SRS				= 0x07, // FAPI SRS related params
	FAPI_E_CAT_UL_RS  			= 0x08,	// FAPI Uplink Reference Signal related params
	FAPI_E_CAT_TDD  			= 0x09, // FAPI TDD structure related params
	FAPI_E_CAT_PHY_CAPABILITY	= 0x0a, // FAPI PHY capabilities report
	FAPI_E_CAT_VSM  			= 0x0b, // Vendor Specific params
	FAPI_E_VSM_CFG_REP_CAT_LAST
} FAPI_E_VSM_CFG_REP_CAT;

/*****************************************
*  Common header that is included
*  at the beginning of all
*  FAPI Vendor Specific TLV message structure
******************************************
*/

typedef struct PACK_STRUCT FAPI_S_VSM_TLV_HDR {
	FAPI_UINT16			  tag;					 // TLV tag
	FAPI_UINT8			  length; 				 //Length in bytes of TLV including tag, length, padding and value fields 
	FAPI_UINT8			  padding;				 // Alignment padding
} FAPI_T_VSM_TLV_HDR;


typedef struct PACK_STRUCT FAPI_S_VSM_HDR {
	FAPI_UINT16			 				num_tlv;//Number of TLVs included in this message
	FAPI_UINT16			 				size;	//Size in bytes of vendor specific message body including num_tlv, size and included TLVs 
} FAPI_T_VSM_HDR;

typedef struct PACK_STRUCT FAPI_S_VSM_CFG_REP_TLV {
	FAPI_T_VSM_TLV_HDR	 	tlv_hdr;			 // VSM TLV Header: tag + length
	FAPI_UINT32			 	value;				 // Value to report
} FAPI_T_VSM_CFG_REP_TLV;


typedef struct PACK_STRUCT FAPI_S_VSM_CFG_REP_CAT {
	FAPI_UINT16			 	category;			 // TLV category. one of FAPI_E_VSM_CFG_REP_CAT
	FAPI_UINT16			 	num_tlv;			 // Number of TLVs included in this message
	FAPI_T_VSM_CFG_REP_TLV	tlvs[];				 // array of CFG Report TLVs
} FAPI_T_VSM_CFG_REP_CAT;

typedef struct PACK_STRUCT FAPI_S_VSM_CFG_REPORT {
	FAPI_T_MSG_HDR		   	msg_hdr;
	FAPI_UINT8				n_categories;        // Number of categories reported
	FAPI_UINT8				padding[3];          // Size of the message - including 'n_categories' and 'size'
	FAPI_T_VSM_CFG_REP_CAT	categories[];        // array of CFG Report categories
}FAPI_T_VSM_CFG_REPORT;

/********************************************
*  FAPI Physical Interface Vendor Specific Messages
*********************************************
*/

// RX.ULSCH.Indication:
typedef struct PACK_STRUCT FAPI_S_RX_ULSCH_IND_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					rssi; 			//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8 					padding;		//Alignment padding
} FAPI_T_RX_ULSCH_IND_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_RX_ULSCH_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR					tag_header;		//VS tag header
	FAPI_T_RX_ULSCH_IND_VSM_TAG_VALUE	value;
} FAPI_T_RX_ULSCH_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_RX_ULSCH_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_RX_ULSCH_IND_VSM_TAG	tlvs[];			//TLVs
} FAPI_T_RX_ULSCH_IND_VSM_MSG;

// HARQ.Indication:
typedef struct PACK_STRUCT FAPI_S_HARQ_IND_PUCCH_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					rssi; 			//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8 					snr;			//Snr (0..120 - -20:0.5:40 [dB])
} FAPI_T_HARQ_IND_PUCCH_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_HARQ_IND_PUSCH_FDD_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					padding[2];		//Alignment padding
} FAPI_T_HARQ_IND_PUSCH_FDD_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_HARQ_IND_PUSCH_TDD_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8					bundling;		// The best matched number of ack/nack per layer 0..3
	FAPI_UINT8 					padding;		//Alignment padding
} FAPI_T_HARQ_IND_PUSCH_TDD_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_HARQ_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR							tag_header;		//VS tag header
	union {
		FAPI_T_HARQ_IND_PUCCH_VSM_TAG_VALUE		pucch;		
		FAPI_T_HARQ_IND_PUSCH_FDD_VSM_TAG_VALUE	pusch_fdd;
		FAPI_T_HARQ_IND_PUSCH_TDD_VSM_TAG_VALUE pusch_tdd;	
	}											value;			// All data-structures whithin value are of the same size		
} FAPI_T_HARQ_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_HARQ_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_HARQ_IND_VSM_TAG		tlvs[];			//TLVs
} FAPI_T_HARQ_IND_VSM_MSG;

// SR.Indication
typedef struct PACK_STRUCT FAPI_S_SR_IND_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					rssi; 			//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8 					snr;			//Snr (0..120 - -20:0.5:40 [dB])
} FAPI_T_SR_IND_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_SR_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR			tag_header;		//VS tag header
	FAPI_T_SR_IND_VSM_TAG_VALUE	value;
} FAPI_T_SR_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_SR_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_SR_IND_VSM_TAG		tlvs[];			//TLVs
} FAPI_T_SR_IND_VSM_MSG;

// CQI.Indication:
typedef struct PACK_STRUCT FAPI_S_CQI_IND_PUCCH_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					rssi; 			//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8 					padding;
} FAPI_T_CQI_IND_PUCCH_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_CQI_IND_PUSCH_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					padding[2];		//Alignment padding
} FAPI_T_CQI_IND_PUSCH_VSM_TAG_VALUE;

typedef struct PACK_STRUCT FAPI_S_CQI_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR							tag_header;		//VS tag header
	union {
		FAPI_T_CQI_IND_PUCCH_VSM_TAG_VALUE		pucch;		
		FAPI_T_CQI_IND_PUSCH_VSM_TAG_VALUE		pusch;
	}											value;			// All data-structures whithin value are of the same size		
} FAPI_T_CQI_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_CQI_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_CQI_IND_VSM_TAG		tlvs[];			//TLVs
} FAPI_T_CQI_IND_VSM_MSG;


// RACH.Indication
typedef struct PACK_STRUCT FAPI_S_RACH_IND_VSM_TAG_VALUE {
	FAPI_UINT8					preamable_id;		//The detected preamble (0..63)
	FAPI_UINT8 					rssi; 				//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8 					detection_metric;	//(0..63 0:1:63 [dB])
	FAPI_UINT8					padding;			//Alignment padding 
} FAPI_T_RACH_IND_VSM_TAG_VALUE;


typedef struct PACK_STRUCT FAPI_S_RACH_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR				tag_header;		//VS tag header
	FAPI_T_RACH_IND_VSM_TAG_VALUE	value;
} FAPI_T_RACH_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_RACH_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_RACH_IND_VSM_TAG		tlvs[];			//TLVs
} FAPI_T_RACH_IND_VSM_MSG;

// SRS.Indication
typedef struct PACK_STRUCT FAPI_S_SRS_IND_VSM_TAG_VALUE {
	FAPI_UINT16					rnti;			//RNTI number for which the following indications are related (0.65535)
	FAPI_UINT8 					rssi; 			//Signal strength Integrated over all the received antennas and normalized to full BW (0:1:130 - -130:1:0 [dBm])
	FAPI_UINT8					padding;		//Alignment padding 
} FAPI_T_SRS_IND_VSM_TAG_VALUE;


typedef struct PACK_STRUCT FAPI_S_SRS_IND_VSM_TAG {
	FAPI_T_VSM_TLV_HDR				tag_header;		//VS tag header
	FAPI_T_SRS_IND_VSM_TAG_VALUE	value;
} FAPI_T_SRS_IND_VSM_TAG;

typedef struct PACK_STRUCT FAPI_S_SRS_IND_VSM_MSG {
	FAPI_T_VSM_HDR 				vsm_header; 	//VS Message header
	FAPI_T_SRS_IND_VSM_TAG		tlvs[];			//TLVs
} FAPI_T_SRS_IND_VSM_MSG;

//
// New MEASUREMENT.Indication FAPI message, transmitted each frame and contains different RF conditions
// Occupies the message type of 0x8c which is reserved by the FAPI standard
//
typedef struct PACK_STRUCT FAPI_S_RX_MEASUREMENT_IND {
	FAPI_T_MSG_HDR		   		msg_hdr; 	   		//Message header
	FAPI_UINT16					sf_sfn;				//SF, SFN at which the reported message is related [15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	FAPI_UINT8					avg_ni;				//Noise and Interference Signal strength measured and averaged over all the non-allocated resources. This value is integrated over all the received antennas and normalized to full BW 0:1:130 -130:1:0 [dBm]
	FAPI_UINT8             		avg_ant1_rssi;      //Received signal strength from antenna 1 averaged over all scheduled data RBs in this SF as measured by PHY. Integer value representing: [-130:1:0]dBm (0..130)
    FAPI_UINT8             		avg_ant2_rssi;      //Received signal strength from antenna 2 averaged over all scheduled data RBs in this SF as measured by PHY. Integer value representing: [-130:1:0]dBm (0..130)
    FAPI_UINT8             		avg_ant3_rssi;      //Received signal strength from antenna 3 averaged over all scheduled data RBs in this SF as measured by PHY. Integer value representing: [-130:1:0]dBm (0..130)
    FAPI_UINT8             		avg_ant4_rssi;      //Received signal strength from antenna 4 averaged over all scheduled data RBs in this SF as measured by PHY. Integer value representing: [-130:1:0]dBm (0..130)
    FAPI_UINT8               	rx_ant1_gain;       //Rx gain of antenna 1 in dB. (0..100, 0xFF if not present).
    FAPI_UINT8               	rx_ant2_gain;       //Rx gain of antenna 2 in dB. (0..100, 0xFF if not present).
    FAPI_UINT8               	rx_ant3_gain;       //Rx gain of antenna 3 in dB. (0..100, 0xFF if not present).
    FAPI_UINT8               	rx_ant4_gain;       //Rx gain of antenna 4 in dB. (0..100, 0xFF if not present).
    FAPI_UINT8			padding;					//Alignment padding
} FAPI_T_RX_MEASUREMENT_IND;

//
// New NI.Indication FAPI message, transmitted when VSM NI_PDU is recieved.
// Occupies the message type of 0x8d which is reserved by the FAPI standard
//
typedef struct PACK_STRUCT FAPI_S_RX_NI_PRB {
	uint8_t						ni; 			//Each RB reports one NI Value, range 0:1:130, 0xFF when the PRB is allocated to PRACH or PUCCH,-130:1:0 [dBm]
} FAPI_T_RX_NI_PRB;

typedef struct PACK_STRUCT FAPI_S_RX_NI_IND {
	FAPI_T_MSG_HDR		   				msg_hdr;   		//Message header
	FAPI_UINT32							handle; 		//An opaque handling returned in the NI.indication
	FAPI_UINT16							sf_sfn;			//SFN/SF at which the reported measurements were performed [15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	FAPI_UINT16							n_rb;			//Number of UL RBs/NI Reports according with BW, range 1:100
	FAPI_T_RX_NI_PRB					rb[];
} FAPI_T_RX_NI_IND;


/*** Vendor Specific messages ***/ 
typedef struct PACK_STRUCT FAPI_S_VSM_TLV {
	FAPI_T_VSM_TLV_HDR					tag_header;		//VS tag header
	FAPI_UINT32							value[];
} FAPI_T_VSM_TLV;

typedef struct PACK_STRUCT FAPI_S_VSM_MSG {
	FAPI_T_VSM_HDR 						vsm_header; 	//VS Message header
	FAPI_T_VSM_TLV						tlvs[];			//TLVs
} FAPI_T_VSM_MSG;

/*** Vendor Specific UL CONFIG REQUEST messages ***/
typedef struct PACK_STRUCT FAPI_S_NI_PDU_VSM_VALUE {
	FAPI_UINT32							handle;			//An opaque handling returned in the NI.indication
} FAPI_T_NI_PDU_VSM_VALUE;


/*** Vendor Specific Configuration messages ***/ 
//CONFIG.request
//CFI
typedef struct PACK_STRUCT FAPI_S_CFG_CFI_VSM_TAG_VALUE {
	FAPI_UINT8							cfi;			//Number of symbols for DL control (1-4)
	FAPI_UINT8							padding[3];		//Alignment padding 
} FAPI_T_CFG_CFI_VSM_TAG_VALUE;

//Version
typedef struct PACK_STRUCT FAPI_S_CFG_VERSION_VSM_TAG_VALUE {
	FAPI_UINT16							vs_major_ver;	//Major version number of Vendor Specific API  (0 - 65535)
	FAPI_UINT8							vs_minor_ver;	//Minor version number of Vendor Specific API  (0 - 255)
	FAPI_UINT8							padding;		//Alignment padding 
} FAPI_T_CFG_VERSION_VSM_TAG_VALUE;

//Test Mode
typedef struct PACK_STRUCT FAPI_S_CFG_ETM_VSM_TLV {
	FAPI_UINT8							test_mode_enabled; 	//Whether to this a Test Mode run
	FAPI_UINT8							disable_all_zeros;	//Whether to disable the all-zeros mode even though PHY is running in Test Mode
	FAPI_UINT8							disable_mi_override;//Whether to disable the hard-coded PHICH MI even though PHY is running in Test Mode
	FAPI_UINT8							padding[1];			//Alignment padding 
} FAPI_T_CFG_ETM_VSM_TLV;

//PARAM.response
//Maximum users per TTI
typedef struct PACK_STRUCT FAPI_S_MAX_USERS_VSM_VALUE {
	FAPI_UINT8							max_dl_users;  	//Maximum users per TTI for each DL request 
														//(different RNTIs not including public channels)
	FAPI_UINT8							max_ul_users;  	//Maximum users per TTI for each UL request 
														//(different RNTIs not including public channels)
	FAPI_UINT8							padding[2];		//Alignment padding 
} FAPI_T_MAX_USERS_VSM_VALUE;

//Primary LTE carrier number
typedef struct PACK_STRUCT FAPI_S_PARAM_RSP_LTE_CAR_ID {
	FAPI_UINT32                         carrier_id;
} FAPI_T_PARAM_RSP_LTE_CAR_ID;

//PHY version
typedef struct PACK_STRUCT FAPI_S_PHY_CHANGE_LIST_TLV {
	FAPI_UINT32							cl_num;		//Maximum users per TTI for each UL/DL request 
} FAPI_T_PHY_CHANGE_LIST_TLV;

typedef struct PACK_STRUCT FAPI_S_PHY_VERSION_TLV {
	FAPI_UINT8  						chipnum;
	FAPI_UINT8  						feature_release;
	FAPI_UINT8  						release_candidate;
	FAPI_UINT8  						snapshot;
} FAPI_T_PHY_VERSION_TLV;

typedef struct PACK_STRUCT FAPI_S_PHY_COMPILATION_MODE_TLV {
	FAPI_UINT8							system_mode;		//Maximum users per TTI for each UL/DL request 
	FAPI_UINT8							mirroring_mode;		//Maximum users per TTI for each UL/DL request 
	FAPI_UINT8							padding[2];		//Maximum users per TTI for each UL/DL request 
} FAPI_T_PHY_COMPILATION_MODE_TLV;

/*
 * -----------------------------------------------------------
 * Platform-specific section
 * -----------------------------------------------------------
 */

#ifdef PLATFORMNAME
	#if (PLATFORMNAME == V3_CDP)
		#include "Platforms/V3/Cores/API/I_fapi_vsm.h"
	#elif (PLATFORMNAME == V4_CDP)
		#include "Platforms/V4/Cores/API/I_fapi_vsm.h"
	#else
	#error Unknown platform
	#endif
#else
#error Platform is not defined
#endif /* PLATFORMNAME */

#ifdef __cplusplus
}
#endif

#endif //_I_FAPI_VS_GLOBAL_H

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */


