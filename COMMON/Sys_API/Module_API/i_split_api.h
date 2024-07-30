/*****************************************************************
*   File Name:		i_split_api.h
*   Description:	This file includes definitions and enumerations
*					regarding to the API between O-RAN and GlueLogic.
*   Author:			Y. Keshet
*
*	Copyright (c) 2019 - 2020 Airspan Networks
*   All Rights Reserved.
*   Confidential and Proprietary - Airspan Networks
*****************************************************************/

#ifndef API_MODULE_API_I_SPLIT_API_H__
#define API_MODULE_API_I_SPLIT_API_H__

#include "i_api_common.h"
#include "network_utilities.h"

// Module name
#define ORAN_SPLIT_HANDLER_MODULE	"ORAN"

// Framework mode
#define _RU_

/***************************************************************
 *
 * 			Split 7.2x (ORAN) definitions:
 *
 ***************************************************************/
#define ECPRI_HEADER_SIZE					(8)
#define ECPRI_COMMON_HEADER_SIZE			(4)
#define RADIO_APP_HEADER_MAX_SIZE			(4)

#define ECPRI_RADIO_APP_HEADER_MAX_SIZE		(ECPRI_HEADER_SIZE + RADIO_APP_HEADER_MAX_SIZE)	//	Size in bytes of the eCPRI header and the Radio Application header
																							//	According to p. 95 on:
																							//	http://confluence-il:8090/display/PHY/O-RAN?preview=/45387932/45391420/ORAN-WG4.CUS.0-v01.00.pdf



#define U_PLANE_SECTION_HEADER_MAX_SIZE		6	//	Size in bytes of the U-Plane section header
												//	According to p. 95 on:
												//	http://confluence-il:8090/display/PHY/O-RAN?preview=/45387932/45391420/ORAN-WG4.CUS.0-v01.00.pdf
												//	NOTE: 6 includes udCompHdr and reserved fields that will be present at the future
#define U_PLANE_SECTION_HEADER_SIZE_WITHOUTCOMP		4

#define MAX_SEQUENCE_ID		256					//	SequenceID field in eCPRI header has 8 bits, therefore,
												//	it's maximal value is 2^8=256

//	This enum contains the different types of data flow directions
//	between the O-RU and O_DU
enum DataDirection
{
	kUL = 0,
	kDL = 1
};


//	This enum contains the different types of ORAN Messages' Plane
enum OranMessagePlaneType
{
	kUPlane = 0x00,		//	User Plane
	kCPlane = 0x02,		//	Control Plane
	kSPlane = 0x05		//	Synchronization Plane
};


//	This enum contains the different types of C Plane Messages
enum OranMessageCPlaneType
{
	kType0 = 0,	//	0x00 = Unused Resource Blocks or symbols in DL or UL
	kType1 = 1,	//	0x01 = Most DL/UL radio channels
	kType3 = 3,	//	0x03 = PRACH and mixed-numerology channels
	kType5 = 5,	//	0x05 = UE scheduling information (UE-ID assignment to section)
	kType6 = 6,	//	0x06 = Channel information
	kType7 = 7	//	0x07 = LAA
};


//	This enum contains the different results of the parsing process
enum ParserResult
{
	kNothingToParse = 0,
	kSuccessfulParse
};


//	This enum contains the different types of FFT Size
enum FrameStructureFFTSize : UINT8
{
	kFFTSize_128      = 0b0111,	//	FFT size = 2^7
	kFFTSize_256      = 0b1000,	//	FFT size = 2^8
	kFFTSize_512      = 0b1001,	//	FFT size = 2^9
	kFFTSize_1024     = 0b1010,	//	FFT size = 2^10
	kFFTSize_2048     = 0b1011,	//	FFT size = 2^11
	kFFTSize_4096     = 0b1100,	//	FFT size = 2^12
	kFFTSize_1536     = 0b1101	//	FFT size = 2^9 * 3
};

//	This enum contains the different types of Sub-carrier Spacing (Mu)
enum FrameStructreMu : UINT8
{
	kMu_0 = 0b0000,	  // Sub-Carrier Spacing = 15    [kHz]  , #Slots = 1
	kMu_1 = 0b0001,	  // Sub-Carrier Spacing = 30    [kHz]  , #Slots = 2
	kMu_2 = 0b0010,	  // Sub-Carrier Spacing = 60    [kHz]  , #Slots = 4
	kMu_3 = 0b0011,	  // Sub-Carrier Spacing = 120   [kHz]  , #Slots = 8
	kMu_4 = 0b0100,	  // Sub-Carrier Spacing = 240   [kHz]  , #Slots = 16
	// Extension to apply for PRACH processing
	kMu_12 = 0b1100,  // Sub-Carrier Spacing = 1.25  [kHz]  , #Slots = 1
	kMu_13 = 0b1101,  // Sub-Carrier Spacing = 3.75  [kHz]  , #Slots = 1
	kMu_14 = 0b1110,  // Sub-Carrier Spacing = 5     [kHz]  , #Slots = 1
	kMu_15 = 0b1111   // Sub-Carrier Spacing = 7.5   [kHz]  , #Slots = 1
};

//======================
//======= Header =======
//======================
//	This structure contains fields that are common to all sections in an ORAN message (eCPRI header + Radio App header)
typedef struct
{
	uint32_t sequence_id_;				//	Message Sequence ID

	//	Information regarding to mapping:
	uint32_t du_port_id_;				//	DU port ID
	uint32_t band_sector_id_;			//	Aggregated cell ID (distinguishes bands and sectors supported by the RU)
	uint32_t carrier_component_id_;		//	Distinguishes Carrier Components supported by the RU
	uint32_t stream_id_;				//	Layer \ Spatial stream \ logical flows ID

	//	Information common to the entire message:
	OranMessagePlaneType plane_type_;	//	Message plane type (C \ U)
	DataDirection data_direction_;		//	Message Direction (UL \ DL)
	uint32_t frame_id_;					//	Frame ID (frame number modulo 256)
	uint32_t subframe_id_;				//	Subframe ID
	uint32_t slot_id_;					//	Slot ID
	uint32_t symbol_id_;				//	In case of a C plane message: Start Symbol ID - first symbol number within slot
										//									to which the information of the message is applied
										//	In case of a U plane message: Symbol ID - identifies the symbol number within the slot
	OranMessageCPlaneType section_type_;//	Type of the section/message
}MESSAGE_DESCRIPTOR;


typedef struct
{
	//	Information from allocation:
	uint32_t allocation_id_;		//	Allocation ID
	bool8_t every_other_rb_;		//	Indicate if every RB is used or every other RB is used
									//	True = every other RB is used
									//	False = every RB is used
									//	Example: rb_=1, start_rb_=1, num_rbs_=3 : RBs used are 1,3,5
	bool8_t sym_inc_;				//	Symbol number increment command
	uint32_t start_rb_;				//	Index of the starting RB of data allocation description
	uint32_t num_of_rbs_;			//	Number of contiguous RBs per data allocation description
	uint32_t re_mask_;				//	Resource Element (RE) mask
	uint32_t num_of_symbols_;		//	Number of symbols
}ALLOCATION_GUARD;

//=============================
//======= Section PRACH =======
//=============================
typedef struct
{
	//	Information from allocation:
	uint32_t allocation_id_;		//	Allocation ID
	bool8_t every_other_rb_;		//	Indicate if every RB is used or every other RB is used
									//	True = every other RB is used
									//	False = every RB is used
									//	Example: rb_=1, start_rb_=1, num_rbs_=3 : RBs used are 1,3,5
	bool8_t  sym_inc_;				//	Symbol number increment command
	uint32_t start_rb_;				//	Index of the starting RB of data allocation description
	uint32_t num_of_rbs_;			//	Number of contiguous RBs per data allocation description
	uint32_t re_mask_;				//	Resource Element (RE) mask
	uint32_t num_of_symbols_;		//	Number of symbols
	uint32_t beam_id_;				//	Beam Identifier
	int32_t  frequency_offset_;		//	Frequency offset with respect to the carrier center frequency before additional filtering
									//	and FFT processing in steps of one half the subcarrier spacings

	//	Information regarding to Beamforming:
	int32_t  num_of_weights_;		//	Number of Beamforming weights
									//	If the beamID is already used, then num_of_weights_=0
	Complex* beamforming_weights_;	//	Pointer to array of Beamforming weights
									//	If the beamID is already used, then the pointer will be NULL
}ALLOCATION_PRACH;

//=======================
//======= Section =======
//=======================

typedef struct
{
	//	Information about the allocation:
	uint32_t allocation_id_;		//	Allocation ID (in O-ran section_id)
	bool8_t every_other_rb_;		//	Indicate if every RB is used or every other RB is used
									//	True = every other RB is used
									//	False = every RB is used
									//	Example: rb_=1, start_rb_=1, num_rbs_=3 : RBs used are 1,3,5
	bool8_t  sym_inc_;				//	Symbol number increment command
	uint32_t start_rb_;				//	Index of the starting RB of data allocation description
	uint32_t num_of_rbs_;			//	Number of contiguous RBs per data allocation description
	uint32_t re_mask_;				//	Resource Element (RE) mask
	uint32_t num_of_symbols_;		//	Number of symbols
	uint16_t beam_id_;				//	Beam Identifier

	//	Information regarding to Beamforming:
	int32_t num_of_weights_;		//	Number of Beamforming weights
									//	If the beamID is already used, then num_of_weights_=0
	Complex* beamforming_weights_;	//	Pointer to array of Beamforming weights
									//	If the beamID is already used, then the pointer will be NULL
}ALLOCATION;

typedef struct
{
	//	Information about the allocation:
	uint32_t allocation_id_;		//	Allocation ID (in O-ran section_id)
	bool8_t every_other_rb_;		//	Indicate if every RB is used or every other RB is used
									//	True = every other RB is used
									//	False = every RB is used
									//	Example: rb_=1, start_rb_=1, num_rbs_=3 : RBs used are 1,3,5
	bool8_t  sym_inc_;				//	Symbol number increment command
	uint32_t start_rb_;				//	Index of the starting RB of data allocation description
	uint32_t num_of_rbs_;			//	Number of contiguous RBs per data allocation description
	uint32_t re_mask1_;				//	Resource Element (RE) mask
	uint32_t num_of_symbols_;		//	Number of symbols
	uint16_t beam_id1_;				//	Beam Identifier
	uint32_t re_mask2_;				//	Resource Element (RE) mask
	uint16_t beam_id2_;				//	Beam Identifier
	uint32_t prachOccurrence;       //

	//	Information regarding to Beamforming:
	int32_t num_of_weights1_;		//	Number of Beamforming weights
									//	If the beamID is already used, then num_of_weights_=0
	Complex* beamforming_weights1_;	//	Pointer to array of Beamforming weights
									//	If the beamID is already used, then the pointer will be NULL
	//	Information regarding to Beamforming:
	int32_t num_of_weights2_;		//	Number of Beamforming weights
									//	If the beamID is already used, then num_of_weights_=0
	Complex* beamforming_weights2_;	//	Pointer to array of Beamforming weights
									//	If the beamID is already used, then the pointer will be NULL
}ALLOCATION_ASFH;


typedef struct
{
	//	Information about the allocation:
	uint32_t allocation_id_;		//	allocation ID
	bool8_t every_other_rb_;		//	Indicate if every RB is used or every other RB is used
									//	True = every other RB is used
									//	False = every RB is used
									//	Example: rb_=1, start_rb_=1, num_rbs_=3 : RBs used are 1,3,5
	bool8_t sym_inc_;				//	Symbol number increment command
	uint32_t start_rb_;				//	Index of the starting RB of data allocation description
	uint32_t num_of_rbs_;			//	Number of contiguous RBs per data allocation description

	//	Information regarding to IQ samples:
	//int32_t num_of_iq_samples_;		//	Number of IQ samples - Not needed: num_of_rbs_*12
	Complex* iq_samples_;			//	Pointer to array of IQ samples
}DATA_DESCRIPTOR;

//===================================
//======= Header PRACH(Type3) =======
//===================================
typedef struct
{
	uint32_t              time_offset_;			//	Time offset from the start of the slot to the start of the Cyclic Prefix (CP) in number of samples Ts
	FrameStructureFFTSize fft_size_;//	FFTSize = 2^fft_size_
	FrameStructreMu       mu_;			//	mu parameter defines the SubCarrier spacing (numerology)
	uint32_t              cp_length_;			//	CP length in number of samples Ts
	uint32_t              ud_comp_header_;		//	User-Data Compression Header
}HEADER_ADDITIONAL_DATA;


class ISplitAPI
{
public:
	virtual EResultT Classify(UINT8* packet_ptr, MESSAGE_DESCRIPTOR* message_descriptor, HEADER_ADDITIONAL_DATA* header_additional_data = nullptr) = 0;	//	DU + RU use
	virtual EResultT SendMessage(const MESSAGE_DESCRIPTOR* message_descriptor, UINT8** packet_ptr, uint32_t* buffer_byte_size) = 0;	//	DU use
	virtual EResultT ReleaseMessage(const MESSAGE_DESCRIPTOR* message_descriptor) = 0;

	virtual EResultT InitBuffer_Read(UINT8* ptr_to_assign,
			const MESSAGE_DESCRIPTOR* message_descriptor,
			const HEADER_ADDITIONAL_DATA* header_additional_data = nullptr) = 0;	//	DU + RU use
	virtual EResultT InitBuffer_Write(UINT8* ptr_to_assign,
			                          const  MESSAGE_DESCRIPTOR* message_descriptor,
			                          const  HEADER_ADDITIONAL_DATA* header_additional_data = nullptr) = 0;	//	DU + Tests use

	virtual EResultT AddAllocation(const MESSAGE_DESCRIPTOR* message_descriptor,
			                       const ALLOCATION* alloc_to_add) = 0;	//	DU + Tests use
	virtual EResultT GetAllocation(MESSAGE_DESCRIPTOR* message_descriptor, ALLOCATION* alloc_to_get, ParserResult* parser_result) = 0;	//	RU use

	virtual EResultT AddAllocation_PRACH(const MESSAGE_DESCRIPTOR* message_descriptor,
			                             const HEADER_ADDITIONAL_DATA* header_additional_data,
			                             const ALLOCATION_PRACH* alloc_to_add) = 0;	//	DU + Tests use

	virtual EResultT GetAllocation_PRACH(MESSAGE_DESCRIPTOR* message_descriptor, ALLOCATION_PRACH* alloc_to_get, ParserResult* parser_result) = 0;	//	RU use
	virtual EResultT GetPrachHeaderAdditionalData(const MESSAGE_DESCRIPTOR* message_descriptor, HEADER_ADDITIONAL_DATA* header_additional_data) = 0;	//	RU use

	virtual EResultT AddAllocation_GUARD(const MESSAGE_DESCRIPTOR* message_descriptor, const ALLOCATION_GUARD* alloc_to_add) = 0;	//	DU + Tests use
	virtual EResultT GetAllocation_GUARD(MESSAGE_DESCRIPTOR* message_descriptor, ALLOCATION_GUARD* alloc_to_get, ParserResult* parser_result) = 0;	//RU use

	virtual EResultT AddData(const MESSAGE_DESCRIPTOR* message_descriptor, const DATA_DESCRIPTOR* data_to_add) = 0;	//	DU + Tests use
	virtual EResultT GetData(MESSAGE_DESCRIPTOR* message_descriptor, DATA_DESCRIPTOR* data_to_get, ParserResult* parser_result) = 0;	//	RU use

	virtual EResultT AddData_PRACH(const MESSAGE_DESCRIPTOR* message_descriptor, const DATA_DESCRIPTOR* data_to_add) = 0;	//	DU + Tests use
	virtual EResultT GetData_PRACH(MESSAGE_DESCRIPTOR* message_descriptor, DATA_DESCRIPTOR* data_to_get, ParserResult* parser_result) = 0;	// RU use

	virtual EResultT PacketsAnalyzing(DataDirection direction,OranMessagePlaneType plane_type, int32_t *missing_packages, uint32_t symbol_number = 0) = 0;	//	DU + RU use

	virtual EResultT GetPacketHeaders(const MESSAGE_DESCRIPTOR* message_descriptor,	UINT8* packet_start_ptr, uint32_t* header_byte_size, uint32_t buffer_max_size) = 0;	// RU use
	virtual EResultT GetUPlaneSectionHeader(const ALLOCATION* allocation, UINT8* packet_ptr, uint32_t* header_byte_size, uint32_t buffer_max_size) = 0;	//	RU use

	virtual int32_t GetUPlaneSectionHeaderSize() = 0;
	virtual EResultT GetUPlaneSectionSize(const int32_t num_of_rbs, int32_t* section_byte_size) = 0;
	virtual EResultT GetUPlaneSectionSizeWithoutHeader(const int32_t num_of_rbs, int32_t* section_byte_size) = 0;
	virtual EResultT GetCPlaneSectionSize(const ALLOCATION* allocation, int32_t* section_byte_size) = 0;
	virtual EResultT GetCPlaneSectionSize_PRACH(const ALLOCATION_PRACH* allocation, int32_t* section_byte_size) = 0;
	virtual EResultT GetPacketCurrentSize(const MESSAGE_DESCRIPTOR* message_descriptor, int32_t* packet_byte_size) = 0;	//	RU use

	virtual void UpdatePayloadSize(UINT8* packet_start_ptr, uint32_t new_payload_size) = 0;	//	DU + RU use
};


#endif	//	API_MODULE_API_I_SPLIT_API_H__
