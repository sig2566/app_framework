/******************************************************************
 * i_CRC_gen_api.h
 * Author: D. Matas
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_CRC_GEN_API_H_
#define API_MODULE_API_I_CRC_GEN_API_H_

#include "global_defines.inc"
// ALL_CRC_BUFFER  - calc. CRC on inp. data buffer
// FIRST_CRC_BUFFER - begin crc calc. on inp. buffer which part of input data
// CONTINUE_CRC_BUFFER - continue crc calc on input buffer as continuous part of input buffer in previous crc calc. function call
// LAST_CRC_BUFFER - finish crc calc as continue of previous crc calc. calls with parts of current data buffer
enum CONFIG_CRC_FLAG {ALL_CRC_BUFFER, FIRST_CRC_BUFFER/*TBD */, CONTINOUS_CRC_BUFFER /* TBD */, LAST_CRC_BUFFER};


// 5G CRC polynom definitions acc. to TS_38212 standard
//#define CRC24A_POLYNOM    0xc3267d80  // [1 1 0 0  0 0 1 1  0 0 1 0  0 1 1 0  0 1 1 1  1 1 0 1  1]                                      //      c       3         2        6        7        d    8
#define CRC24A_POLYNOM    0x01be64c3 // [1 1 0 0  0 0 1 1  0 0 1 0  0 1 1 0  0 1 1 1  1 1 0 1  1]  -for little endians                                    //      c       3         2        6        7        d    8
//#define CRC24B_POLYNOM    0xc0003180  // [1 1 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 1 1  0 0 0 1  1];                                      //     c      0         0        0        3        1     8
#define CRC24B_POLYNOM    0x018c0003  //[1 1 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 1 1  0 0 0 1  1];     // For little    endian
#define CRC24C_POLYNOM    0xd9588b80  // [1 1 0 1  1 0 0 1  0 1 0 1  1 0 0 0  1 0 0 0  1 0 1 1  1]                                      //     d        9        5        8        8        b     8
//#define CRC16_POLYNOM     0x88108000  // [1 0 0 0  1 0 0 0  0 0 0 1  0 0 0 0  1];                                      //     8         8        1        0    8
#define CRC16_POLYNOM     0x00010811  // [0 0 0 1  0 0 0 0  1 0 0 0  0 0 0 1  0 0 0 1];     // For little endian
#define CRC11_POLYNOM     0xe2100000  // [1 1 1 0  0 0 1 0  0 0 0 1]                                      //      e       2         1
#define CRC6_POLYNOM      0xc2000000  // [1 1 0 0  0 0 1]                                      //     c        2
#define NO_CRC            0x0
#define CRC_TOP_BIT_MASK 0x80000000

// List of 5G defined CRCs
enum  CRC_poly{

   CRC_24A,
   CRC_24B,
   CRC_24C,
   CRC_16,
   CRC_11,
   CRC_6

};


struct CRC_gen_call_api
{
     virtual void CRCCalculation( uint8_t *inp_data_ptr, int32_t num_of_inp_data_bits, CRC_poly CRC_format,uint8_t config_crc_flag, uint32_t *CRC_result)=0;
     virtual void Reset() = 0;
};


#endif /* API_MODULE_API_I_CRC_GEN_API_H_ */
