/******************************************************************
 * i_const_data_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "const_data.h"


EResultT CConstData::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
{

	CBaseModule::IInit(callback_ptr, target_api, init_info);

    return E_OK;
}
EResultT CConstData::IConfigure(EConfigId id, void *in, void **out)
{
    *out = (void*)(static_cast<CConst_Data_api*>(this));
	return E_OK;
}

CConstData::CConstData()
{

}

class CConstData     *g_module_ptr = NULL;


//API function to connect with ITarget API
extern "C" uint32_t API_CONNECT_FUNC(void **target_ptr)
{
	g_module_ptr = new CConstData();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}

