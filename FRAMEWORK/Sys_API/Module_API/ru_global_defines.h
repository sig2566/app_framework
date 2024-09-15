/******************************************************************
 * ru_global_defines.h
 * Author:
 * Created: June 2019
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef RU_GLOBAL_DEFINES_INC_
#define RU_GLOBAL_DEFINES_INC_
#include 	"global_defines.inc"

 #define MIN_CONST(a,b) ((a) < (b) ? (a) : (b))



/*********************Configuration definitions*************************************/ 
#ifndef _RU_OFFLINE_
#define		RU_RT_MODE	  //RT Mode - temporary until we will remove all non-RT dependencies - should be used for hybrid mode etc. (all modes that are not off-line)
#endif

#define PCAP_HEADER_MODE //in this mode we use pcap_header instead of off-line data header

/*********************Printing & Comparing definitions****************************/
//#define		PRINT_MEMPOOL_LOG	//	This flag enables printfs of mempool creation logs
 
/*********************Full Bypass & Run-length definitions**************************/
//#define 	FULL_BYPASS_MODE_DL
//#define 	FULL_BYPASS_MODE_UL
// Only one of the following #defines might be opened, i.e. we can use only one method of run-length usage.
// If both are #defined - the system's behavior will be unpredictable !!!
// Also pay attention that their definition is relevant only if we work in full BP mode (at the moment we support run-length only in this mode)
//#define		RUN_LENGTH_ON_CPLANE
//#define		RUN_LENGTH_ON_UPLANE

/*********************Profiling definitions******************/
// DL profiling - can't be open both
#define PROFILING_TEST
//#define STEP_BY_STEP_PROFILING_TEST

//#define COMPARE_ADDER_OUTPUT
#define MTU_CPU_FPGA_MIN_SIZE	800
#define MTU_CPU_FPGA_MAX_SIZE	3000

//How much time the RU application will be synchronized before the first oran packet (in slot resolution)
#define NUMBER_SLOTS_START_TIME_RU_APPLICATION_BEFORE_FIRST_ORAN_PACKET 2

#ifdef RU_RT_MODE
	const int32_t  SYMBOLS_OFFEST_USEC = -30 ;   // Note - Need to set to 0 in offline mode - neg values not supported
	const int32_t  SLOT_OFFEST_USEC    = -20 ;   // Note - Need to set to 0 in offline mode - neg values not supported
#else
	const int32_t  SYMBOLS_OFFEST_USEC = 0   ;   // Note - Need to set to 0 in offline mode - neg values not supported
	const int32_t  SLOT_OFFEST_USEC    = 0   ;   // Note - Need to set to 0 in offline mode - neg values not supported
#endif



typedef enum       
{
    TIMING_MODE_TIMER,                          // run according timers (in this mode the RSE_test is not running)
    TIMING_MODE_RSE_TEST,                        // run according GL Test (in this mode the RSE_test and timers are running but operation is set according RSE_Test)
}TIMER_RSE_TEST_MODE_T;
#define 	TIMING_MODE					TIMING_MODE_RSE_TEST
														
#define		SYMBOL_TIMER_OFFSET					0
#define		BF_START_TIMER_OFFSET				0.66
#define		UPLAN_START_TIMER_OFFSET			0.66

typedef enum       
{
    e_CALL_TIMER_FROM_RSE_TEST,
    e_CALL_TIMER_FROM_TIMERS
}CALL_TIMER_T;

enum TimeEvent
{
	tSLOT = 0,
	tSYMB = 1
};
#define     CALL_TIMER                          e_CALL_TIMER_FROM_RSE_TEST


//	This enum describes the data engine type: PreEqualizer \ Beamforming
enum DataEngineType : unsigned char
{
	kBeamformingDataEngine = 0,
	kPreEqualizerDataEngine
};

//#define Architecture_A
// ==============================================================================================================================
//-----------------------------------------------------------------
//#define 	MAX_DL_DATA_STREAMS_PER_RB			(NUM_OF_RU_ANTENNAS)                    // 32 stands for the possibility of TX Antennas streaming (e.g. for CSI-RS) instead of 8 Layers that we have to support in DL (e.g. for PDSCH)
///////// #define 	MAX_DL_DATA_STREAMS_PER_RB		    (RU_MAX_NUM_OF_LAYERS)                  // 8 layers will be used for BF multiplication, otherwise we will work with Antennas and will make a Bypass (with 32 Rx Ant) without multiplication
//-----------------------------------------------------------------
#define     MAX_SECTIONS_IN_CPLANE_PACKET        (255)
#define     MAX_SECTIONS_IN_UPLANE_PACKET        (MAX_NUM_OF_RBS_BH)

#define 	MAX_DL_DATA_STREAMS_PER_RB	         (NUM_OF_RU_ANTENNAS)                    // 32 stands for the possibility of TX Antennas streaming (e.g. for CSI-RS) instead of 8 Layers that we have to support in DL (e.g. for PDSCH)
//#define 	MAX_DL_DATA_STREAMS_PER_RB           (RU_MAX_NUM_OF_LAYERS)                  // 8 layers will be used for BF multiplication, otherwise we will work with Antennas and will make a Bypass (with 32 Rx Ant) without multiplication
#define 	MAX_UL_DATA_STREAMS_PER_RB	         (NUM_OF_RU_ANTENNAS)  				     // 32 stands for the possibility of RX Antennas streaming (e.g. for SRS) instead of 4 Layers that we have to support in UL (e.g. for PUSCH)
#define		ANT_GROUPS_NUMBER			         (MIN_CONST(4,NUM_OF_RU_ANTENNAS))	     // Number of antennas groups - min between 4 and NUM_OF_RU_ANTENNAS - can't be more than antennas - in order NUM_ANTENNAS_PER_ANT_GROUP >=1
#define 	BF_ENGINES_NUMBER			         (ANT_GROUPS_NUMBER)    					// Number of  BF engines in FE (as a number of ANT GROUPS)
#define 	PE_ENGINES_NUMBER			         (ANT_GROUPS_NUMBER)  					 // Number of  PreEqualizer engines in FE (as a number of ANT GROUPS)

#define     NUM_ANTENNAS_PER_ANT_GROUP	         (NUM_OF_RU_ANTENNAS/ANT_GROUPS_NUMBER)	 // Number of antennas in one group (engines)

#define RU_MAX_THEORETICAL_NUM_LAYERS 			(8)

#define 	NUM_OF_SLOT_RU_ANTENNAS_SAMPLES      (NUM_OF_RU_ANTENNAS * MAX_NUM_SUB_CURRIERS_FH* MAX_NUM_OFDM_PER_SLOT)
#define 	NUM_OF_SLOT_RU_ANTENNA_GROUP_SAMPLES (NUM_ANTENNAS_PER_ANT_GROUP * MAX_NUM_SUB_CURRIERS_FH)
#define	    MEAN_RU_SNR_THRESHOLD                (20.0)
#define	    MIN_RU_SNR_THRESHOLD                 (15.0)
#define 	NUM_OF_SLOT_RU_ANTENNAS_SAMPLES_BH      (NUM_OF_RU_ANTENNAS * MAX_NUM_SUB_CURRIERS_BH* MAX_NUM_OFDM_PER_SLOT)



#define     STITCHER_INPUT_SIZE                  (MAX_NUM_SUB_CURRIERS_BH * sizeof(Complex) + MAX_SECTIONS_IN_UPLANE_PACKET * U_PLANE_SECTION_HEADER_MAX_SIZE)

#define HOST_ADDRESS 0x11
#define SOURCE_ADDRESS 0x33

#endif /* RU_GLOBAL_DEFINES_INC_ */

