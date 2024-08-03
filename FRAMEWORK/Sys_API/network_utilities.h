/*****************************************************************
*   File Name:		network_utilities.h
*
*   Description:	This file includes functions and macros
*   				regarding to network utilities that are
*   				available for both RU and DU.
*
*   Author:			Y. Keshet
*
*
*
*
*   Information about Endianness in general and ORAN protocol:
*   ==========================================================
*
*   The bits layout of ORAN protocol described in (*) is in Big Endian (BE).
*   Since the network protocol we are using is Ethernet, it means
*   that the Network Byte Order is Big Endian.
*   [Network Bit Order is transparent. It's the NIC job to translate
*   the network bit order between the processor's architecture endianness
*   and the network protocol's endianness. By the way, Ethernet network bit
*   order is Little Endian (LE) ].
*   However, our various machines (VM on Intel architecture and target
*   on ARM architecture) are Little Endian.
*
*   In order to be compatible with a Big Endian Network protocol, a
*   Little Endian machine must follow these rules:
*   1.	All structures that compose ORAN packets must be declared
*   	as __attribute__((packed))
*   2.	Bit-fields:
*   	2.1 Bit-field's field order should be declared according to
*   		the endianness: LE - LSB to MSB,  BE - MSB to LSB.
*   	2.2 If the Bit-field size is greater than 1 byte, then
*   		it should be declared as a union (for byte-swapping
*   		purposes in LE).
*   3.	Every field or bit-field whose size is larger than 1 byte
*   	must be byte-swapped in LE.
*   4.	When serializing: first, set data. Then (if needed), swap bytes.
*   5.	When parsing:     first(if needed), swap bytes. Then, parse data.
*
*	(*) - http://confluence-il:8090/display/PHY/O-RAN?preview=/45387932/45391420/ORAN-WG4.CUS.0-v01.00.pdf
*
*
*
*
*	Copyright (c) 2019 - 2020 Airspan Networks
*   All Rights Reserved.
*   Confidential and Proprietary - Airspan Networks
*****************************************************************/
#ifndef NETWORK_UTILITIES_H__
#define NETWORK_UTILITIES_H__


#include "i_api_common.h"


//#define ORAN_BIG_ENDIAN		1					//	Big Endian architecture
//#define ORAN_LITTLE_ENDIAN	2					//	Little Endian architecture
//#define ORAN_BYTE_ORDER		ORAN_LITTLE_ENDIAN	//	This is the definition of the processor's endianness



//	Swapping bytes for 16-bit variable:
#define BYTE_SWAPPING_16(v)		(	\
		(( (uint16_t)(v) & UINT16_C(0x00ff) ) << 8) | 	\
		(( (uint16_t)(v) & UINT16_C(0xff00) ) >> 8)		\
		)


//	Swapping bytes for 32-bit variable:
#define BYTE_SWAPPING_32(v)		(	\
	   ( ( (uint32_t)(v) & UINT32_C(0x000000ff) ) << 24) |	\
	   ( ( (uint32_t)(v) & UINT32_C(0x0000ff00) ) <<  8) |	\
	   ( ( (uint32_t)(v) & UINT32_C(0x00ff0000) ) >>  8) |	\
	   ( ( (uint32_t)(v) & UINT32_C(0xff000000) ) >> 24)	\
	   )


//	Swapping bytes for 64-bit variable:
#define BYTE_SWAPPING_64(v) 	(	\
		( ( (uint64_t)(v) & UINT64_C(0x00000000000000ff) ) << 56) |		\
		( ( (uint64_t)(v) & UINT64_C(0x000000000000ff00) ) << 40) |		\
		( ( (uint64_t)(v) & UINT64_C(0x0000000000ff0000) ) << 24) |		\
		( ( (uint64_t)(v) & UINT64_C(0x00000000ff000000) ) <<  8) |		\
		( ( (uint64_t)(v) & UINT64_C(0x000000ff00000000) ) >>  8) |		\
		( ( (uint64_t)(v) & UINT64_C(0x0000ff0000000000) ) >> 24) |		\
		( ( (uint64_t)(v) & UINT64_C(0x00ff000000000000) ) >> 40) |		\
		( ( (uint64_t)(v) & UINT64_C(0xff00000000000000) ) >> 56)		\
		)

class NetworkUtilities
{
private:

public:


	/**********************************************************************
	 *@brief	This function reverse the byte order of a 16-bit variable.
	 *@brief
	 *@brief	Input:	val - pointer to uint16_t variable
	 *@brief
	 *@brief	Output:	None
	 *@brief
	 **********************************************************************/
	static void ByteSwapping_16(uint16_t*   val)
	{
		*val = BYTE_SWAPPING_16(*val);
	}


	/**********************************************************************
	 *@brief	This function reverse the byte order of a 32-bit variable.
	 *@brief
	 *@brief	Input:	val - pointer to uint32_t variable
	 *@brief
	 *@brief	Output:	None
	 *@brief
	 **********************************************************************/
	static void ByteSwapping_32(uint32_t* val)
	{
		*val = BYTE_SWAPPING_32(*val);
	}


	/**********************************************************************
	 *@brief	This function reverse the byte order of a 64-bit variable.
	 *@brief
	 *@brief	Input:	val - pointer to uint64_t variable
	 *@brief
	 *@brief	Output:	None
	 *@brief
	 **********************************************************************/
	static void ByteSwapping_64(uint64_t* val)
	{
		*val = BYTE_SWAPPING_64(*val);
	}


};



#endif		//	NETWORK_UTILITIES_H__
