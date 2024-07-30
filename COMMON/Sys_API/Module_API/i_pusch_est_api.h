/******************************************************************
 * i_channel_est_api.h
 * Author: D. Matas
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include "i_api_common.h"

#ifndef API_MODULE_API_I_PUSCH_EST_API_H_
#define API_MODULE_API_I_PUSCH_EST_API_H_

#define ESTIMATION_MODULE  "PUSCH_ESTIMATION"

#define ESTIMATION_FIFO_IN   "ESTIMATION_FIFO_IN_JOB_REQ"

#define MAX_ESTIMATION_DATA_IO_SIZE   (50000)
#define MAX_NUM_OF_TX_ANT             (16)


struct EstimationHandlerIO
{
    uint32_t  max_number_of_rx_antennas;
    uint32_t  number_of_tx_antennas;
    uint32_t  total_number_of_sequences; //per sequence
    uint32_t  RB_size; //per sequence
    uint32_t  stride; //jump between successive DMRS signals
    uint32_t  k_offset[2]; //due only 2 k_offet comes from matlab. offset of dmrs RE from RB start (0-3/5, for example for dmrs 0/4/8 - offset=0.for 2/6/10 - offset is 2)
    uint32_t  number_of_active_symbols_in_slot;
    uint32_t  num_of_layers;
    uint32_t  Qm;
    uint32_t  sequence_symbol_index[MAX_NUM_OF_DMRS_SEQ];
    uint8_t   NrOfCDMs;

};

struct EstimationParameters
{
    uint8_t   max_number_of_rx_antennas;
    uint8_t   number_of_tx_antennas;
    uint16_t  RB_size; //per sequence
    uint8_t   stride; //jump between successive DMRS signals
    uint16_t  length_of_sequence;
    uint32_t  number_of_active_symbols_in_slot;
    uint8_t   num_of_layers;
    uint8_t	  start_symbol;
    uint8_t   num_of_symbols; //num of symbols containing DMRS
    uint8_t   num_of_different_seq;  //depend on layers number (1-2 layers - 2 seq, 3-4 layers - 4 seq )
    uint8_t total_number_of_sequences; //All total seq = num_of_symbols*num_of_different_seq
    uint8_t  k_offset[MAX_DMRS_SEQUENCES_NUM_PER_SYMBOL];
     uint8_t  sequence_symbol_index[MAX_NUM_OF_DMRS_SEQ];
     uint8_t NrOfCDMs;
     FAPI_P_T_DMRS_CFG  DMRS_parameters;
      //uint32_t  Phazor_length;  //per sequence
};

struct PUSCHEstimationIO
{
    Complex **dmrs;
    Complex *reference_rs;
    Complex *timing_phasor_ptr;
    Complex *freq_phasor_ptr;
    Complex *channel_matrix_ptr;
    Complex *noise_matrix_ptr;
    EstimationParameters est_parameters;
};

struct pusch_est_API{
    virtual void  Run_all_estimations_association(Complex *dmrs, Complex *reference_rs, EstimationHandlerIO est_parameters,Complex *timing_phasor_ptr,Complex *freq_phasor_ptr,Complex *channel_matrix,Complex *noise_matrix)=0;
};


#endif /* API_MODULE_API_I_DMRS_API_H_ */
