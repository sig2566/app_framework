/******************************************************************
 * i_fapi_builder_api
 * Author: Yafit Feldman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_FAPI_GEN
#define API_MODULE_API_I_FAPI_GEN

#include "i_api_common.h"
#include "fapi_p.h"


#define FAPI_GEN_MODULE "FAPI_GEN"

struct FAPI_builder_api{
	//- Load two XML files: enodeBConfig.xml, session_tst.xml
    virtual EResultT LoadData(char *econfig_path, char *session_path, char *sourch_path, char *test_suffix, char *test_path) =0;
    //- Return  pointer to enodeB configuration message
    virtual EResultT GetConfiguration(char **config_msg) = 0;
    //Return the FAPI messages per the next slot.
    virtual EResultT GetNextSlotFAPI(uint32_t frame_num, uint32_t slot_num, SLOT_ptr *next_slot) =0;
    virtual EResultT GetError(char **err_str) = 0;
    virtual CONFIGREQUESTStruct *GetEnodeBStruct_p_() = 0;
    virtual SlotGroup *GetSlotGroupStr_p_() = 0;
    virtual SLOT_ptr GetSlotPtr_p_() = 0;


};


#endif /* API_MODULE_API_I_FAPI_GEN */
