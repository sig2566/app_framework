/*
 * i_ru_modules_api.h
 *
 *  Created on: June 26, 2019
 *      Author: E. Maidman
 */

#ifndef RU_MODULES_API_H_
#define RU_MODULES_API_H_

#include <stdio.h>

/******************************************************************************************//*!
 *@class RU_MODULES_API
 *@brief The purpose of this class is :
 *@brief Abstract API Class for all RU modules to inherit
  *********************************************************************************************/
struct RU_MODULES_API
{
	virtual void IgetAllMemoriesSizesAndDefinitions(FILE* file_ptr) = 0;
};


#endif /* RU_MODULES_API_H_ */
