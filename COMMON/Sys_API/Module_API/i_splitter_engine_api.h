/*
 * i_splitter_engine_api.h
 *
 *  Created on: October 8, 2018
 *      Author: emaidman
 */

#ifndef API_MODULE_API_I_SPLITTER_ENGINE_API_H_
#define API_MODULE_API_I_SPLITTER_ENGINE_API_H_

#include "i_api_common.h"

#define SPLITTER_ENGINE_MODULE  "SPLITTER_ENGINE"

/******************************************************************************************//*!
 *@class CExtractedDmrsFifo
 *@brief The purpose of this class is :
 *@brief definition of the Extracted DMRS FIFOs for all Extracted DMRS data Sequences
 *@brief and all RX Antennas
 *********************************************************************************************/
#define MAX_DMRS_FIFO_PER_TB		(MAX_NUM_OF_RX_ANTENNAS*MAX_DMRS_SEQUENCES_NUM_PER_SYMBOL) // 64 FIFOs per TB
#define	TOTAL_DMRS_FIFO_NUMBER		(MAX_NUM_TBS_PER_SLOT*MAX_DMRS_FIFO_PER_TB) // 1024 FIFOS FOR ALL THE EXTRACTED DMRS DATA (PER TB PER RX ANTENNA AND PER SEQUENCE)



struct SplitterConfigureMsg
{
	uint8_t number_rx_antennas;
};

enum ESplitterJobType
{
	E_SYMBOL_INFO,
	E_TB_INFO,
	E_SPLITTER_JOB_TYPE_LAST
};

// Splitter Engine Job Descriptor format
/*
 *
 	 -	Job Type	- 1 bit
 	 -	tb_number   - 4 bit
 	 -	tb_first_fifo_ptr - 64 bit
 	 -	first_rb	- 9 bit
 	 -	rbs_length	- 9 bit
 	 -	rb_dmrs_bitmap- 12 bit
 	 -	number_sequences - 2 bit
 	 -	frame index – 16 LSbits
	 -	slot index in frame - 9 bits
	 -	mini slot index - 2 bits
 *
 */
struct SplitterEngineJobDesc
{
	ESplitterJobType	job_type;
	uint8_t 			tb_number; //TB number as it's written on the TB's object (the order it was received from the FAPI massage). According to this number we will send DMRS to the proper FIFO
	CMemArea**			tb_first_fifo_ptr; // pointer to the first FIFO of the TB DMRS data (from the total 32 FIFOs - 16 for Rx Antennas * 2 for Sequences)
	CMemArea*			symbol_out_fifo_ptr; // output Symbol data storage for Equalizer
	uint16_t			first_rb;
	uint16_t			rbs_length;
	uint16_t			rb_dmrs_bitmap;	// single RB map of marking where DMRS REs are mapped (only 12 bits - number of REs in RB - are relevant)
	uint8_t				number_sequences;
	uint8_t				k_offset_order_arr[MAX_DMRS_SEQUENCES_NUM_PER_SYMBOL]; // ordered array of DMRS sequences K_Offsets array INDICES as they start to appear on RB (e.g. for TX Ports #4,5,6,7 Conf.Type 2 we will have array [2,0,3,1] since K_Offsets of the DMRS Sequences in this case are [1,3,0,2])
	uint16_t			frame_index;
	uint16_t			slot_index_in_frame;
	uint8_t				minislot_index;
};


/*
// definition of the Symbols FIFOs for the output data from the Splitter Engine toward the Equalizer Engine
struct CSymbolsToEqualizerFifo
{
	CMemArea	m_symbols_allocated_data_[MAX_NUM_OFDM_PER_SLOT];
	CMemArea*	allocated_symbols_data_fifo_ptrs_[MAX_NUM_OFDM_PER_SLOT];
	CSymbolsToEqualizerFifo(){for(uint8_t fifo_count=0; fifo_count<MAX_NUM_OFDM_PER_SLOT; fifo_count++) allocated_symbols_data_fifo_ptrs_[fifo_count] = &m_symbols_allocated_data_[fifo_count];};
};
*/
enum EReType
{
	E_NOT_ALLOCATED,
	E_DATA,
	E_DMRS,
	E_PUCCH,
	E_SRS,
	E_RE_TYPE_LAST
};
/*
 * Each RE word include:
					-	32 bits complex word for each antenna – total max size = 32bits *  16Ant
					-	2 (or 4) bits for RE type:  DMRS / Data / Not used ( PUCCH / SRS / … are optional)
					-	2 bits for RB index LSBits
 *
 */

struct SplitterOutputReDataHeader
{
	EReType		re_type;
	uint16_t	rb_index; // for debug purposes. might be only 2-3 LSBits of RB number in order to detect new RB REs data start
};

struct SplitterOutputReData
{
	SplitterOutputReDataHeader		data_header;
	Complex							single_re_data[MAX_NUM_OF_RX_ANTENNAS];
};

/*
 * Slot Header – will be written before a new Slot REs in each FIFO:
					-	Bits 0 – 31:      FEEDABBA
					-	Bits 32 – 47:    frame index – 16 LSbits
					-	Bits 48 – 63:    slot index in frame 9 bits
					-	Bits 64 – 67:    mini slot index 2 bits
 *
 */
struct SplitterOutputSlotHeader // for debug purposes
{
	uint32_t	delimiter_pattern;
	uint32_t	frame_index;
	uint32_t	slot_index;
	uint32_t	minislot_index;
};

union SplitterOutputData
{
	SplitterOutputReData 			re_data_msg;
	SplitterOutputSlotHeader		slot_header_msg;
};

enum ESplitterMsgType
{
	E_RE_PAYLOAD, // might be DMRS or Data, etc.
	E_SYMBOL_HEADER
};

struct SplitterOutputMsg
{
	ESplitterMsgType			msg_type;
	SplitterOutputData		 	equalizer_fifo_data;
};

// Name of input FIFO for Splitter Engine
#define SPLITTER_ENGINE_FIFO_IN   "SPLITTER_ENGINE_FIFO_IN_JOB_DESCS"
#define SPLITTER_DATA_FIFO_IN     "SPLITTER_FIFO_IN_DATA"


#endif /* API_MODULE_API_I_SPLITTER_ENGINE_API_H_ */
