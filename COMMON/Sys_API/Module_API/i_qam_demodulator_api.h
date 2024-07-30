/******************************************************************
 * i_qam_demodulator_api.h
 * Author: Haim Teicher
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_QAM_DEM_API_H_
#define API_MODULE_API_I_QAM_DEM_API_H_
#include "i_api_common.h"

#define QAM_DEMODULE  "QAM_DEMODULATOR"
#define MAX_DEMODULATOR_IO_SIZE (33600)// = 16800 for e_QAM16 LLRs = 4, for each input sample (8400 samples)
									    // = 4*8334 for e_QAM64 test & debug against MATLAB
#define MAX_QAMDEMOD_OUT_SIZE   (33600) // e_QAM256
#define DEMODULATOR_INPUT_SIZE  (8400) //(8334)  // = 8400 for e_QAM16 RB = 25, SC = 12, symbols = 14, => 4200 {I, Q}
									    // = 8334 for e_QAM64 test & debug against MATLAB

struct QAM_demodulator_API{
    virtual void QAM_demodulation(Complex* input_LLRs, int8_t** output_LLRs, uint16_t* SNR_ptr, uint32_t numLayers, uint16_t numREs, uint32_t Qm) = 0;
};


#endif /* API_MODULE_API_I_QAM_DEM_API_H_ */
