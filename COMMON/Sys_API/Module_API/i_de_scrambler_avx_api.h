/******************************************************************
 * i_de_scrambler_avx_api.h
 * Author: Dikshant Chitkara
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_DE_SCRAMBLER_AVX_API_H_
#define API_MODULE_API_I_DE_SCRAMBLER_AVX_API_H_

#include "i_code_block_rx_main_api.h"
#define		MAX_LLR_BUF_SIZE_IN_BYTES	360000

#define DE_SCRAMBLER_AVX_MODULE "DE_SCRAMBLER_AVX"
struct DE_SCRAMBLER_AVX_api{
    virtual uint32_t Descrambler_avx(Descrambler Descr,uint8_t *scr_out) =0;
    virtual uint32_t Prbs_Gen(uint8_t *Prbs,uint32_t c_init, uint32_t prbs_length) =0;
};
 

#endif /* API_MODULE_API_I_DE_SCRAMBLER_API_H_ */
