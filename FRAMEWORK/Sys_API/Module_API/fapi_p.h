

#ifndef HANDLER_FAPI_P_H
#define HANDLER_FAPI_P_H
#include"gnb_l1_l2_api.h"
#include <iostream>
#include <vector>
#ifdef __cplusplus
extern "C" {
#endif

/**************************************
 *	DEFINES
 */
#define PACK_STRUCT __attribute__((packed))
#define RBS_MASK_SIZE 9
/********************************************
 *  FAPI_P Physical Interface API Enumerators
 ********************************************
 */
#ifndef FAPI_P_UINT8
#define FAPI_P_UINT8 		unsigned char
#endif
#ifndef FAPI_P_UINT16
#define FAPI_P_UINT16 		unsigned short
#endif
#ifndef FAPI_P_UINT32
#define FAPI_P_UINT32		unsigned int  //long
#endif
#ifndef FAPI_P_INT8
#define FAPI_P_INT8 		signed char
#endif
#ifndef FAPI_P_INT16
#define FAPI_P_INT16 		signed short
#endif
#ifndef FAPI_P_INT32
#define FAPI_P_INT32		signed int  //long
#endif
#ifndef FAPI_P_BOOL
#define FAPI_P_BOOL			unsigned long
#endif
#ifndef FAPI_P_TBOOL
#define FAPI_P_TBOOL		unsigned char
#endif

/*********************************
 *  MAC-PHY Interface message
 *  types
 *********************************
 */

//#define MAX_MESSAGE_LENGTH  100000
typedef enum {
	//FAPI_P_E_PARAM_REQ = 0x00,
	//FAPI_P_E_PARAM_RSP = 0x01,
	FAPI_P_E_CONFIG_REQ = MSG_TYPE_PHY_CONFIG_REQ,
	FAPI_P_E_CONFIG_RSP = MSG_TYPE_PHY_CONFIG_RESP,
	FAPI_P_E_START_REQ = MSG_TYPE_PHY_START_REQ,
	FAPI_P_E_START_RSP = MSG_TYPE_PHY_START_RESP,
	FAPI_P_E_STOP_REQ = MSG_TYPE_PHY_STOP_REQ,
	FAPI_P_E_STOP_RSP = MSG_TYPE_PHY_STOP_RESP,
	//FAPI_P_E_UE_CONFIG_REQ = 0x07,
	//FAPI_P_E_UE_CONFIG_RSP = 0x08,
	FAPI_E_ERROR_IND = MSG_TYPE_PHY_ERR_IND,

	FAPI_P_E_DL_CONFIG_REQ = MSG_TYPE_PHY_DL_CONFIG_REQ,
	FAPI_P_E_UL_CONFIG_REQ = MSG_TYPE_PHY_UL_CONFIG_REQ,
	FAPI_P_E_UL_DCI_CONFIG_REQ = 0X86,
	//FAPI_E_SF_IND = 0x90,
	FAPI_P_E_TX_REQ = MSG_TYPE_PHY_TX_REQ,
	FAPI_E_RX_ULSCH_IND = MSG_TYPE_PHY_RX_ULSCH_IND,
	FAPI_E_SLOT_IND = MSG_TYPE_PHY_SLOT_IND,   //New TTI indicator for 5G
	FAPI_E_PRACH_IND = MSG_TYPE_PHY_RX_RACH_IND,
	FAPI_E_SRS_IND = MSG_TYPE_PHY_RX_SRS_IND,


} FAPI_P_E_MSG_TYPE;

typedef enum {

	//DL CONFIG messages
	FAPI_P_E_DL_DCI_PDU = 0,
	FAPI_P_E_DLSCH_PDU = 1,
	FAPI_P_E_CSI_RS_PDU = 2,
	FAPI_P_E_BCH_PDU = 3,

	//UL CONFIG messages

	FAPI_P_E_ULSCH_PDU = 0,
	FAPI_P_E_ULCCH_UCI_PDU = 1,
	FAPI_P_E_ULRACH_PDU = 2,
	FAPI_P_E_SRS_PDU = 3,
} FAPI_P_E_PDU_TYPE;

/*********************************
 *  DL config PDU types
 *********************************
 */
typedef enum {
//	FAPI_P_E_DL_DCI_PDU	    = 0,
//	FAPI_P_E_DL_BCH_PDU 	= 1,
	FAPI_P_E_DL_DLSCH_PDU = 3,
	FAPI_P_E_DL_PCH_PDU = 4,

} FAPI_P_E_DL_CONFIG_PDU_TYPE;
// UL config PDU types

// DL DCI FORMAT types
/*
 typedef enum
 {
 FAPI_P_E_DL_DCI_1_0		= 0,
 FAPI_P_E_DL_DCI_1_1 	= 1,
 }FAPI_P_E_DL_DCI_FORMAT_TYPE;
 */

// UL config PDU types
typedef enum {
	FAPI_P_E_UL_ULSCH_PDU = 0,
//...............................
//FAPI_E_UL_ULSCH_CSI_UCI_HARQ_PDU= 15,
//FAPI_E_UL_MAX_PDUS_NUM,

} FAPI_P_E_UL_CONFIG_PDU_TYPE;

/************************************************
 * Common header that is included at the
 * beginning of all FAPI_P MAC-PHY messages
 ************************************************
 */
/*
typedef struct PACK_STRUCT FAPI_P_S_MSG_HDR {
	FAPI_P_UINT8 type;           		//Message type ID
	FAPI_P_UINT8 len_ven;   	//Length of vendor-specific message body (bytes)
	FAPI_P_UINT32 len;           		//Length of message body (bytes)
} FAPI_P_T_MSG_HDR;
*/
#define FAPI_P_T_MSG_HDR L1L2MessageHdr
/************************************************
 *
 ************************************************
 */

//PDSCH timing mapping definitions 38.214 - 5.1.2.1
//-------------------------------------------------------
//enum PDSCH_time_mapping_type - is in i_dmrs_api.h
typedef struct PACK_STRUCT FAPI_P_S_SYMB_ALLOC {
	FAPI_P_UINT8 slot_number_k0;  //0 (Mu=0) or  0/1 (in Mu=1)
	FAPI_P_UINT8 start_symbol;  //0-13
	FAPI_P_UINT8 symbol_length;  //1-14
} FAPI_P_T_SYMB_ALLOC;

/************************************************
 *
 ************************************************
 */
//According to TS_38.211 p:7.4.1.1
enum EDL_DMRS_pos {
	DMRS_POS0, DMRS_POS1, DMRS_POS2, DMRS_POS3, DMRS_POS_SIZE
};

enum EDL_DMRS_config_type {
	DMRS_TYPE1 = 1, DMRS_TYPE2 = 2
};

//PDSCH timing mapping definitions 38.214 - 5.1.2.1
//-------------------------------------------------------
enum PDSCH_time_mapping_type //mainly for DMRS , but has another usage
{
	PDSCH_TIME_ALLOCATION_TYPE_A = 0, PDSCH_TIME_ALLOCATION_TYPE_B = 1
};

enum EDmrsTypeAPos {
	DMRS_TA_2 = 2, DMRS_TA_3 = 3
};

/*
 <DMRS description='PDSCH Configuration'>
 <DmrsConfigType description='Reference signal configuration type: 1,2'>1</DmrsConfigType>
 <DmrsAddPos description='Reference signal add position: 0/1/2/3'>1</DmrsAddPos>
 <DmrsMappingType description='Reference signal mapping type: A, B'>A</DmrsMappingType>
 <DmrsTypeAPos description='Type A Position: 2 or 3'>3</DmrsTypeAPos>
 <EnableDoubleSymbolDMRS description='Enable Double symbols DMRS'>0</EnableDoubleSymbolDMRS>
 </DMRS>
 */
//DMRS configuration data 38.211-7.4.1.1
//System time indicator


typedef struct PACK_STRUCT FAPI_P_S_DMRS_CFG {
	FAPI_P_INT32 n_scid; //Possible values 0,1 if it is -1, then scrambling ID data was not sent from L2
	FAPI_P_UINT32 n_id_scid;
	FAPI_P_UINT32 EnableScramblingIDDMRS;
	EDL_DMRS_pos dmrs_add_pos;
	EDL_DMRS_config_type dmrs_config_type;
	PDSCH_time_mapping_type dmrs_mapping_type;
	EDmrsTypeAPos dmrs_type_a_pos;
	FAPI_P_UINT32 enable_double_symbol_dmrs;

} FAPI_P_T_DMRS_CFG;

/************************************************
 * Common PDU fields included at the beginning of
 * every PDU in FAPI_P MAC-PHY messages
 ************************************************
 */
//Length of PDU is defined as DLSCH  length for debugging purpose
typedef struct PACK_STRUCT FAPI_P_S_PDU {
	FAPI_P_UINT8 pdu_type;				// PDU type. Each pdu has its own type
	FAPI_P_UINT8 pdu_size;// Size of the PDU control information (in bytes). This length value includes the 2 bytes required for the PDU type and PDU size parameters.
	//FAPI_P_UINT8				pdu[];					// The actual PDU structure
} FAPI_P_T_PDU;

/************************************************
 * DL_CONFIG.request
 ************************************************/
typedef struct PACK_STRUCT FAPI_P_S_DL_CONFIG_REQ {
	FAPI_P_T_MSG_HDR msg_hdr; 	   		//Message header
	SFN_SlotStruct sf_sfn;	//[15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	FAPI_P_UINT16 length;//The length of the downlink subframe configuration. Range 0...65535
	FAPI_P_UINT8 n_pdcch_symb;//The number of OFDM symbols for the PDCCH. Range 0...4
	FAPI_P_UINT8 n_dci;	//The number of DCI PDUs included in this message. Range 0...255
	FAPI_P_UINT16 n_pdu;//Number of PDUs that are included in this message. Range 0...514
	FAPI_P_UINT8 n_pdsch_rnti;		//Number of unique RNTIs sent on the PDSCH.
									//- a PCH PDU will have an unique RNTI and should be included in this value
									//- a DLSCH PDU can be one transport block sent to a UE with an unique RNTI. This RNTI should be included in this value
									//- a DLSCH PDU can be one of two transport blocks sent to a UE. In this case the two DLSCH PDUs will share the same RNTI. Only one RNTI should be included in this value.
	FAPI_P_T_PDU dl_pdu[];
} FAPI_P_T_DL_CONFIG_REQ;

/************************************************
 *
 ************************************************
 */
typedef struct PACK_STRUCT FAPI_P_S_DLSCH_PDU_A {
	FAPI_P_UINT32 length; //The length (in bytes) of the associated MAC PDU, delivered in TX.request. This should be the actual length of the MAC PDU, which may not be a multiple of 32-bits.
	//length can be calculated from rb_coding (number of RB's) , MCS , NUMBER_OF_LAYERS
	FAPI_P_UINT16 pdu_idx; //This is a count value which is incremented every time a BCH, MCH, PCH or DLSCH PDU is included in the DL_CONFIG.request message. This value is repeated in TX.request and associates the control information to the data. It is reset to 0 for every subframe. Range 0...65535
	FAPI_P_UINT16 rnti;	//The RNTI used for identifying the UE when receiving the PDU. Valid for all DCI formats. Value: 1...65535.
	FAPI_P_UINT8 ra_type;//Resource allocation type/header. Valid for DCI formats: 1,2,2A. 0=type 0, 1=type 1
	FAPI_P_UINT8 vir_rb_flag;//Type of virtual resource block used. Valid for DCI formats: 1A,1B,1D, 0 = localized, 1 = distributed
	FAPI_P_UINT32 rb_bitmap[RBS_MASK_SIZE]; FAPI_P_UINT8 MCS;// convinent for 5G - contain modulation, but forcase  UE does not relevant MCS.
	FAPI_P_UINT8 rv;//HARQ redundancy version. This should match the value sent in the DCI Format PDU which allocated this grant. Value: 0...3.
	FAPI_P_UINT8 trans_scheme; 		//The MIMO mode used in the PDU
	FAPI_P_UINT8 n_layers; 			//The number of layers used in transmission
	FAPI_P_UINT8 Enable256QAM; //	similar to	altCQI_table_r12;	//Indicates if ITBS and Modulation order use alternative CQI table Value: 1=Alternative table, 0=Regular table
	FAPI_P_T_DMRS_CFG DMRS_parameters;
	FAPI_P_T_SYMB_ALLOC SYMB_parameters;
	//FAPI_P_UINT8					codebook_idx[];		//Only valid when transmission scheme = 3, 4, 5. Defines the number of subbands and codebooks used for PMI. If value=1 then a single PMI value is supplied which should be used over all RB. Value 0...13
} FAPI_P_T_DLSCH_PDU_A;

typedef struct PACK_STRUCT FAPI_P_S_DLSCH_ALL_2_4 {
	FAPI_P_T_DLSCH_PDU_A dlschpdu_a;
	//   FAPI_P_T_DLSCH_PDU_B dlschpdu_b;
	//   FAPI_P_T_BEAM_FORMING_INFO beamforminginfo;
	//   FAPI_P_T_DLSCH_PDU_C dlschpdu_c;
} FAPI_P_T_DLSCH_ALL_2_4;

/************************************************
 * TX.request
 * TX Request PDU (FAPI 2.4+)
 * TX Request PDU (FAPI 1.1)
 * Includes at least one TLV
 ************************************************
 */
// TX_REQ TLV Type (Tag options)
typedef enum {
	FAPI_P_E_TLV_VALUE = 0, FAPI_P_E_TLV_POINTER = 1,
} FAPI_E_TX_REQ_TLV_TYPE;

typedef struct PACK_STRUCT FAPI_P_S_TLV {
	FAPI_P_UINT16 tag; //Range 0...1. 0: Payload is carried directly in the value field, 1: Pointer to payload is in the value field
	FAPI_P_UINT16 length; //Length of the actual payload in bytes, without the padding bytes
	FAPI_P_UINT8 *value; //Always a multiple of 32-bits. Tag=0: Only the most significant bytes of the size indicated by length field are valid. Remaining bytes are zero padded to the nearest 32-bit boundary. Tag=1: Pointer to the payload. Occupies 32-bits
} FAPI_P_T_TLV;

typedef struct PACK_STRUCT FAPI_P_S_TX_REQ_PDU {
	FAPI_P_UINT32 pdu_length; //The total length (in bytes) of the PDU description and PDU data, without the padding bytes.
	FAPI_P_UINT16 pdu_idx; //This is a count value which starts from 0. It is incremented for each BCH, MCH, PCH or DLSCH PDU. This value was included in TX.request and associates the data to the control information. It is reset to 0 for every subframe. Range 0...65535
	FAPI_P_UINT16 n_tlv; //The number of TLVs describing the data of the transport block.
	FAPI_P_T_TLV tlv[];              //Always a multiple of 32-bits.
} FAPI_P_T_TX_REQ_PDU;
/************************************************
 * TX.request
 * For (FAPI 2.4+) and (FAPI 1.1)
 * Includes at least one TX_REQ_PDU
 ************************************************
 */
typedef struct PACK_STRUCT FAPI_P_S_TX_REQ {
	FAPI_P_T_MSG_HDR msg_hdr;            //Message header
	SFN_SlotStruct sf_sfn;             //[15:4] SFN, range 0..1023
	FAPI_P_UINT16 n_pdu;     //Number of PDUs that are included in this message.
	FAPI_P_T_TX_REQ_PDU pdu[];
} FAPI_P_T_TX_REQ;

//Uplink
/************************************************
 * UL_CONFIG.request
 * For (FAPI 2.4+) and (FAPI 1.1)
 * Includes any or all of the PDUs described above
 ************************************************
 */

typedef struct PACK_STRUCT FAPI_P_S_UL_CONFIG_REQ {
	FAPI_P_T_MSG_HDR msg_hdr;            //Message header
	SFN_SlotStruct sf_sfn;     //[15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	FAPI_P_UINT16 length; //The length of the uplink subframe configuration. Range 0...65535
	FAPI_P_UINT8 n_pdu;   //The number of UL SCHs PDUs included in this message.
	FAPI_P_UINT8 rach_prach_frq_res; //If semi-static information is held in the MAC For FDD:
									 //0-No RACH in this subframe,1-RACH present in this subframe.
									 //For TDD: Bits 0:5 indicate which RACH frequency resources are used in this subframe, see [8] section 5.7.1.
									 //0-This RACH frequency index is not used 1-This RACH frequency index is used.
									 //If semi-static information is held in the PHY this parameter is ignored
	FAPI_P_UINT8 srs_present; //If semi-static information is held in the MAC:  0-No SRS in this subframe, 1-SRS present in this subframe. If semi-static information is held in the PHY this parameter is ignored.
	FAPI_P_T_PDU ul_pdu[];
} FAPI_P_T_UL_CONFIG_REQ;
/************************************************
 * UL_CONFIG.request
 * ULSCH PDU (FAPI 2.4+), Release 8 Fields
 * ULSCH PDU (FAPI 1.1), Release 8 Fields
 ************************************************
 */
typedef struct PACK_STRUCT FAPI_P_S_ULSCH_PDU_R8 {
//    FAPI_UINT32                 handle;             //An opaque handling returned in the RX.indication
//    FAPI_UINT16                 size;               //The size of the ULSCH PDU in bytes as defined by the relevant UL grant. The size can be 0 if UCI over ULSCH without data is configured. The size of CQI/RI/HARQ are not added to this element
	FAPI_P_UINT32 TB_size; //The size of TB MAC expected to receive from PDU (in bytes). Value: 1...65535
	FAPI_P_UINT16 rnti; //The RNTI used for identifying the UE when receiving the PDU. Value: 1...65535
	FAPI_P_UINT32 rb_bitmap[RBS_MASK_SIZE];
	FAPI_P_UINT8 MCS; // convinent for 5G - contain modulation, but forcase  UE does not relevant MCS.
	FAPI_P_UINT8 rv; //HARQ redundancy version. This should match the value sent in the DCI Format PDU which allocated this grant. Value: 0...3.
	FAPI_P_UINT8 trans_scheme;      //The MIMO mode used in the PDU
	FAPI_P_UINT8 n_layers;          //The number of layers used in transmission
	FAPI_P_UINT8 Enable256QAM; //   similar to  altCQI_table_r12;   //Indicates if ITBS and Modulation order use alternative CQI table Value: 1=Alternative table, 0=Regular table
	FAPI_P_UINT8 num_CW; //
	FAPI_P_UINT8 num_of_Tx_Ant;
	FAPI_P_UINT8 NrOfCDMs;
	FAPI_P_T_DMRS_CFG DMRS_parameters;
	FAPI_P_T_SYMB_ALLOC SYMB_parameters;
} FAPI_P_T_ULSCH_PDU_R8;

//Uplink structures
// Slot (TTI) indication message structure
typedef struct PACK_STRUCT FAPI_P_S_SLOT_IND {
	FAPI_P_T_MSG_HDR msg_hdr;                //Message type ID
	SFN_SlotStruct sf_sfn;      //[15:4] SFN, range 0..1023, [3:0] SF, range 0...9
} FAPI_P_T_SLOT_IND;

/******************************************************************************************//*!
 *@class CONFIGREQUESTStruct
 *@brief The purpose of this class is :
 *@brief This struct contains all the variables to build Config.request message.
 *@brief Depicts how PHY interacts with L2/L3 through API messages to change PHY state.
 *@brief Config.request message. change the PHY state from IDLE to CONFIGURED
 *********************************************************************************************/
/*
typedef struct PACK_STRUCT PDU_HEADER {
	uint16_t nPDUType;					// 0: DL DCI PDU
										// 1: DLSCH PDU
										// 2: CSI-RS PDU
										// 3: BCH PDU
	uint16_t nPDUSize;// Size of the PDU control information (in bytes).This length value
					  // includes the 4 bytes required for the PDU type and PDU size parameters.

} PDU_HEADER;


typedef struct PACK_STRUCT L1L2MessageHeader {
	uint16_t nMessageType;
	uint16_t nMessageLen;
} L1L2MessageHeader;
*/

/******************************************************************************************//*!
 *@class DLSCH_PDU
 *@brief The purpose of this class is :
 *@brief Contain all the variables to build a PDSCH channel type
 *********************************************************************************************/
/*
typedef struct PACK_STRUCT DLSCH_PDU {
	PDUStruct header;
	uint32_t nTBSize[2];				// Transmit block size (in bytes)

	uint32_t nRBGIndex[5];				// For resource allocation type 0.
										// RBG index allocated for this DLSCH in bitmap.
										// The most significant bit represents the RBG of lower index.
										// The maximum number is 138 with 275 RBs and RBG size 2.
	uint16_t nRNTI; // The RNTI used for identifying the UE when receiving the PDU, Value: 1 -> 65535.
	uint16_t nUEId;					// UE index in the sector, Value:0 -> 1199
	uint16_t nBWPSize;					// bandwidth part size, Value: 0->275
	uint16_t nBWPStart;			// bandwidth part start RB index, Value: 0->254
	uint16_t nNIDnSCID; 		// Refer to section 7.4.1.1.2 [2]
								// It is given by the higher-layer parameter DL-DMRS-Scrambling-ID if provided and the PDSCH is scheduled by PDCCH with CRC scrambled by C-RNTI or CS-RNTI, otherwise, L2 needs to set it to physical cell id.
	uint16_t nNid;						// Refer to section 7.3.1.1 [2]
										// It equals the higher-layer parameter Data-scrambling-
										// Identity if configured and the RNTI equals the C-RNTI,
										// otherwise L2 needs to set it to physical cell id.
	uint16_t nRBStart;					// For resource allocation type 1.
										// The starting resource block for this ULSCH allocation.
	uint16_t nRBSize;					// For resource allocation type 1.
										// The number of resource block allocated to this DLSCH grant.
										// This should match the value sent in the DL DCI
										// PDU which allocated this grant.
	uint16_t nEpreRatioOfDmrsToSSB;		// DMRS EPRE, Value :1->20000
										// 0.001 dB step, -6dB to 14 dB
	uint8_t nSubcSpacing;				// subcarrierSpacing, value:0->4
	uint8_t nCpType;			// Cyclic prefix type, 0: Normal; 1: Extended
	uint8_t nMcsTable;// MCS-Table-PDSCH, 0: not set to '256QAM', 1: set to '256QAM'
	uint8_t nTransmissionScheme;// 0: non-codebook-based transmission, 1: codebook-based transmission
	uint8_t nNrOfCodeWords;				// Up to 2 codewords, Value: 1 -> 2
	uint8_t nMCS[2];					// MCS index, Value : 0->31
	uint8_t nRV[2];						// Redundancy version, Value : 0->3
	uint8_t nNDI[2];					// New data indication
										// 0: retransmission
										// 1: new data
	uint8_t nNrOfLayers;				// Layer number
										// Value : 1->8
										// single user, up to 8 layers
										// multi-user, up to 4 layers
	uint8_t nNrOfAntennaPorts;			// Number of antenna ports, Value : 1->8
	uint8_t nPortIndex[8];				// Antenna port index
										// 0: port 1000
										// 1: port 1001
										// ….
										// 11: port 1011
										// Value : 0->11

	uint8_t nSCID;						// For DMRS generation, Value : 0->1
	uint8_t nVRBtoPRB;					// VRB-to-PRB-mapping
										// 0: non-interleaved VRB-to-PRB mapping
										// 1: interleaved VRB-to-PRB mapping
	uint8_t nRBBundleSize;// Resource block bundle size L, refer to section 7.3.1.6 in [2]
	uint8_t nStartSymbolIndex;// start symbol index of PDSCH mapping, include both DMRS and data
							  // Value: 0->13
	uint8_t nNrOfSymbols;// PDSCH duration in symbols, include both DMRS and data
						 // Value: 1->14
	uint8_t nResourceAllocType;			//resource allocation type
										// 0: type 0
										// 1: type 1
	uint8_t nNrOfRBGs;					// For resource allocation type 0.
										// Number of RBGs
	uint8_t nRBGSize;					// For resource allocation type 0.
										// RBG size, Value: 2,4,8,16
	uint8_t nHARQID; 					// HARQ Process number, Value : 0->15
	uint8_t nMappingType;				// PDSCH mapping Type
										// 0: mapping type A
										// 1: mapping type B
	uint8_t nDMRSConfigType;			// DL DMRS config type
										// 0: type 1
										// 	1: type2
	uint8_t nNrOfDMRSSymbols;			// DL DMRS symbol number
										// 1: single symbol
										// 2: double symbol
	uint8_t nDMRSAddPos;				// DL additional DMRS position
										// DL-DMRS-add-pos
										// Value: 0->3
	uint8_t nPTRSPresent;				// DL-PTRS-present
										// 0: PT-RS is not present
										// 1: PT-RS is present
	uint8_t nNrOfPTRSPorts;				// Up to 2 ports
										// DL-PTRS-ports.
										// Value: 1->2
	uint8_t nPTRSPortIndex[2];			// Value: 0->11
										// 0: port 1000
										// 1: port 1001
										// ….
										// 11: port 1011
	uint8_t nNrOfDMRSAssPTRS[2];// The number of DM-RS ports associated to PT-RS.it should be relative to dmrs-group.
								// Value: 1->6
	uint8_t nPTRSTimeDensity;			// PT-RS time density
										// Value: 0,1, 2 or 4
										// 0 means PT-RS is not present
	uint8_t nPTRSFreqDensity;			// PT-RS frequency density
										// Value: 0, 2 or 4
										// 0 means PT-RS is not present
	uint8_t nPTRSReOffset;// DL-PTRS-RE-offset, refer to Table 7.4.1.2.2-1 in [2]
						  // Value: 0->3
	uint8_t nNrOfCDMs;// Number of DM-RS CDM groups without data, it determines the ratio of PDSCH EPRE to DM-RS EPRE.  refer to Table 4.1-1 in [5]
					  // Value: 1->3
	uint8_t nCodebookType;				// codebookType in 38.214
										// 0: Type1 Single-Panel
										// 1: Type I Multi-Panel
										// 2: Type II
										// 3: Type II Port Selection
	uint8_t nPMI;						// Codebook index
										// Refer to Table 5.2.2.2.1-1 in 38.214
	uint8_t nEpreRatioOfPDSCHToPTRS;	// PDSCH-to-PT-RS EPRE ratio
										// ratio of PDSCH EPRE to PTRS EPRE, refer to Table 4.1-2 in [4]
										// Value :0->3

} DLSCH_PDU_s;
*/
/******************************************************************************************//*!
 *@class DL_DCI_PDU
 *@brief The purpose of this class is :
 *@brief Contain all the variables to build a PDCCH channel type
 *********************************************************************************************/
/*
struct DL_DCI_PDU {
	PDUStruct header;
	uint16_t nRNTI; 	 // The RNTI associated with the UE, Value: 1 -> 65535
	uint16_t nBWPSize;					// bandwidth part size, Value: 0->275
	uint16_t nBWPStart;			// bandwidth part start RB index, Value: 0->254
	uint8_t nSubcSpacing;				// subcarrierSpacing, value:0->4
	uint8_t nCpType;			// Cyclic prefix type, 0: Normal; 1: Extended
	uint8_t nCoreSetType;// value: 0 or 1, 0: CORESET is configured by the PBCH or SIB1
						 //				  1: otherwise
	uint8_t nBeamId;			// Beam index that UE DL DCI used, Value:0~63
	uint8_t nNrofTxRU;					// TxRU number, Value:1~4
//	uint8_t				nTxRUIdx[MAX_TXRU_NUM];		// TxRU index, refer to spec 36.897, section 5.2.2-1, Value:0~3
	uint8_t nStartSymbolIndex;// Starting OFDM symbol for the CORESET, Value: 0-13
	uint8_t nNrOfSymbols;// Contiguous time duration of the CORESET in number of symbols
	uint32_t nFreqDomain[2];				// frequencyDomainResources
											// The most significant bit of the first word corresponds to the most
											// significant bit defined in 38.331
	uint8_t nCCEToREGType;				// CORESET-CCE-to-REG-mapping-type
										// 0: non-interleaved CCE-to-REG mapping
										// 1: interleaved CCE-to-REG mapping
	uint8_t nREGBundleSize;	// CORESET-REG-bundle-size, Variable R in section 7.3.2.3 [2]
							// Value: 2,3,6
	uint8_t nInterleaveSize;				// CORESET-interleaver-size
	// Value: 2,3,6
	uint16_t nShift;						// Refer to section 7.3.2.2 [2]
											// If it is for a PDCCH transmitted in a CORESET configured by the PBCH or SIB1,
											// L2 need to set it to physical cell ID.
											// Otherwise, L2 needs to set it to CORESET-shift-index.
	uint16_t nScid;						// Refer to section 7.3.2.3 [2]
										// For a UE-specific search space it equals the higher-
										// layer parameter PDCCH-DMRS-Scrambling-ID if configured,
										// otherwise L2 needs to set it to physical cell ID.
	uint16_t nRNTIScramb;				// Refer to section 7.3.2.3 [2]
										// It is given by the C-RNTI for a PDCCH in a UE-specific
										// search space if the higher-layer parameter PDCCH-DMRS-Scrambling-ID
										// is configured, otherwise L2 needs to set it to zero.
	uint8_t nCCEStartIndex;	// CCE start Index used to send the DCI: Value: 0->135
	uint8_t nAggrLvl;				// Aggregation level used: Value: 1,2,4,8,16
	uint16_t nEpreRatioOfPDCCHToSSB;		// PDCCH data EPRE, Value :1->20000
											// 0.001 dB step, -6dB to 14 dB
	uint16_t nEpreRatioOfDmrsToSSB;		// PDCCH DMRS EPRE, Value :1->20000
										// 0.001 dB step, -6dB to 14 dB
	uint16_t nTotalBits;		// The total DCI length including padding bits
//	uint8_t				nDciBits [DCI_PAYLOAD_BTYE_LEN];		// DCI payload, bit order is as following
	// bit0-bit7 are mapped to first byte of MSB - LSB

} ;

*
/******************************************************************************************//*!
 *@class ULSCH_PDU
 *@brief The purpose of this class is :
 *@brief Contain all the variables to build a PUSCH channel type message
 *********************************************************************************************/
/*
struct ULSCH_PDU {
	PDUStruct header;
	SFN_SlotStruct SSC;	//  system frame number 0-1023

	uint8_t nSectorId;	//Sector index of the 5GNB,Value: 0->2
	uint8_t nCarrierIdx;
	uint8_t nSubcSpacing;	//subcarrierSpacing value:0->4
	uint8_t nCpType;	//Cyclic prefix type  0: Normal; 1: Extended
	uint8_t nMcsTable; // MCS-Table-PUSCH 0: not set to '256QAM' 1: set to '256QAM'
	uint8_t nMCS; // MCS index value: 0->31
	uint8_t nTransPrecode; //0: transform precoding enabled,1: transform precoding disabled
	uint8_t nTransmissionScheme; //0: non-codebook-based transmission,1: codebook-based transmission
	uint8_t nNrOfLayers; //number of layers,Value: 1,2,4
	uint8_t nNrOfAntennaPorts; //number of antenna ports,Value: 2,4
	uint8_t nPortIndex[5]; //Up to 4 ports,0: port 1000,1: port 1001 ... 11: port 1011



	uint8_t nNid; //Data-scrambling-IdentityValue : 0->1023
	uint8_t nSCID; // For DMRS generatio Value : 0->1
	uint8_t nVRBtoPRB; // VRB-to-PRB-mapping 0: non-interleaved VRB-to-PRB mapping 1: interleaved VRB-to-PRB mapping
	uint8_t nRBBundleSize; // Resource block bundle size L, refer to section 7.3.1.6 in [2]
	uint8_t nPMI; //Precoding matrix indicator,Value: 0->27
	uint8_t nStartSymbolIndex; //start symbol index of PUSCH mapping  Value: 0, 2 or 3 for PUSCH mapping type A:  [2 or 3] for PUSCH mapping type B:  [0]
	uint8_t nNrOfSymbols; //PUSCH duration in symbols Value: 1->14
	uint8_t nResourceAllocType; //resource allocation type,0: type 0,1: type 1
	uint8_t nNrOfRBGs; //For resource allocation type 1.Number of RBGs
	uint8_t nRBGSize; //For resource allocation type 1.RBG size,Value: 2,4,8,16
	uint8_t nRV; //Redundancy version,Value: 0 -> 3
	uint8_t nHARQID; //HARQ Process number.
	uint8_t nNDI; //Specify whether this received PUSCH is a new transmission from UE. This should match the value sent in the DCI Format 0 PDU which allocated this grant.
	uint8_t nMappingType; //PUSCH mapping Type,0: mapping type A,1: mapping type B
	uint8_t nDMRSConfigType; //UL DMRS config type,0: type 1,1: type2
	uint8_t nNrOfDMRSSymbols; //Number of UL DMRS symbols,1: single symbol,2: double symbol
	uint8_t nDMRSAddPos; //UL additional DMRS position,UL-DMRS-add-pos,Value: 0->3
	uint8_t nPTRSPresent; //UL-PTRS-present,0: PT-RS is not present,1: PT-RS is present
	uint8_t nNrOfPTRSPorts; //UL-PTRS-ports. Up to 2 ports,Value: 1->2
	uint8_t nPTRSPortIndex0; //0: port 1000,1: port 1001 ��. 11: port 1011
	uint8_t nPTRSPortIndex1;
	uint8_t nNrOfDMRSAssPTRS0; //The number of DM-RS ports associated to PT-RS  Value: 1->6
	uint8_t nNrOfDMRSAssPTRS1;
	uint8_t nPTRSTimeDensity; //PT-RS time density,Value: 0,1, 2 or 4,0 means PT-RS is not present
	uint8_t nPTRSFreqDensity; //PT-RS frequency density,Value: 0, 2 or 4,0 means PT-RS is not present
	uint8_t nPTRSReOffset; //UL-PTRS-RE-offset , refer to Table 6.4.1.2.2.1-1 in [2] Value: 0->3


	uint16_t nRNTI;	//The RNTI used for identifying the UE when receiving the PDU,Value: 1 -> 65535.
	uint16_t nUEId;	//UE index in the sector,Value:0 -> 1199
	uint16_t nBWPSize;	//bandwidth part size,Value: 20->275
	uint16_t nBWPStart;	//bandwidth part start RB index,Value: 0->254

	uint16_t nNIDnSCID; // DL-DMRS-Scrambling-ID, For DMRS generation Value : 0->65535

	uint16_t nRBStart; //For resource allocation type 0.The starting resource block for this ULSCH allocation.
	uint16_t nRBSize; //For resource allocation type 0.The number of resource block allocated to this ULSCH grant.This should match the value sent in the DCI Format 0 PDU which allocated this grant.

	uint32_t PDUidx;
	uint32_t nRBGIndex0; //For resource allocation type 1.RBG index allocated for this ULSCH.The maximum number is 138 with 275 RBs and RBG size 4.
	uint32_t nRBGIndex1;
	uint32_t nRBGIndex2;
	uint32_t nRBGIndex3;
	uint32_t nRBGIndex4;
	uint32_t nRBGIndex5;
	uint32_t nTBSize; // transmit block size (in bytes)

} ;

*/
/******************************************************************************************//*!
 *@class UL_DCI_PDU
 *@brief The purpose of this class is :
 *@brief Contain all the variables to build a message with DCI content used for the
 *@brief scheduling of PUSCH
 *********************************************************************************************/

/*
struct UL_DCI_PDU {
	PDUStruct header;
	SFN_SlotStruct SSC; // SFN/Slot/Carrier. A 32-bit value where, [31:22] SFN, range 0-1023.
				  // [21:13] Slot, range 0-319
				  // [12:9] carrier index, range 0-15
	uint16_t nBWPSize;					// BW part size, value 0-275
	uint16_t nBWPStart;					// BW part start RB index, value 0-4
	uint8_t nSubcSpacing;				// subcarrier spacing, value 0-4
	uint8_t nCpType;				// Cyclic prefix type, 0:Normal, 1:Extanded
	uint8_t nDCI;		// The number of UL DCI PDUs included in this message.

} ;
*/

/******************************************************************************************//*!
 *@class ULCCH_UCI_PDU
 *@brief The purpose of this class is :
 *@brief Contain all the variables to build a message which present when a UE has been
 *@brief instructed to send uplink UCI
 *********************************************************************************************/
/*
struct ULCCH_UCI_PDU {
	PDUStruct header;
	uint16_t nRNTI;	// The RNTI used for identifying the UE when receiving the PDUValue:
					// from 1 to65535
	uint16_t nUEId;					// UE index in the sectorValue:from 0 to1199
	uint16_t nBWPSize;				// Bandwidth part sizeValue: from 0 to275
	uint16_t nBWPStart;		// Bandwidth part startRB indexValue: from 0 to 254
	uint8_t nSubcSpacing;				// subcarrierSpacing Value:from 0 to4
	uint8_t nCpType;					// Cyclic prefix type0: Normal; 1:
	uint8_t ExtendednULType; 	// Uplink frequency0: NR UL ; 1: supplementary
	uint8_t ULnFormat; 					// PUCCH formatValue:from 0 to 4
	uint16_t nID; // Data scrambling identity if high layer configured, otherwise is nCellIDValue:from 0 to1023
	uint16_t nScamblingID; // Using for reference signal scramblingValue:from 0 to65535
	uint8_t nSRPriodAriv;				// Whether SR period arrivedValue: 0,1
	uint8_t nStartSymbolx;			// Index of first symbolValue: from 0 to 13
	uint8_t nSymbols;				// Number of symbolsValue:1,2,from 4 to 14
	uint16_t nStartPRB;	// Index of the first PRB prior to frequency hopping or no-hoppingValue:from 0 to 275
	uint16_t n2ndHopPRB;// Index of the first PRB after frequency hoppingValue: from 0 to 275
	uint16_t nPRBs;						// A number of PRBsValue: from 1 to 276
	uint8_t nFreqHopFlag;// Frequency hopping for a PUCCH resource enabled or disabled Value: 0,1
	uint8_t nGroupHopFlag;				// Flag for group hopping Value: 0,1
	uint8_t nSequenceHopFlag;			// Flag for sequence hoppingValue: 0,1
	uint8_t nAddDmrsFlag;				// Flag for additional DMRSValue: 0,1
	uint8_t nCycleShift;		// For format 0 or format 1Value: from 0 to 11
	uint8_t nBeamId;					// Beam indexValue:from 0 to 63
	uint8_t nNrofRxRU;					// Number of RxRUValue:from 1 to 4
//	uint8_t				nRxRUIdx[MAX_RXRU_NUM];		// Rx RU index, refer to spec 36.897, section 5.2.2-1 and API doc section 3.1.3 Value:from 0 to 3
	uint8_t nFmt1OrthCCodeIdx;// An index of an orthogonal cover code in case of PUCCH format 1 Value: from 0 to 6
	uint8_t nFmt4OrthCCodeIdx;// An index of an orthogonal cover code in case of PUCCH format 4 Value: 0,1,2,3
	uint8_t nFmt4OrthCCodeLength;// A length of an orthogonal cover code in case of PUCCH format 4 Value: 2,4
	uint16_t nBitLenUci;					// Bit length of UCI payload
} ;

*/
/*
struct SRS_PDU
{
	uint16_t nRNTI; 	 		// The RNTI used for identifying the UE when receiving the PDU, Value: 1 -> 65535.
	uint16_t nUEId;				// UE index in the sector, Value:0 -> 1199
	uint16_t nBWPSize;			// bandwidth part size, Value: 0->275
	uint16_t nBWPStart;			// bandwidth part start RB index, Value: 0->254
	uint8_t nSubcSpacing;		// subcarrierSpacing, value:0->4
	uint8_t nCpType;			// Cyclic prefix type, 0: Normal; 1: Extended
	uint8_t nNrOfSrsPorts;	 	// Number of SRS ports, Value: 1 ,2 ,4.
	uint8_t nComb;				// Transmission comb, Value: 2 or 4
	uint8_t nCombOffset;		// combOffset, value: 0->3
	uint8_t nCyclicShift;		// cyclicShift, value: 0->11
	uint8_t nStartPos;			// startPosition ,the starting position in the time domain, value:0->5
	uint8_t nNrOfSymbols;		// Number of SRS symbols, Value: 1 ,2 ,4.
	uint8_t nRepetition;		// repetitionFactor, Value: 1 ,2 ,4.
	uint8_t nFreqPos;			// freqDomainPosition, nRRC, value:0->67
	uint16_t nFreqShift;		// freqDomainShift, nShift, value:0->268
	uint8_t nCSrs;				// c-SRS, value:0->63
	uint8_t nBSrs;				// b-SRS, value:0->3
	uint8_t	nBHop;				// b-hop, value:0->3
	uint8_t	nHopping;			// groupOrSequenceHopping, value:0->2
								// 0: neither, 1: groupHopping, 2: sequenceHopping
	uint16_t nSrsId;				// The SRS sequence identity,  sequenceId, value:0->1023
	uint8_t	nResourceType;		// Value:0->2, 0: aperiodic, 1: semi-persistent	, 2: periodic
	uint16_t	nTsrs;			// SRS-Periodicity in slots
								// Value: 1,2,3,4,5,8,10,16,20,32,40,64,80,160,320,640,1280,2560
	uint16_t	nToffset;		// Value:0->2559
	uint8_t	nBeamId;			// Beam index, value:0~63
	uint8_t	nNrofRxRU;			// Number of RxRU, value:1~4
	//uint8_t	nRxRUIdx[MAX_RXRU_NUM]		// RxRU index, refer to spec 36.897, section 5.2.2-1, value:0~3
};
*/

/*
struct DL_CONFIG_request {
	tL1L2MessageHeader header;
	SFN_SlotStruct SSC;						// A 32-bit value where,
	// [28:25] carrier index, range from 0 to 15
	// [24:9] SFN, range from 0 to 1023
	// [8:0] Slot, range from 0 to 319
	uint8_t nDCI;		// Number of DL DCIs that are included in this message
	uint8_t nPDU;			// Number of PDUs that are included in this message.
							// All PDUs in the message are numbered in order.
							// Range from 0 to 255
	uint8_t nGroup;				// Number of UE Groups included in this message.
	char dl_config_data[MAX_HANDLER_DATA_IO_SIZE];
//	std::vector<DL_DCI_PDU> dl_dci_pdu_content_vec;
//	std::vector<DLSCH_PDU> dlsch_pdu_content_vec;
//  std::vector<CSI_RS_PDU>	csi_rs_pdu_content_vec;


} ;



struct FAPI_DLSCH_PDU_index {
	uint16_t nPDUType; 		//0: DL DCI PDU
								//1: DLSCH PDU
								//2: CSI-RS
								//3: BCH PDU
	uint16_t nPDUSize;

	tDlSchPdu DLSCH_PDU_str;

	uint8_t nUe;//Number of UE in this group, For SU-MIMO, one group includes one UE only.
	// For MU-MIMO, one group includes up to 12 UEs.
	uint8_t nPduIdx;// Index for number of PDU identified by nPDU in this message

};
*/

struct FAPI_DL_DCI_PDU_index {
	uint16_t nPDUType; 		//0: DL DCI PDU
								//1: DLSCH PDU
								//2: CSI-RS
								//3: BCH PDU
	uint16_t nPDUSize;

	tDciPduStruct DL_DCI_PDU_str;

	uint8_t nUe;//Number of UE in this group, For SU-MIMO, one group includes one UE only.
	// For MU-MIMO, one group includes up to 12 UEs.
	uint8_t nPduIdx;// Index for number of PDU identified by nPDU in this message

};



/*

struct UL_CONFIG_request {
	tL1L2MessageHeader header;
	SFN_SlotStruct SSC;						// A 32-bit value where,
	// [28:25] carrier index, range from 0 to 15
	// [24:9] SFN, range from 0 to 1023
	// [8:0] Slot, range from 0 to 319
	uint8_t nRachPresent;//0: do not receive RACH in this slot; 1: receive RACH in this slot

	uint8_t nPDU;			// Number of PDUs that are included in this message.
							// All PDUs in the message are numbered in order.
							// Range from 0 to 255
	uint8_t nUlsch;		// Number of ULSCH that are included in this message.
	uint16_t nUlcch;	// Number of ULCCH that are included in this message.
	//uint8_t nulrach;					// Number of ULRACH that are included in this message.
	//uint8_t nsrs;						// Number of SRS_PDU that are included in this message.
	uint8_t nGroup;				// Number of UE Groups included in this message.

//	std::vector<ULSCH_PDU> ulsch_pdu_content_vec;
//	std::vector<ULCCH_UCI_PDU> ulcch_uci_pdu_content_vec;
	//std::vector<ULCCH_UCI_PDU>
	//std::vector<SRS_PDU>

	uint8_t nUe;//Number of UE in this group, For SU-MIMO, one group includes one UE only.
	// For MU-MIMO, one group includes up to 12 UEs.
	uint8_t nPduIdx;// Index for number of PDU identified by nPDU in this message

	char ul_config_data[MAX_HANDLER_DATA_IO_SIZE];

} ;

*/

/******************************************************************************************//*!
 *@class Tx_req
 *@brief The purpose of this class is :
 *@brief The format of the TX.request message is described in Table 4-110.
 *@brief This message contains the MAC PDU data for transmission over the air interface.

 *********************************************************************************************/
#ifdef dlstruct
struct DlPduDataStruct {

	/**** word 1 *****/
	uint8_t nPduIdx; // This value is an index for number of PDU identified by nPDU in DL_CONFIG.request message

	/**** word 2,3 *****/
	FAPI_P_UINT16 nPduLen1;     // length of first transport block in byte
	FAPI_P_UINT16 nPduLen2;     // length of second transport block in byte

	/**** word 4,5 *****/
	uint8_t *pPayload1; // Content of first transport block, the first PRERESERVED_DATA_HEADER_LEN bytes are reserved for SW-FPGA header
	uint8_t *pPayload2; // Content of second transport block in case of 2 TBs,the first PRERESERVED_DATA_HEADER_LEN bytes are reserved for SW-FPGA header
} ;


struct TxRequestStruct {
	/**** word 1 *****/
	tL1L2MessageHeader sMsgHdr;

	/**** word 2 *****/
	SFN_SlotStruct SSFN_Slot;

	/**** word 3 *****/
	uint8_t nPDU; // Number of PDUs that are included in this message.Range 0 -> 514

	char tx_request_data[MAX_HANDLER_DATA_IO_SIZE];
//	std::vector<DlPduDataStruct> sDLPDUDataStruct;
} ;

#endif
#ifdef __cplusplus
}
#endif
struct UL_DCI_request {
	tL1L2MessageHeader header;
	SFN_SlotStruct SSC;		// A 32-bit value where,
	// [31:22] SFN, range from 0 to 1023
	// [21:13] Slot, range from 0 to 319
	// [12:9] carrier index, range from 0 to 15
	uint16_t nBWPSize;	// BW part size, 0-275
	uint16_t nBWPStart;	// BW part start RB index, 0-254
	uint8_t nSubcSpacing; // subcarrierspacing, 0-4
	uint8_t nCpType;	  //Cyclic prefix type, 0:Normal, 1: Extended
	uint8_t nDCI;		// The number of UL DCI PDUd included in this message
	std::vector<tUlDciRequestStruct> ul_dci_pdu_vec;
} ;

/******************************************************************************************//*!
 *@class Slot
 *@brief The purpose of this class is :
 *@brief Determine the structure of each slot
 *@brief They are used to indicate the DL and UL Slot structures.
 *********************************************************************************************/
#define PDU_STORAGE (10000)

struct SLOT {
	//tDlConfigRequestStruct dl_config_request_message;
	uint32_t storage_pdu_dl[PDU_STORAGE];
	//tUlConfigRequestStruct ul_config_request_message;
	uint32_t storage_pdu_ul[PDU_STORAGE];
	//tTxRequestStruct tx_request_struct_message;
	uint32_t storage_tx_req[PDU_STORAGE];

	//tUlDciRequestStruct ul_dci_request_message;
	uint32_t storage_dci_req[PDU_STORAGE];
	uint8_t data_out_slot[MAX_HANDLER_DATA_IO_SIZE/8];

};



struct SLOT_ptr {
	tDlConfigRequestStruct *dl_config_request_message_p;
	tUlConfigRequestStruct *ul_config_request_message_p;
	tTxRequestStruct *tx_request_struct_message_p;
	tUlDciRequestStruct *ul_dci_request_message_p;
//	uint8_t *data_out_[MAX_HANDLER_DATA_IO_SIZE/8];
};


/******************************************************************************************//*!
 *@class SlotGroup
 *@brief The purpose of this class is :
 *@brief Consolidate all the slots in a session
 *@brief Defines the number of repetitions
 *********************************************************************************************/

struct SlotGroup {
	int repeat;
	std::vector<SLOT> SLOT_GROUP_vec;
	int num_of_slots;
    void Reset()
    {
        repeat= 0;
        num_of_slots = 0;
        SLOT_GROUP_vec.clear();
    }
    SlotGroup()
    {
        Reset();
    }
};

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */


#endif //HANDLER_FAPI_P_H
/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
