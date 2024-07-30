/*
 * CSimControl.h
 *
 *  Created on: Feb 11, 2018
 *      Author: swuser
 */
#include "i_api_common.h"

#ifndef CSIMCONTROL_H_
#define CSIMCONTROL_H_
#include <vector>
#include <string>
using namespace ns_5g_phy;

class CSimControl : public IGL_ControlCallBackAPI
{
	ITarget    		*target_ptr_;
	IGL_ControlAPI  *control_ptr_;
	IGL_DebugAPI	*debug_ptr_;
	CMemArea		*mem_areas_p_[MAX_AREAS];
	CModuleInfo		modules_[MAX_MODULES];
	uint32_t 			num_modules_;
	uint32_t			num_areas_;
	uint32_t			sys_time_idx_;
	CMemArea**		log_tab_p_;		
	std::vector<std::string> prof_info_;
	std::vector<std::string> last_logs_;


public:
	CSimControl();
	virtual ~CSimControl();
	void Init(const char * cfg_path);
	void RestartHot();
	void Stop();
	void Start(uint32_t sfn, uint32_t usec);
	CMemArea **GetMemAreas(uint32_t *nuareas_);
	CModuleInfo* GetModules(uint32_t *numodules_);
	void GetInfo(EDebugOperationT cmd, void *input, void **out);
	virtual EResultT IStopRequest(ESeverityT severity);
	virtual EResultT I_TTI_evt(SysTimeT *sys_time_p);
	SysTimeT GetTime();
	std::vector<std::string> &GetProfilerCntrs();
	std::vector<std::string> &GetLastLogs();
	EResultT SetSeverity(ESeverityT prio)
	{
	    return debug_ptr_->ISetLogSeverity(prio);
	}
	void SendCLI(char* command_str, char **respond)
	{
		debug_ptr_->ISendCLI(command_str, respond);
	}


};

#endif /* CSIMCONTROL_H_ */

