/*
 * i_code_block_main_api.h
 *
 *  Created on: Aug 5, 2018
 *      Author: dmatas
 */

#ifndef API_MODULE_API_I_PUSCH_DEMODULATION_API_H_
#define API_MODULE_API_I_PUSCH_DEMODULATION_API_H_


#define TB_PUSCH_DATA_IN    "TB_PUSCH_DATA_IN"
#define TB_PUSCH_DMRS_IN    "TB_PUSCH_DMRS_IN"
#define TB_PUSCH_DMRS_OUT_FIX    "TB_PUSCH_DMRS_OUT_FIX"
#define TB_PUSCH_RS_IN      "TB_PUSCH_RS_IN"
#define TB_PUSCH_DATA_OUT   "TB_PUSCH_DATA_OUT"
#define TB_PUSCH_LLR_DATA_OUT   "TB_PUSCH_LLR_DATA_OUT"

#define TB_PUSCH_REF_DATA   "TB_PUSCH_REF_DATA"
#define TB_PUSCH_TEST_NOISE "TB_PUSCH_REF_DATA"


#include "i_pusch_est_api.h"
#include "i_mimo_receiver_api.h"

struct PUSCHDemodulationIO
{
    Complex  *dmrs;
    Complex *reference_rs;
    EstimationHandlerIO est_parameters;
    Complex *data;
    Complex *data_equalized_out;
    int16_t *llr_out;
};


#define PUSCH_DEMODE_MODULE "PUSCH_DEMODULATION"
// Name of input FIFO for
#define PUSCH_DEMODULATION_FIFO_IN   "PUSCH_DEMODULATION_FIFO_IN_JOB_REQ"

#endif /* API_MODULE_API_I_PUSCH_FIX_API_H_ */
