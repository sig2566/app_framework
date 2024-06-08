/******************************************************************
 * cpy5gstub.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#include "cpy5gstub.h"

void Cpy5Gstub::ResetData()
{
	// TODO Auto-generated constructor stub
	uint32_t i;
	num_areas_ = 0;
	num_modules_ = 0;
	sys_time_idx_ = -1;
	
	mem_areas_p_ =  NULL;
	modules_p_ = NULL;
	out_2_terminal_ = FALSE;
	out_2_file_ = FALSE;

}

Cpy5Gstub::~Cpy5Gstub() {
	// TODO Auto-generated destructor stub
}

std::string Cpy5Gstub::SetFileOut(std::string name)
{
	if(name.size() != 0)
	{
		debug_data_file_name_ = name;
		RTDBG_SetSharedMemFile((char*)debug_data_file_name_.data());
	}
	else {
		debug_data_file_name_.clear();
		return "Error: Empty file name";
	}
	return "Passed";
}
std::string Cpy5Gstub::OutLogOn()
{
	out_2_terminal_= TRUE;
	return "Passed";

}
std::string Cpy5Gstub::OutLogOff()
{
	out_2_terminal_= FALSE;
	return "Passed";
}
void Cpy5Gstub::OutThread(uint32_t num)
{

	//boost::posix_time::seconds workTime(1);
	while(1)
	{
		if((out_2_terminal_  == TRUE)|| (out_2_file_ == TRUE)){
			std::vector<std::string> logs= sim_control_.GetLastLogs();
			if(out_2_terminal_){
				//Print logs into default file, which the other windows get last lines via tail -f command
				std::vector<std::string>::iterator it;
				for( it = logs.begin(); it != logs.end(); ++it) {
					default_out_file_ << it->c_str() << "\n";
				}
				default_out_file_.flush();
			}
			if(out_2_file_ == TRUE){
				//Print logs into file
			}

		}
		//boost::this_thread::sleep(workTime);
		sleep(1);
	}


}

std::string Cpy5Gstub::Init(std::string &path)
{
	std::string  res("Passed");
	const char *ptr= path.data();
	sim_control_.Init(ptr);
	modules_p_ = sim_control_.GetModules(&num_modules_);
	mem_areas_p_ =  sim_control_.GetMemAreas(&num_areas_);
	//Open windows for output
	{
		//char oss[1000];
		default_file_name_ = "tmp_out.log";
		default_out_file_.open(default_file_name_);
		//sprintf(oss, "xterm tail -f %s  &", default_file_name_.c_str());
		//system(oss);
		default_out_file_ << "Start logging\n";
		default_out_file_.flush();
	}

	//Open output terminal window and start logging thread
	out_thread_ = boost::thread(&Cpy5Gstub::OutThread, this, 2);

	return res;
}


std::string Cpy5Gstub::StopSys()
{
    std::string  res("Passed");
    sim_control_.Stop();
    return res;
}


list Cpy5Gstub::GetMemAreasNames()
{
	list res;
	uint32_t i;
	for(i=0; i < num_areas_; i++)
	{
//		res.push_back((*mem_areas_p_)[i].GetName());
		mem_areas_names_[i] = (*mem_areas_p_)[i].GetName();
		res.append( mem_areas_names_[i]);
	}
	return res;
}

list Cpy5Gstub::GetMemAreasSizes()
{
	list res;
	uint32_t i;
	for(i=0; i < num_areas_; i++)
	{
		mem_areas_sizes_[i] = (*mem_areas_p_)[i].GetTotalDataSize();
		res.append( mem_areas_sizes_[i]);
	}
	return res;
}



list Cpy5Gstub::GetModulesNames()
{
	list res;
	uint32_t i;
	for(i=0; i < num_modules_; i++)
	{
		//res.push_back(modules_p_[i].module_name_);
		module_names_[i] = modules_p_[i].module_name_;
		res.append(module_names_[i]);
	}
	return res;

}
std::string Cpy5Gstub::CallModule(std::string mod_name)
{
	uint32_t i;
	std::string res("Passed");
	SysTimeT cur_time= sim_control_.GetTime();
	for(i=0; i< num_modules_; i++)
	{
		if(strcmp(modules_p_[i].module_name_, mod_name.data()) == 0)
		{
			modules_p_[i].module_p_->ICall(&cur_time, (uint32_t)E_DEBUG);
			return res;
		}
	}
	return "Wrong module name";
}

std::string Cpy5Gstub::StopModule(std::string mod_name)
{
	uint32_t i;
	std::string res("Passed");
	for(i=0; i< num_modules_; i++)
	{
		if(strcmp(modules_p_[i].module_name_, mod_name.data()) == 0)
		{
			modules_p_[i].module_p_->IStop(E_BP);
			return res;
		}
	}
	return "Wrong module name";
}

CMemAccess*  Cpy5Gstub::GetMemArea(std::string name)
{
	uint32_t i;
	for(i=0; i< num_areas_; i++)
	{
		if(strcmp(name.data(), (*mem_areas_p_)[i].name_)==0)
		{
			mem_access_tab_[i].SetMemArea(&(*mem_areas_p_)[i]);
			return &mem_access_tab_[i];
		}
	}
	return NULL;
}

list Cpy5Gstub::GetProfileCnt()
{
	list res;
	uint32_t i, num_grp = RTDBG_GetGrpNum();;
	std::vector<std::string> prof_cnt;
	typename std::vector<std::string>::iterator iter;
	for(i=0; i< num_grp; i++)
	{
		uint32_t j,  num;
		char buf[1000];
		num = RTDBG_GetProfCntrs(i);
		if(num ==-1)
		{
			return res;
		}
		for(j=0; j< num; j++)
		{
			ProfileData prof_tmp;
			char prof_name[1000];
			char grp_name[100];
			if(RTDBG_GetProfInfo(i, j, &prof_tmp, grp_name, prof_name)==false)
			{
				continue;
			}
			sprintf(buf, "%s, %s, avg=, %lld, max=, %lld, last=, %lld, max time=, %lld, measurements=, %lld\n",
						grp_name, prof_name, prof_tmp.average_cnt_, prof_tmp.max_cnt_, prof_tmp.last_cnt_, prof_tmp.max_cnt_time_, prof_tmp.meas_num_);
			prof_cnt.push_back(buf);
		}
	}
	for (iter = prof_cnt.begin(); iter != prof_cnt.end(); ++iter) {
		res.append(*iter);
	}

	return res;
}

list Cpy5Gstub::GetLogs()
{
	list res;
	std::vector<std::string> &logs_v = sim_control_.GetLastLogs();
	typename std::vector<std::string>::iterator iter;
	for (iter = logs_v.begin(); iter != logs_v.end(); ++iter) {
		res.append(*iter);
	}

	return res;
}


std::string Cpy5Gstub::GetTime()
{
	SysTimeT time= sim_control_.GetTime();
	std::string res;
	char buf[100];
	sprintf(buf,"NF=%d NSF=%d OFFSET=%d",time.nf, time.nsf, time.offset);
	res= buf;
	return res;
}

std::string Cpy5Gstub::Run(uint32_t usecs)
{
	sim_control_.Start(usecs/1000, usecs%1000);
	return "Passed";
}

std::string Cpy5Gstub::Restart()
{
	std::string res("Passed");
	sim_control_.RestartHot();
	return res;
}

std::string Cpy5Gstub::Version()
{
	std::string res("VER1.0");
	return res;
}
std::string Cpy5Gstub::RunTest(std::string mod_name, std::string TestPath)
{
	uint32_t i;
	std::string res("Passed");
	SysTimeT cur_time= sim_control_.GetTime();
	for(i=0; i< num_modules_; i++)
	{
		if(strcmp(modules_p_[i].module_name_, mod_name.data()) == 0)
		{
			EResultT out_res;
			void *ptr_out;
			char *ptr_in= (char*)TestPath.data();
			modules_p_[i].module_p_->IConfigure(e_TEST_CONFIG, (void *)ptr_in, &ptr_out);
			out_res= modules_p_[i].module_p_->ICall(&cur_time, (uint32_t)E_DEBUG);
			if(out_res == E_FAIL)
			{
				res = "Failed";
			}
			return res;
		}
	}
	return "Wrong module name";
}

std::string Cpy5Gstub::ConfigModule(std::string mod_name, uint32_t config_type, std::string TestPath)
{
    uint32_t i;
    std::string res("Passed");
    SysTimeT cur_time= sim_control_.GetTime();
    for(i=0; i< num_modules_; i++)
    {
        if(strcmp(modules_p_[i].module_name_, mod_name.data()) == 0)
        {
            EResultT out_res;
            void *ptr_out;
            char *ptr_in= (char*)TestPath.data();
            out_res= modules_p_[i].module_p_->IConfigure(config_type, (void *)ptr_in, &ptr_out);
            if(out_res == E_FAIL)
            {
                res = "Failed";
            }
            return res;
        }
    }
    return "Wrong module name";
}

std::string Cpy5Gstub::SetSeverity(uint32_t prio)
{

    if(prio <= E_INFO)
    {
        std::string res = "Done";
        sim_control_.SetSeverity((ESeverityT)prio);
        return res;
    }
    else
    {
        std::string res = "Wrong priority request";
        return res;
    }
}


Cpy5Gstub sim5g[MAX_NUM_OF_OBJECTS];
//API function to connect with ITarget API
//extern "C" uint32_t IGetConnectAPI(void **target_ptr)
//{
//     *target_ptr= static_cast<IModuleControlAPI*>(sim5g);
//    return 0;
//}

