/******************************************************************
 * i_pusch_equalizer_api.h
 * Author: E. Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include "i_api_common.h"
#include "i_splitter_engine_api.h"
#include "i_handler_api.h"

#ifndef API_MODULE_API_I_PUSCH_EQUALIZER_API_H_
#define API_MODULE_API_I_PUSCH_EQUALIZER_API_H_

#define EQUALIZER_ENGINE_MODULE  "PUSCH_EQUALIZER"

// Name of input FIFO for Equalizer Engine
#define EQUALIZER_ENGINE_JOBS	   "EQUALIZER_ENGINE_FIFO_IN_JOB_DESCS"
#define EQUALIZER_DATA_FIFO_IN     "EQUALIZER_FIFO_IN_DATA"


struct EqualizerEngineJobDescriptor
{
	uint32_t	number_of_rx_antennas;											// up to 16 antennas
	uint32_t	number_of_layers;												// up to 4 layers
    uint32_t    tb_number;                                                      // tb index on slot, in sorted order. Purpose:Detect slot header only in tb_number=0
    uint32_t    first_tb_symbol;                                                // first OFDM symbol allocated on this TB
	uint32_t	tb_width;														// number of symbols allocated on this TB from the entire 14 subframe's symbols
	Complex*	time_estimation_ptr;											// STO of RB
	uint32_t	total_rb_number_on_tb;											// required for STO/CFO multiplexing
	uint32_t	rb_offset_on_tb;												// for debug finding Slot Headers. Might be changed to 1 bit flag, e.g. "first_rb_on_tb_flag" (1 - for first RB on TB, 0 - otherwise)
	uint32_t 	Qm; 															// Derived from MCS given by FAPI
	Complex*	freq_estimation_ptr;											// CFO of RB
	Complex*	s_matrix_ptr;													// RB's S-Matrix
	uint16_t*   SNR_ptr;
	CMemArea**	first_tb_symbol_fifo;											// input data to Equalizer (current TB's first Symbol FIFO) including DMRS REs (from array of 14 Symbols FIFOs). Each message inside contains data and header as well.
	Complex**	first_equalized_symbol_fifo;									// pointer to array of pointers to output data from Equalizer divided by Symbols buffers (14 FIFOs)
	int8_t**	first_llr_symbol_fifo;											// pointer to array of pointers to output data from LLR divided by Symbols buffers (14 FIFOs)

	// Each output TB will be located at predefined constant offsets according to max number of supported Layers (4).
	// It shouldn't be that way on a real FIFO implementation (on FIFO data has to be conjugated !!!)
	// In case of real FIFO implementation the data can be possible retrieved by getting a single FIFO buffer
	// that has to be defined by maximum possible size of up to 275 RBs. In this implementation the following offset-value will be redundant !!!
	uint32_t	output_rb_ptr_offset_equalizer[MAX_NUM_OFDM_PER_SLOT]; // output REs location offset on Symbol
	uint32_t	output_rb_ptr_offset[MAX_NUM_OFDM_PER_SLOT]; // output REs location offset on Symbol

	uint32_t    is_first_rb_on_symbol_flag[MAX_NUM_OFDM_PER_SLOT]; // flag that defines if this RB is a first RB on each particular Symbol (for debugging detection of Symbol Headers on each Symbol). Can be 1 bit per each RB Symbol, i.e. can catch only 1 Word (14 bits)
};

struct EqualizingHandlerIO
{
	/*
	uint32_t  max_number_of_rx_antennas;
    uint32_t  number_of_tx_antennas;
    uint32_t  number_of_sequences; //per sequence
    uint32_t  RB_size; //per sequence
    uint32_t  stride; //jump between successive DMRS signals
    //uint32_t  Phazor_length;  //per sequence
     *
     */
	uint32_t	max_number_of_antennas;	// up to 16 antennas
	uint32_t	max_number_of_layers;	// up to 4 layers
	uint32_t	tb_width;				// number of symbols allocated on this TB from the entire 14 subframe's symbols
	uint32_t	num_rbs_in_tb;			// RB(s) amount inside of TB
	uint32_t	rb_index;				// RB index inside of TB (can be [0 - (num_rbs_in_tb-1)])
	Complex*	output_symbols;			// output symbols location
};



struct EqualizerOutputReData
{
	SplitterOutputReDataHeader		equalizer_data_header;
	Complex							equalized_re_data[MAX_NUM_OF_LAYERS];
};


struct pusch_equalizing_API{

	virtual void pusch_tb_equalizing(	Complex* 				input_symbols,
										Complex*				time_phasor_vec_ptr,
										Complex*				freq_phasor_vec_ptr,
										Complex*				s_matrix_vec_ptr,
										EqualizingHandlerIO& 	equal_parameters)				= 0;

	virtual void pusch_rb_equalizing(	Complex* 				input_rb_symbols,
    									Complex* 				s_matrix,
 										Complex* 				time_phasor_estimation,
 										Complex* 				freq_phasor_estimation,
										EqualizingHandlerIO& 	equal_parameters)		= 0;

    virtual void data_complex_interleaving(	Complex**	dataPtrsArray,
											uint32_t	dataSize,
											uint32_t	numOfElementsToInterleave,
											Complex*	outDataPtr
											) = 0;

};


#endif /* API_MODULE_API_I_PUSCH_EQUALIZER_API_H_ */
