/******************************************************************
 * i_const_data_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#include "const_data.h"


EResultT CConstData::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api)
{

	CBaseModule::IInit(callback_ptr, target_api);

    return E_OK;
}
EResultT CConstData::IConfigure(uint32_t id, void *in, void **out)
{
    *out = (void*)(static_cast<CConst_Data_api*>(this));
	return E_OK;
}

CConstData::CConstData()
{

}

class CConstData     *g_module_ptr = NULL;


//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module_ptr = new CConstData();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;

}

