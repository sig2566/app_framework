/*
 * i_avx_de_Interleaver_api.h
 *
 *  Created on: Aug 13, 2019
 *      Author:
 */

#ifndef API_MODULE_API_I_AVX_DE_INTERLEAVER_API_H_
#define API_MODULE_API_I_AVX_DE_INTERLEAVER_API_H_

#include "i_code_block_rx_main_api.h"

#define DE_INTERLEAVER_AVX_MODULE "DE_INTERLEAVER_AVX"

struct DE_INTERLEAVER_AVX_api
{
    virtual uint32_t  DeIntereleaver_avx(struct DeRateMatching *DeRmConfig,struct LdpcDecoder *LDPCConf, int8_t *deInterleaver_in, int8_t *deInterleaver_out)=0;

};
#endif /* API_MODULE_API_I_DE_INTERLEAVER_API_H_ */
