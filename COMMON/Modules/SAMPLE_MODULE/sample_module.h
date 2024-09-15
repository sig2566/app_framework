/******************************************************************
 * sample_module.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_
#define MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_
#include "i_api_common.h"
#include <i_const_data_api.h>
#include "version.h"

using namespace ai_framework_proj;
/******************************************************************************************//*!
 *@class CModuleControl
 *@brief The purpose of this class is :
 *@brief It is example of the component implementation.
 *@brief In the case ofcreation new class only MANDATORY code must be copied. Other code is optional.
 *@brief Thet are used for demonstration of the framework services.
 *********************************************************************************************/
class CModuleControl : public IModuleControlAPI
{
    /***********************************************************************************/
    //MANDATORY fields. Only three variables below must be copied into new class
    IModuleControlCallBackAPI* callback_ptr_;
    ITarget*    target_ptr_;
    char mod_name_[MDO_NAME_SIZE];
    /***********************************************************************************/
    //Examples of useful data types (optional)
    CMemArea *in_ptr, *out_ptr, *mem_pool_, *job_req_, *job_rsp_; //Example of pointers to data areas
    CProfileCnt call_prof_;  //Example of profiler counter
    IModuleControlAPI  *test_module_p_; //Example pointer to module API.
    IModuleControlAPI  *const_data_module_p_;
    CConst_Data_api *const_api_p_;
public:
    virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api);
    virtual EResultT IColdStart();
    virtual EResultT IWarmStart();
    virtual EResultT IHotStart();
    virtual EResultT IStop(ESeverityT severity);
    virtual EResultT ICall(uint32_t param);
    virtual EResultT IConfigure(uint32_t id, void *in, void **out);
    virtual EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info);
    CModuleControl(const char *mod_name);
};




#endif /* MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_ */
