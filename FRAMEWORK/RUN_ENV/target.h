/*
 * target.h
 *
 *  Created on: Feb 19, 2018
 *      Author: swuser
 */

#ifndef TARGET_H_
#define TARGET_H_
#include "crse_control.h"
#include "version.h"
using namespace ns_5g_phy;

class CTarget : public ITarget
{
	CRSE_Control rse_control_;
public:
	virtual EResultT IGetTarget(EInterfaceT api_type, uint32_t id, void **gl_api_ptr);
	virtual EResultT IGetVersionInfo(char** module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char** add_info);
	CTarget();

};





#endif /* TARGET_H_ */
