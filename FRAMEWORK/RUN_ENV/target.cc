/*
 * target.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: ishoihet
 */

#include "target.h"

EResultT CTarget::IGetTarget(EInterfaceT api_type, uint32_t id, void **gl_api_ptr)
{
	if(api_type == E_CONTROL_API)
		*gl_api_ptr =(void*)static_cast<IGL_ControlAPI*>(&rse_control_);
	else
		*gl_api_ptr =(void*)static_cast<IGL_DebugAPI*>(&rse_control_);
	return E_OK;
}

EResultT CTarget::IGetVersionInfo(char** module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char** add_info)
{
	char tmp_mod_name[]= MOD_NAME;
	strcpy(*module_name, tmp_mod_name);
	*major_ver= MAJOR_ID;
	*minor_ver= MINOR_ID;
	*build_num= 0;
	add_info[0]= 0;

	return E_OK;
}
CTarget::CTarget()
{
	rse_control_.SetTarget(this);
}

CTarget *g_target_ptr;


//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	ITarget* tmp_ptr;
	g_target_ptr = new CTarget;
	tmp_ptr = static_cast<ITarget*>(g_target_ptr);
	*target_ptr= (void*)tmp_ptr;
	return 0;
}

