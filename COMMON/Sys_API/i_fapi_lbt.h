/*-----------------------------------------------------------
 * LBT.c
 *
 * FAPI LBT Definitions
 *
 * Copyright(c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * OWNER:                     
 *
 *-----------------------------------------------------------
 */
#ifndef _I_FAPI_LBT_H_
#define _I_FAPI_LBT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
#define FAPI_LBT_TXOP_SYNC_MAX_PLMN_IDS         (8)


/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */
// LBT Config Request TLV types
typedef enum
{
	FAPI_E_CFG_LBT_ED_THRESHOLD					= 1,
	FAPI_E_CFG_LBT_PD_THRESHOLD					= 2,
	FAPI_E_CFG_LBT_DATA_TX_CHANNELS_MODE 		= 3,
    FAPI_E_CFG_LBT_AUXILIARY_TX_CHANNELS_MODE 	= 4,
    FAPI_E_CFG_LBT_DATA_RX_CHANNELS_MODE		= 5,
	FAPI_E_CFG_LBT_AUXILIARY_RX_CHANNELS_MODE	= 6,
	FAPI_E_CFG_LBT_FILLER_SIGNAL_ENABLE 		= 7,
	FAPI_E_CFG_LBT_WCUBS_ENABLE					= 8,
	FAPI_E_CFG_LBT_DRS_WCUBS_ENABLE				= 9,
	FAPI_E_CFG_LBT_MAX_PACKET_DECODE_LENGTH		= 10,
    FAPI_E_CFG_LBT_WCUBS_GRANULARITY			= 11,
	FAPI_E_CFG_LBT_MIN_TXOP						= 12,
	FAPI_E_CFG_LBT_TXOP_SYNC 					= 13,
	FAPI_E_CFG_LBT_TXOP_SYNC_PLMN_IDS 			= 14,
	FAPI_E_CFG_LBT_RFFE_MODE					= 15,
	FAPI_E_CFG_LBT_STATE_MACHINE_MODE			= 16,
	FAPI_E_CFG_LBT_MAX
}FAPI_E_LBT_CONFIG_TLV_TYPE;

// LBT Tx channel mode
typedef enum
{
	FAPI_E_LBT_TX_CH_MODE_DISABLED           = 0,
    FAPI_E_LBT_TX_CH_MODE_WCUBS_ONLY         = 1,
    FAPI_E_LBT_TX_CH_MODE_MAX
}FAPI_E_LBT_TX_CH_MODE_TYPE;

// LBT Filler Signal enable
typedef enum
{
	FAPI_E_LBT_FILLER_SIGNAL_DISABLED        = 0,
    FAPI_E_LBT_FILLER_SIGNAL_ENABLED         = 1,
    FAPI_E_LBT_FILLER_SIGNAL_MAX
}FAPI_E_LBT_FILLER_SIGNAL_TYPE;

// LBT W-CUBS enable
typedef enum
{
	FAPI_E_LBT_WCUBS_DISABLED        = 0,
    FAPI_E_LBT_WCUBS_ENABLED         = 1,
    FAPI_E_LBT_WCUBS_MAX
}FAPI_E_LBT_WCUBS_TYPE;

// LBT DRS W-CUBS enable
typedef enum
{
	FAPI_E_LBT_DRS_WCUBS_DISABLED        = 0,
    FAPI_E_LBT_DRS_WCUBS_ENABLED         = 1,
    FAPI_E_LBT_DRS_WCUBS_MAX
}FAPI_E_LBT_DRS_WCUBS_TYPE;

// LBT TXOP Sync
typedef enum
{
	FAPI_E_LBT_TXOP_SYNC_DISABLED        = 0,
    FAPI_E_LBT_TXOP_SYNC_ENABLED         = 1,
    FAPI_E_LBT_TXOP_SYNC_MAX
}FAPI_E_LBT_TXOP_SYNC_TYPE;

// LBT Rx channel mode
typedef enum
{
	FAPI_E_LBT_RX_CH_MODE_DISABLED           = 0,
    FAPI_E_LBT_RX_CH_MODE_ED_ONLY            = 1,
    FAPI_E_LBT_RX_CH_MODE_ED_PD              = 2,
    FAPI_E_LBT_RX_CH_MODE_MAX
}FAPI_E_LBT_RX_CH_MODE_TYPE;

// LBT state machine mode
typedef enum
{
	FAPI_E_LBT_RFFE_MODE_SGC		    = 0,
    FAPI_E_LBT_RFFE_MODE_MRC		    = 1,
    FAPI_E_LBT_RFFE_MODE_MAX
}FAPI_E_LBT_RFFE_MODE_TYPE;

// LBT state machine mode
typedef enum
{
    // Standard modes
	FAPI_E_LBT_SM_MODE_DISABLED		    = 0,        // To be defined
	FAPI_E_LBT_SM_MODE_LTEU_P15	        = 1,        // LTE-U P1.5 state machine (operational mode)

    // Debug modes
	FAPI_E_LBT_SM_MODE_CONTINUOUS		= 1000,	    // Continous transmission mode: The carrier behaves as licensed carrier.
	FAPI_E_LBT_SM_MODE_SIM	            = 2000,
	FAPI_E_LBT_SM_MODE_SIM_TX_OFF		= 2001
}FAPI_E_LBT_SM_MODE_TYPE;


// LBT DL CONFIG PDU types
typedef enum
{
	FAPI_E_LBT_NORMAL_REQ_PDU	= 0,
	FAPI_E_LBT_DRS_REQ_PDU		= 1,
	FAPI_E_LBT_DL_CONFIG_PDU_MAX
}FAPI_E_LBT_DL_CONFIG_PDU_TYPE;

// LBT DL IND PDU types
typedef enum
{
	FAPI_E_LBT_NORMAL_RSP_PDU	= 0,
	FAPI_E_LBT_DRS_RSP_PDU		= 1,
	FAPI_E_LBT_DL_IND_PDU_MAX
}FAPI_E_LBT_DL_IND_PDU_TYPE;

//LBT DRS Force TX types
typedef enum
{
	FAPI_E_LBT_DRS_DO_NOT_FORCE_TX	= 0,
	FAPI_E_LBT_DRS_FORCE_TX			= 1,
	FAPI_E_LBT_DRS_MAX_FORCE_TX
}FAPI_E_LBT_DRS_FORCE_TX_TYPE;

// LBT NORMAL RSP result types
typedef enum
{
	FAPI_E_LBT_NORMAL_RESULT_SUCCESS	    = 0,    // Indicates that DRS is sent
	FAPI_E_LBT_NORMAL_RESULT_FAILURE	    = 1,    // Indicates that DRS is not sent
	FAPI_E_LBT_NORMAL_RESULT_MAX
} FAPI_E_LBT_NORMAL_RESULT_TYPE;

// LBT NORMAL RSP reason types
typedef enum
{
	FAPI_E_LBT_NORMAL_REASON_WON	        = 0,    // Valid for success. Indicates the LBT process successfully won the channel.
    FAPI_E_LBT_NORMAL_REASON_WON_SYNC       = 1,    // Valid for success. Indicates the LBT process successfully won the channel and synchronized to friendlt eNB.
	FAPI_E_LBT_NORMAL_REASON_NOT_WON	    = 2,    // Valid for failure. Indicates that DRS LBT process didn't win the channel.
    FAPI_E_LBT_NORMAL_REASON_ABANDONED	    = 3,    // Valid for failure. Indicates the LBT process has ended due to end SF/SFN and Min TXOP.
    FAPI_E_LBT_NORMAL_REASON_DRS_LBT	    = 4,    // Valid for failure. Indicates failure due to DRS LBT.
    FAPI_E_LBT_NORMAL_REASON_DRS_SF	        = 5,    // Valid for failure. Indicates failure due to DRS subframe.
	FAPI_E_LBT_NORMAL_REASON_MAX
} FAPI_E_LBT_NORMAL_REASON_TYPE;

// LBT DRS RSP result types
typedef enum
{
	FAPI_E_LBT_DRS_RESULT_SUCCESS	    = 0,    // Indicates that DRS is sent
	FAPI_E_LBT_DRS_RESULT_FAILURE	    = 1,    // Indicates that DRS is not sent
	FAPI_E_LBT_DRS_RESULT_MAX
} FAPI_E_LBT_DRS_RESULT_TYPE;

// LBT DRS RSP reason types
typedef enum
{
	FAPI_E_LBT_DRS_REASON_WON	        = 0,    // Valid for success. Indicates that DRS LBT process successfully won the channel.
	FAPI_E_LBT_DRS_REASON_NOT_WON	    = 1,    // Valid for success/failure. Indicates that DRS LBT process didn't win the channel.
    FAPI_E_LBT_DRS_REASON_ALREADY_WON	= 2,    // Valid for success. Indicates that DRS subframe is within TXOP.
	FAPI_E_LBT_DRS_REASON_MAX
} FAPI_E_LBT_DRS_REASON_TYPE;

typedef struct PACK_STRUCT FAPI_S_LBT_CONFIG_TLV {
	uint16_t					tag;				//tag
	uint16_t					length;				//Length (in bytes)
	uint32_t					value;				//Value of tag parameter
}FAPI_T_LBT_CONFIG_TLV;

typedef struct PACK_STRUCT FAPI_S_LBT_CONFIG_REQ {
	FAPI_T_MSG_HDR			msg_hdr; 			   	//Message header
	uint8_t					n_tlv;					//Number of TLVs contained in the message body.
	FAPI_T_LBT_CONFIG_TLV	tlv[];
} FAPI_T_LBT_CONFIG_REQ;

typedef struct PACK_STRUCT FAPI_S_LBT_CONFIG_RSP {
	FAPI_T_MSG_HDR			msg_hdr; 			   	//Message header
	uint8_t					error_code;				//see table 86
	uint8_t					n_tlv_invalid;			//Number of invalid or unsupported TLVs contained in the message body.
	uint8_t					n_tlv_missing;			//Number of missing TLVs contained in the message body. If the PHY is in the CONFIGURED state this will always be 0.
	FAPI_T_LBT_CONFIG_TLV	tlv[];					//A list of invalid or unsupported TLVs – each TLV is presented in its entirety.
													// after the invalid TLVs there is a list of the missing TLV's
}FAPI_T_LBT_CONFIG_RSP;

typedef struct PACK_STRUCT FAPI_S_LBT_NORMAL_REQ_PDU {
	uint32_t                	handle;      		// An opaque handling returned in LBT_NORMAL_RSP PDU in LBT_DL.indication
	uint32_t                	n_cca;      		// Indicates the value of the initial defer factor (n). 
	uint32_t                	N_cca ;				// Indicates the initial value of the backoff counter (N). 
	uint32_t                	m_cca;      		// Indicates the value of WeCCA defer factor (m) in slots. 
	uint32_t                	M_cca ;				// Indicates the value of agg. CSMA backoff counter (N) in slots. 
	uint32_t               	offset;             // Indicates the LBT start time in microseconds from the beginning of the subframe scheduled by this message. Values: 0-999
	uint32_t					lte_txop;			// Indicates the TXOP duration in subframes. Note: L1 may adjust the TXOP according “SF/SFN End” (See below) and detection of friendly eNB. L1 sends the actual TXOP in LBT indication message.
	uint16_t					txop_sf_sfn_end;  	// Indicates the SFN/SF by which the TXOP must end. A 16-bit value where, [15:4] SFN, range 0 ? 1023, [3:0] SF, range 0 ? 9
	uint16_t					extend_sf_sfn_1;  	// if TXOP is ending right before extend SF/SFN 1 or 2, L1 will extend the TXOP by one subframe. MinTXOP threshold is evaluated after the TXOP is extended, [15:4] SFN, range 0 ? 1023, [3:0] SF, range 0 ? 9
	uint16_t					extend_sf_sfn_2;  	// if TXOP is ending right before extend SF/SFN 1 or 2, L1 will extend the TXOP by one subframe. MinTXOP threshold is evaluated after the TXOP is extended, [15:4] SFN, range 0 ? 1023, [3:0] SF, range 0 ? 9
} FAPI_T_LBT_NORMAL_REQ_PDU;

typedef struct PACK_STRUCT FAPI_S_LBT_DRS_REQ_PDU {
	uint32_t                	handle;      		// An opaque handling returned in LBT_DRS_RSP  PDU in LBT_DL.indication
	uint32_t                	n_cca;      		// Indicates the value of the initial defer factor (n). 
	uint32_t                	N_cca ;				// Indicates the initial value of the backoff counter (N). 
	uint32_t               	offset;             // Indicates the LBT start time in microseconds from the beginning of the subframe scheduled by this message. Values: 0-999
	uint32_t					force_tx;			// Indicates whether DRS shall be transmitted regardless of CCA result. Values: 0 = Do not force Tx (Reserved for LAA), 1 = Force Tx
} FAPI_T_LBT_DRS_REQ_PDU;

typedef struct PACK_STRUCT FAPI_S_LBT_DL_CONFIG_REQ {
	FAPI_T_MSG_HDR		   	msg_hdr; 	   		//Message header
	uint16_t					sf_sfn;				//[15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	uint16_t					length;				//The length of the lbt downlink subframe configuration. Range 0...65535
	uint16_t					n_pdu;				//Number of PDUs that are included in this message. Range 0...514
	FAPI_T_PDU				lbt_dl_pdu[];
} FAPI_T_LBT_DL_CONFIG_REQ;

typedef struct PACK_STRUCT FAPI_S_LBT_NORMAL_RSP_PDU {
	uint32_t                	handle;      					// An opaque handling received in LBT_NORMAL_REQ PDU
	uint32_t                	result;      		        	// Indicates the LBT procedure result of SF/SFN. 0=SUCCESS, 1=FAILURE
	uint32_t                  reason;				        	// Indicates the success/failure reason.
    uint32_t                  lte_txop;   		        	// Actual LTE TXOP in subframes. Valid when LBT result = SUCCESS.
    uint32_t                  cubs_and_filler_duration;   	// Indicates the duration in microseconds the channel was occupied before first downlink subframe (equals to W-CUBS packet duration + filler signal duration). Valid when LBT result = SUCCESS.
    uint32_t                  remaining_backoff_counter;		// Backoff counter (N) value at the time the message was generated. Valid when LBT result  = FAILURE.
    uint32_t                  remaining_csma_backoff_counter;	// CSMA Backoff counter (M) value at the time the message was generated. Valid when LBT result  = FAILURE.
    uint32_t                  nav;   		                	// NAV value in microseconds at the time the message was generated. Valid when LBT result = FAILURE.
} FAPI_T_LBT_NORMAL_RSP_PDU;

typedef struct PACK_STRUCT FAPI_S_LBT_DRS_RSP_PDU {
	uint32_t                	handle;      		// An opaque handling received in LBT_DRS_REQ PDU
	uint32_t                	result;      		// Indicates the LBT procedure result of SF/SFN. 0=SUCCESS (DRS is sent), 1=FAILURE (DRS is not sent)
	uint32_t                  reason; 			// Indicates the success/failure reason.
} FAPI_T_LBT_DRS_RSP_PDU;

typedef struct PACK_STRUCT FAPI_S_LBT_DL_IND{
	FAPI_T_MSG_HDR		   	msg_hdr; 	   		//Message header
	uint16_t					sf_sfn;				//[15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	uint16_t					length;				//The length of the lbt downlink subframe configuration. Range 0...65535
	uint16_t					n_pdu;				//Number of PDUs that are included in this message. Range 0...514
	FAPI_T_PDU				lbt_dl_pdu[];
} FAPI_T_LBT_DL_IND;

typedef struct PACK_STRUCT FAPI_S_LBT_MEAS_IND{
	FAPI_T_MSG_HDR		   	msg_hdr; 	   				//Message header
	uint16_t					sf_sfn;						//ndicates the SF/SFN in which the measurements were performed. [15:4] SFN, range 0..1023, [3:0] SF, range 0...9
	uint32_t					meas_duration;				//Duration in microseconds of the reported measurement period
	uint32_t					ed;							//Number of slots in current measurement period where ED > ED threshold. Generated when LBT Indication is sent during LBT periods and at the end of the subframe during Idle periods
	uint32_t					pure_ed; 					//Number of slots in current measurement period where ED > ED_threshold and there is no overlapping with packet detections (including NAV). Generated when LBT Indication is sent during LBT periods and at the end of the subframe during Idle periods
	uint32_t					non_friendly_ed; 			//Number of slots in current measurement period where ED > ED threshold that overlap non friendly eNodeB detections (Including NAV). 	Generated when LBT Indication is sent during LBT periods and at the end of the subframe during Idle periods
	uint32_t					pd; 						//Sum duration in microseconds of all packet detections during the measurement period. Generated when LBT Indication is sent during LBT periods and at the end of the subframe during Idle periods
	uint32_t					non_friendly_pd; 			//Sum duration in microseconds of all non-friendly eNodeB packet detections during the measurement period. Generated when LBT Indication is sent during LBT periods and at the end of the subframe during Idle periods
	uint32_t					lte_txop; 					//LTE TXOP in subframes. Generated when the medium is won and an LBT success indication is sent to L2
	uint32_t					reuse1; 					//Sum duration in microseconds where TX overlapped with friendly eNodeB transmission. Generated when the medium is won and an LBT success indication is sent to L2
	uint32_t					cubs_and_filler_duration;	//Sum duration in microseconds of W-CUBS and filler transmission during the measurement period. Generated when the medium is won and an LBT success indication is sent to L2
	uint32_t					rssi;
} FAPI_T_LBT_MEAS_IND;


// Error indications

//FAPI_E_MSG_LBT_TXOP_LESS_THAN_MIN_TXOP Error Code.
typedef struct PACK_STRUCT FAPI_S_LBT_TXOP_LIMIT {
	uint8_t		sub_error_code;					//The Sub Error Code for this message.
	uint32_t		received_val;					//Number of PDUs received.
	uint32_t		min_txop;						//PHYs maximum number of accepted PDUs for this channel.
} FAPI_T_LBT_TXOP_LIMIT;

//FAPI_E_MSG_LBT_TXOP_LESS_THAN_MIN_TXOP Error Code.
typedef struct PACK_STRUCT FAPI_S_LBT_END_SF_SFN_GAP_ERROR {
	uint8_t		sub_error_code;					//The Sub Error Code for this message.
	uint16_t		end_sf_sfn;						//End sf_sfn as got from the NORMAL PDU.
	uint16_t		extended_sf_sfn;				//Extended sf_sfn as defined by the NORMAL PDU
} FAPI_T_LBT_END_SF_SFN_GAP_ERROR;

/*
 * -----------------------------------------------------------
 * Static inline functions section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif //_I_FAPI_LBT_H_

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */

