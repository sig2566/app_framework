/******************************************************************
 * i_de_scrambler_api.h
 * Author: Eitan Glotman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_DE_SCRAMBLER_API_H_
#define API_MODULE_API_I_DE_SCRAMBLER_API_H_


#define		MAX_LLR_BUF_SIZE_IN_BYTES	350000

#define DE_SCRAMBLER_MODULE "DE_SCRAMBLER"
struct DE_SCRAMBLER_call_api{
    virtual uint32_t Process(uint32_t len , int8_t *llr_ptr, uint32_t *prbs_ptr) =0;

};
 

#endif /* API_MODULE_API_I_DE_SCRAMBLER_API_H_ */
