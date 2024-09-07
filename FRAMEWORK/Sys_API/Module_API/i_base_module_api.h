/******************************************************************
 * i_base_module_api
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_BASE_MODULE_API_H_
#define API_MODULE_API_I_BASE_MODULE_API_H_
#include "version.h"
#include "i_api_common.h"
namespace ai_framework_proj
{
    /*******************************************************************************************//*!
    *@class CBaseModule
    *@brief The purpose of this class is :
    *@brief Base class for module implementation.
    *@brief It contains basic implementation of IModuleControlAPI API
    ***********************************************************************************************/
    class CBaseModule : public IModuleControlAPI
    {
    public:
        virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api)
        {

            /********************************************************************************************/
            // MANDATORY fields
            // Initialization of the pointers to the framework services.
            callback_ptr_= callback_ptr;
            target_ptr_= target_api;
            /*********************************************************************************************/

            return E_OK;
        }
        virtual EResultT IColdStart()
            {
               return E_OK;
            }
        virtual EResultT IWarmStart()
            {
                return E_OK;
            }
        virtual EResultT IHotStart()
            {
                return E_OK;
            }
        virtual EResultT IStop(ESeverityT severity)
            {
                return E_OK;
            }
        virtual EResultT ICall(uint32_t param)
            {
        		return E_OK;
            }
        virtual EResultT IConfigure(uint32_t id, void *in, void **out)
            {
                return E_OK;
            }
        virtual EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info)
            {
                /*********************************************************************************/
                //MANDATORY code. Return module version information.
                strncpy(module_name, mod_name_, MDO_NAME_SIZE);
                *major_ver= MAJOR_ID;
                /*************************************************************************************/
                *minor_ver= MINOR_ID;
                *build_num= 0;
                strncpy(add_info, TIME_DATE, MDO_NAME_SIZE);
                return E_OK;
                /*********************************************************************************/
            }
        CBaseModule(const char *mod_name)
            {
                /*********************************************************************************/
                //MANDATORY code. Init mandatory fields

                callback_ptr_ = NULL;
                target_ptr_ = NULL;
                strncpy(mod_name_, mod_name, MDO_NAME_SIZE);
                /*********************************************************************************/


            }

        CBaseModule()
        {
            callback_ptr_ = NULL;
            target_ptr_ = NULL;
            strncpy(mod_name_, MOD_NAME, MDO_NAME_SIZE);

        }


    protected:

          /***********************************************************************************/
         //MANDATORY fields. Only three variables below must be copied into new class
         IModuleControlCallBackAPI* callback_ptr_;
         ITarget*    target_ptr_;
         char mod_name_[MDO_NAME_SIZE];
         /***********************************************************************************/

    };
}
#endif /* API_MODULE_API_I_BASE_MODULE_API_H_ */
