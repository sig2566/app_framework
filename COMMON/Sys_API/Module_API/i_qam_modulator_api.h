/******************************************************************
 * i_qam_modulator_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_QAM_M_API_H_
#define API_MODULE_API_I_QAM_M_API_H_
#include "i_api_common.h"

#define QAM_MODULE  "QAM_MODULATOR"

struct QAM_modulator_api{
    virtual uint32_t QAM_modulation(uint8_t*, Complex*, uint32_t, uint32_t, Constellation) =0;

};


#endif /* API_MODULE_API_I_QAM_M_API_H_ */
