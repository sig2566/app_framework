/*******************************************************************************
** Copyright(c) 2019 AirSpan TM. All rights reserved.
** Any unauthorized disclosure or publication of the confidential and proprietary
** information to any other party will constitute an infringement of copyright laws.
**
** File   : as_infra_envdep.h
** Author : 
** Date   : 20 Nov 2019
** Purpose: This file contains definitions and declarations for the .....
**
** Notes  : 
**
********************************************************************************
*/

#ifndef __AS_INFRA_ENVDEPH__
#define __AS_INFRA_ENVDEPH__

#include <sys/types.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define AS_INFRA_FALSE 0

#define AS_INFRA_TRUE 1

typedef enum AS_INFRA_RETCODE_ {
    AS_INFRA_RETCODE_ROK = 0,
    AS_INFRA_RETCODE_RFAILED,
    AS_INFRA_RETCODE_ROKDNA,
    AS_INFRA_RETCODE_ROKDUP,
    AS_INFRA_RETCODE_RTIMEOUT,
    AS_INFRA_RETCODE_RSKIP
} AS_INFRA_RETCODE;

#define AS_INFRA_PUBLIC /* public is c default scope */

#define AS_INFRA_PRIVATE AS_INFRA_PUBLIC /* in x86 private is same to AS_INFRA_PUBLIC scope */

#define AS_INFRA_EXTERN extern

#define AS_INFRA_UNUSED(x) (x = x)

#define AS_INFRA_NULL 0

#define AS_INFRA_NULLP 0L

#define AS_INFRA_CRASH_NOW()                                                \
    {                                                              \
        printf("AS_INFRA_CRASH_NOW() in %s(%u)\n", __FUNCTION__, __LINE__); \
        *(char*)0 = 0;                                             \
    }

/* ansi */
#define ARGS(m) m

#define AS_INFRA_RETVALUE(v) return (v) /* return value */

#ifndef VOLATILE 
#define VOLATILE volatile /* volatile */
#endif

#define AS_INFRA_RETVOID return /* void */

#define AS_INFRA_REG1 register /* register 1 - highest priority */

#define AS_INFRA_REG2 register /* register 2 - highest priority */

#define AS_INFRA_CONSTANT const /* constant */

#define AS_INFRA_MIN(x, y) ((x) > (y) ? (y) : (x)) /* get minimum value */

#define AS_INFRA_MAX(x, y) ((x) > (y) ? (x) : (y)) /* get maximum value */
 
#endif /* __AS_INFRA_ENVDEPH__ */
