/******************************************************************
 * cpy5gstub.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef CPY5GSTUB_H_
#define CPY5GSTUB_H_
#include "i_api_common.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "csim_control.h"
#include <boost/python.hpp>
#include <boost/thread.hpp>
#include "i_handler_api.h"
#include "rt_debug_api.h"
#ifdef RU_PHY
#include "ru_version.h"
#else
#include "du_version.h"
#endif
//#include <boost/date_time.hpp>


using namespace boost::python;
using namespace ns_5g_phy;
/******************************************************************************************//*!
 *@class CMemAccess
 *@brief The purpose of this class is :
 *@brief Implementation wrapper around CMemArea class to make user friendly access
 *@brief to CMemArea buffers via Python shell
 *********************************************************************************************/

class CMemAccess
{
	CMemArea *mem_area_p_;
public:
	void SetMemArea(CMemArea *mearea_p_)
	{
		mem_area_p_= mearea_p_;
	}
	std::string GetName()
	{
		std::string tmp(mem_area_p_->name_);
		return tmp;
	}

	CMemAccess()
	{
		mem_area_p_ = NULL;
	}

	//Methods for access to CMemArea storage as data chunks.

	/*Function: GetChunk

	*/
	list GetChunkU32(int32_t prev_indx)
	{
        list tmp;
        uint32_t *addr=(uint32_t *)mem_area_p_->GetPrevChunk(prev_indx);
        ASSERT(addr != NULL);
        uint32_t i, n= mem_area_p_->GetDataSize()/sizeof(uint32_t);
        for(i=0; i< n; i++)
        {
            tmp.append(addr[i]);
        }
        return tmp;

	}

	void SaveChunkU32(int32_t prev_indx, list vec)
    {
        uint32_t *addr=(uint32_t*)mem_area_p_->GetPrevChunk(prev_indx);
        uint32_t i, vsize = len(vec) , n= mem_area_p_->GetDataSize()/sizeof(uint32_t);
        ASSERT(mem_area_p_->PushFIFO() != NULL);
        ASSERT(vsize <= n);
        for(i=0; i< n; i++)
        {
            if(i < vsize)
                addr[i]= extract<uint32_t>(vec[i]);
            else
                addr[i] = 0;
        }

    }

	//Methods for access to CMemArea storage as FIFO
	list  PopFloat()
	{
		list tmp;
		float *addr=(float *)mem_area_p_->PopFIFO();
		ASSERT(addr != NULL);
		uint32_t i, n= mem_area_p_->GetDataSize()/sizeof(float);
		for(i=0; i< n; i++)
		{
			tmp.append(addr[i]);
		}
		return tmp;
	}
	std::string PopString()
	{
		char *addr= (char *)mem_area_p_->PopFIFO();;
		ASSERT(addr != NULL);
		std::string res(addr);
		return res;;
	}

	list PopU32()
	{
		list tmp;
		uint32_t *addr=(uint32_t *)mem_area_p_->PopFIFO();
		ASSERT(addr != NULL);
		uint32_t i, n= mem_area_p_->GetDataSize()/sizeof(uint32_t);
		for(i=0; i< n; i++)
		{
			tmp.append(addr[i]);
		}
		return tmp;

	}

    list PopU8()
    {
        list tmp;
        uint8_t *addr=(uint8_t *)mem_area_p_->PopFIFO();
        ASSERT(addr != NULL);
        uint32_t i, n= mem_area_p_->GetDataSize()/sizeof(uint8_t);
        for(i=0; i< n; i++)
        {
            tmp.append(addr[i]);
        }
        return tmp;

    }

	void PushStr(std::string str)
	{
		char *addr= (char *)mem_area_p_->AllocFIFO();
		ASSERT(mem_area_p_->PushFIFO() != NULL);
		uint32_t i = str.size(), n= mem_area_p_->GetDataSize();
		ASSERT(i < n);
		const char* ptr= str.data();
		strcmp(addr, ptr);
		memset(addr+i, 0, n-i);
	}

	void PushFloat(list vec)
	{
		float *addr=(float *)mem_area_p_->AllocFIFO();
		ASSERT(mem_area_p_->PushFIFO() != NULL);
		uint32_t i, vsize = len(vec), n= mem_area_p_->GetDataSize()/sizeof(float);
		ASSERT(vsize <= n);
		for(i=0; i< n; i++)
		{
			if(i < vsize)
				addr[i]= extract<float>(vec[i]);
			else
				addr[i] = 0;
		}
	}

	void PushU32(list vec)
	{
		uint32_t *addr=(uint32_t*)mem_area_p_->AllocFIFO();
		uint32_t i, vsize = len(vec) , n= mem_area_p_->GetDataSize()/sizeof(uint32_t);
		ASSERT(mem_area_p_->PushFIFO() != NULL);
		ASSERT(vsize <= n);
		for(i=0; i< n; i++)
		{
			if(i < vsize)
				addr[i]= extract<uint32_t>(vec[i]);
			else
				addr[i] = 0;
		}
	}

	void SetNextWrBuf()
	{
		mem_area_p_->PushFIFO();
	}

	void SetNextRdBuf()
	{
		mem_area_p_->PopFIFO();
	}

};

/******************************************************************************************//*!
 *@class Cpy5Gstub
 *@brief The purpose of this class is :
 *@brief Implementation main interface with Python shell
  *********************************************************************************************/
class Cpy5Gstub {
	CSimControl sim_control_;
	CMemArea		**mem_areas_p_;
	CModuleInfo		*modules_p_;
	uint32_t 		num_modules_;
	uint32_t		num_areas_;
	uint32_t		sys_time_idx_;
	std::ofstream	output_file_;
	std::ofstream	default_out_file_;
	CMemAccess		mem_access_tab_[MAX_AREAS];
	std::string		mem_areas_names_[MAX_AREAS];
	uint32_t		mem_areas_sizes_[MAX_AREAS];
	std::string		module_names_[MAX_MODULES];
	bool_t			out_2_terminal_;
	bool_t			out_2_file_;
	std::string		debug_data_file_name_;
	std::string		default_file_name_;
	boost::thread   out_thread_;
	Handler_API     *handler_module_p_;


	void OutThread(uint32_t num);

public:
	//***********************************************************
	uint32_t AS_PHY_Connect();
	uint32_t  GL_FAPI_cfg(CONFIGREQUESTStruct *pInitParam);
	uint32_t FAPI_parser(char* msg);
	//***********************************************************
	std::string Init(std::string &path);
	list GetMemAreasNames();
	list GetMemAreasSizes();
	list GetModulesNames();
	std::string CallModule(std::string mod_name);
	std::string StopModule(std::string mod_name);
	std::string StopSys();
	CMemAccess*  GetMemArea(std::string name);
	std::string GetTime();
	std::string Run(uint32_t usecs);
	std::string Restart();
	std::string Version();
	list GetProfileCnt();
	list GetLogs();
	std::string SetFileOut(std::string name);
	std::string OutLogOn();
	std::string OutLogOff();
	std::string RunTest(std::string mod_name, std::string TestPath);
	std::string ConfigModule(std::string mod_name, uint32_t config_type, std::string TestPath);
	std::string SetSeverity(uint32_t prio);
	void SendCLI(char* command_str, char **respond)
	{
		sim_control_.SendCLI(command_str, respond);
	}

	Cpy5Gstub()
	{
		ResetData();
	}
	void ResetData();
	Cpy5Gstub(const Cpy5Gstub& x)
	{
		ResetData();
		ASSERT(0);
	}

	virtual ~Cpy5Gstub();
};

#endif /* CPY5GSTUB_H_ */
