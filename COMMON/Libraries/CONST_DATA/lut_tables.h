/******************************************************************
 * lut_tables.cc
 * Author:  Elad (Algorithm)
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef LUT_TABLES_H
#define LUT_TABLES_H
#include "i_api_common.h"


// values of exp(j*[-pi...pi])
extern  uint32_t exp_table[512];


// values of 1/x x=[0,1,2,..275]
extern  int16_t inv_table[276];

extern int16_t   inv_2x_table[2049];



#endif
