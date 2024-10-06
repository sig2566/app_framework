
#ifndef MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_
#define MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_
#include "i_api_common.h"
#include "i_const_data_api.h"
#include "i_sample_module_api.h"
#include "version.h"

using namespace ai_framework_proj;


/******************************************************************************************//*!
 *@class CSAMPLE_MODULE
 *@brief The purpose of this class is :
 *@brief It is example of the component implementation.
 *@brief In the case of creation new class only MANDATORY code must be copied. Other code is optional.
 *@brief They are used for demonstration of the framework services.
 *********************************************************************************************/
class CSAMPLE_MODULE : public CBaseModule, public sample_module_api
{
    //Examples of useful data types (optional)
    CMemArea *in_ptr, *out_ptr, *mem_pool_, *job_req_, *job_rsp_; //Example of pointers to data areas
    CProfileCnt call_prof_;  //Example of profiler counter
    IModuleControlAPI  *test_module_p_; //Example pointer to module API.
    IModuleControlAPI  *const_data_module_p_;
    CConst_Data_api *const_api_p_;
public:
    virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info);
    virtual EResultT IColdStart();
    virtual EResultT ICall(uint32_t param);
    virtual EResultT IConfigure(EConfigId id, void *in, void **out);
    virtual uint32_t calc_sum_uint_16();
    CSAMPLE_MODULE();
    CSAMPLE_MODULE(const char *mod_name);
};




#endif /* MODULES_SAMPLE_MODULE_SAMPLE_MODULE_H_ */
