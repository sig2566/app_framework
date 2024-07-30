/******************************************************************
 * i_mimo_receiver_api.h
 * Author: Niv arad
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/



#ifndef API_MODULE_API_I_MIMO_RECEIVER_API_H_
#define API_MODULE_API_I_MIMO_RECEIVER_API_H_

#define MAX_NOISE_RESOLUTION 6 //2*NUMBER_SC_IN_RB/dmrs_stride_ (min stride is 4)

// Name of input FIFO for mimo_reciever
#define MIMO_RECIEVER_FIFO_IN   "MIMO_RECIEVER_FIFO_IN_JOB_REQ"
#define PUSCH_RECIEVER  "PUSCH_RECIEVER"


struct MimoRecieverIO
{
    Complex *input_channel_matrix;
    Complex *input_noise_matrix;
    uint8_t num_receive_antennas;
    uint8_t num_of_layers;
    uint16_t current_RB_in_TB;
    uint16_t RB_size;
    uint8_t Q;
    uint8_t diagonal_covariance;
    uint8_t noise_resoloution;  // The number of noise samples estimation per each receive antenna per RB
    Complex *output_matrix_per_RB;
    uint16_t *output_SNR_per_RB;

};

 struct pusch_mimo_receiver_API{
 		    virtual void receiver(Complex* input_channel_matrix, Complex* input_noise_matrix, uint8_t num_receive_antennas, uint8_t num_of_layers, uint8_t Q, uint16_t RB_size, uint8_t diagonal_covariance, uint8_t channel_res, Complex* output_matrix)=0;
            
 };


#endif /* API_MODULE_API_I_MIMO_RECEIVER_API_H_ */
