/*
 * i_est_mimo_api.h
 *
 *  Created on: Oct 11, 2018
 *      Author: dmatas
 */

#ifndef API_MODULE_API_I_EST_MIMO_API_H_
#define API_MODULE_API_I_EST_MIMO_API_H_


#define TB_PUSCH_DMRS_IN    "TB_PUSCH_DMRS_IN"
#define TB_PUSCH_DMRS_OUT_FIX    "TB_PUSCH_DMRS_OUT_FIX"
#define TB_PUSCH_RS_IN      "TB_PUSCH_RS_IN"

#define TB_PUSCH_REF_DATA   "TB_PUSCH_REF_DATA"
#define TB_PUSCH_TEST_NOISE "TB_PUSCH_REF_DATA"


#include "i_pusch_est_api.h"

struct EstMimoIO
{
    Complex  *dmrs;
    Complex *reference_rs;
    EstimationHandlerIO est_parameters;
};


#define EST_MIMO_MODULE "EST_MIMO"
// Name of input FIFO for
#define EST_MIMO_FIFO_IN   "EST_MIMO_FIFO_IN_JOB_REQ"

#endif /* API_MODULE_API_I_EST_MIMO_API_H_ */
