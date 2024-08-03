/*
 * i_use_framework.h
 *
 *  Created on: April 25, 2018
 *      Author: David Matas
 */
#include <time.h>

#ifndef API_USE_FRAMEWORK_H_
#define API_USE_FRAMEWORK_H_


using namespace ns_5g_phy;
class UseFrameworkclass
{

public:
	IModuleControlCallBackAPI* callback_framework_class_;
	void InitCallFramework(IModuleControlCallBackAPI* callback_framework_class)
	{
		this->callback_framework_class_=callback_framework_class;
	}
};

#endif
