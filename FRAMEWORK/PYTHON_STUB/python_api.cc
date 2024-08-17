/******************************************************************
 * cpy5gstub.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#include "cpy5gstub.h"

extern Cpy5Gstub sim5g[];
static uint32_t curr_cell = 0;

//Python wrapper function

list GetProfData()
{
	return sim5g[curr_cell].GetProfileCnt();
}

list GetLogs()
{
	return sim5g[curr_cell].GetLogs();
}

std::string  Init(std::string tmp)
{
	return sim5g[curr_cell].Init(tmp);
}

std::string  RestartAgain()
{
	return sim5g[curr_cell].Restart();
}


list GetMemAreasNames()
{
	return sim5g[curr_cell].GetMemAreasNames();
}

list GetMemAreasSizes()
{
	return sim5g[curr_cell].GetMemAreasSizes();
}


list GetModulesNames()
{
//	list tmp;
//	tmp[0]= ",";
	return sim5g[curr_cell].GetModulesNames();
	//return tmp;
}

std::string CallModule(std::string mod_name)
{
	return sim5g[curr_cell].CallModule(mod_name);
}

std::string StopModule(std::string mod_name)
{
	return sim5g[curr_cell].StopModule(mod_name);
}

CMemAccess  GetMemArea(std::string name)
{
	return *sim5g[curr_cell].GetMemArea(name);
}

std::string GetTime()
{
	return sim5g[curr_cell].GetTime();
}

std::string Run(uint32_t usecs)
{
	return sim5g[curr_cell].Run(usecs);
}

std::string Version()
{
	return sim5g[curr_cell].Version();
}

std::string StopLogs()
{
	return sim5g[curr_cell].OutLogOff();
}

std::string StartLogs()
{
	return sim5g[curr_cell].OutLogOn();
}


std::string RunTest(std::string ModName, std::string TestPath)
{

	return sim5g[curr_cell].RunTest(ModName, TestPath);
}

std::string StopSys()
{

    return sim5g[curr_cell].StopSys();
}

std::string ConfigModule(std::string ModName, uint32_t conf_type, std::string TestPath)
{

    return sim5g[curr_cell].ConfigModule(ModName, conf_type, TestPath);
}

std::string SetLogPriority(uint32_t severity)
{
    return sim5g[curr_cell].SetSeverity(severity);
}

std::string SelectCell(uint32_t cell_ind)
{
    std::string res;
    if(cell_ind < NUM_OBJECTS)
    {
        curr_cell = cell_ind;
        res= "Ok";
    }
    else
    {
        char buf[100];
        sprintf(buf, "Failed. Max value should be less then %d", NUM_OBJECTS-1);
        res = buf;
    }
    return res;

}
std::string GetCurrentCell()
{
    std::string res;
    char buf[100];
    sprintf(buf, " Current cell is %d", curr_cell);
    res = buf;
    return res;
}

//**********************************************************************************
//DebugRT features
//**********************************************************************************
std::string SetDebugRT(std::string rt_debug_file)
{
	RTDBG_SetSharedMemFile((char*)rt_debug_file.data());
    return "SetDebugRT" + sim5g[curr_cell].SetFileOut(rt_debug_file);
}

std::string InitDebugRT()
{
	if(RTDBG_ActivateDebugDataCollection()==true)
		return "InitDebugRT: Passed";
	else
		return "InitDebugRT: Failed";
}
std::string AttachDebugRT()
{
	if(RTDBG_AttachDebugData()==true)
		return "AttachDebugRT: Passed";
	else
		return "AttachDebugRT: Failed";
}

std::string StartDebugRT()
{
	RTDBG_Start();
	return "StartDebugRT: Passed";
}
std::string StopDebugRT()
{
	RTDBG_Stop();
	return "StopDebugRT: Passed";
}
std::string StatusDebugRT()
{
	std::string status_str;
	RTDBG_STATUS_T status;
	char *version= RTDBG_GetVersion();
	printf("RT_DEBUG tools version: %s\n", version);
	status= RTDBG_GetStatus();
	switch(status){
		case RTDBG_INIT:
			status_str= "RTDBG_INIT";
			break;
		case RTDBG_RUNNING_CONSUMER:
			status_str= "RTDBG_RUNNING_CONSUMER";
			break;
		case RTDBG_RUNNING_PRODUCER:
			status_str= "RTDBG_RUNNING_PRODUCER";
			break;
		case RTDBG_STOPPED:
			status_str= "RTDBG_STOPPED";
			break;
		default:
			status_str= "Error Wrong Status";
	}

	printf("RT_DEBUG Status: %s\n", status_str.data());
    return status_str;
}
std::string ExtractDebugRT(std::string out_file)
{
    if(RTDBG_ExtractDebugData((char*)out_file.data())==true)
    	return "ExtractDebugRT: Passed";
    else
    	return "ExtractDebugRT: Failed";
}

std::string CloseDebugRT()
{
	RTDBG_Close();
    return "CloseDebugRT: Passed";
}
int32_t ReadEvCnt(std::string grp_name, std::string cnt_name)
{
	char tmp_grp_name[100], out[200];
	int32_t i, j, n_evt, n_grp = RTDBG_GetGrpNum();
	RT_counter *cnt_p = NULL;
	for(i=0; i< n_grp; i++)
	{
		if(RTDBG_GetGrpName(i, tmp_grp_name)==false)
		{
			continue;
		}

		if(strcmp(grp_name.data(),tmp_grp_name)!=0)
		{
			continue;
		}

		n_evt= RTDBG_GetCountersNum(i);
		for(j=0; j<n_evt; j++)
		{
			RT_counter *cnt_p = RTDBG_GetCounter(i, j);
			if(strcmp(cnt_p->cnt_name, cnt_name.data())==0)
			{
				return cnt_p->val;
			}
		}
	}
	printf("Evt counter was not found\n");
	return -1;
}

list EvCntList()
{
	list res;
	char grp_name[100], out[300];
	std::vector<std::string> event_cnt;
		typename std::vector<std::string>::iterator iter;
	int32_t i, j, n_evt, n_grp = RTDBG_GetGrpNum();
	if(n_grp==-1)
		return res;

	for(i=0; i< n_grp; i++)
	{
		n_evt= RTDBG_GetCountersNum(i);
		if(RTDBG_GetGrpName(i, grp_name)==false)
		{
			continue;
		}
		for(j=0; j<n_evt; j++)
		{
			RT_counter *cnt_p = RTDBG_GetCounter(i, j);
			sprintf(out,"%s.%s= %ld",grp_name, cnt_p->cnt_name, cnt_p->val);
			event_cnt.push_back(out);
		}
	}
	for (iter = event_cnt.begin(); iter != event_cnt.end(); ++iter) {
		res.append(*iter);
	}
	return res;
}
//Expose classes to PYTHON

BOOST_PYTHON_MODULE(py5Gstub)
{
	//fs::python::init_and_export_converters();
	class_<CMemAccess>("CMemAccess")
		.def("popfloat",&CMemAccess::PopFloat)
		.def("popstring",&CMemAccess::PopString)
		.def("popu32",&CMemAccess::PopU32)
		.def("popu8",&CMemAccess::PopU8)
		.def("pushstr",&CMemAccess::PushStr)
		.def("pushfloat",&CMemAccess::PushFloat)
		.def("pushu32",&CMemAccess::PushU32)
		.def("nextw",&CMemAccess::SetNextWrBuf)
		.def("name" ,&CMemAccess::GetName)
		.def("get_chunk32" ,&CMemAccess::GetChunkU32)
		.def("save_chunk32" ,&CMemAccess::SaveChunkU32)
		.def("nextr",&CMemAccess::SetNextRdBuf);



	class_<Cpy5Gstub>("Cpy5Gstub", init<>())
		.def("run",&Cpy5Gstub::Run)
		.def("time",&Cpy5Gstub::GetTime)
		.def("getmem",&Cpy5Gstub::GetMemArea,return_value_policy<manage_new_object>())
		.def("callmod",&Cpy5Gstub::CallModule)
		.def("modnames",&Cpy5Gstub::GetModulesNames)
		.def("areas",&Cpy5Gstub::GetMemAreasNames)
		.def("version",&Cpy5Gstub::Version)
		.def("init",&Cpy5Gstub::Init);

		def("init", Init);
		def("areanames",GetMemAreasNames);
		def("areas_sizes",GetMemAreasSizes);
		def("modulenames",GetModulesNames);
		def("callmod",CallModule);
		def("stopmod",StopModule);
		def("getmem",GetMemArea);
		def("time",GetTime);
		def("run",Run);
		def("ver", Version);
		def("prof",GetProfData);
		def("logs",GetLogs);
		def("stop_logs", StopLogs);
		def("start_logs", StartLogs);
		def("run_test", RunTest);
		def("stop", StopSys);
		def("mconfig", ConfigModule);
		def("logprio", SetLogPriority);
		def("select_cell", SelectCell);
		def("cur_cell", GetCurrentCell);
		def("reset",RestartAgain);
		//RT Debug commands
		def("debug_shmem",SetDebugRT);
		def("debug_init",InitDebugRT);
		def("debug_attach",AttachDebugRT);
		def("debug_start",StartDebugRT);
		def("debug_stop",StopDebugRT);
		def("debug_status",StatusDebugRT);
		def("debug_extract",ExtractDebugRT);
		def("debug_close",CloseDebugRT);

		def("debug_readcnt",ReadEvCnt);
		def("debug_cntl",EvCntList);
}

//		.def("version",&Cpy5Gstub::Version)
