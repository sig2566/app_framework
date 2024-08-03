/*
 * i_ant_group_pool_api.h
 *
 *  Created on: May 29, 2019
 *      Author: E. Maidman
 */

#ifndef API_MODULE_API_I_ANT_GROUP_POOL_API_H_
#define API_MODULE_API_I_ANT_GROUP_POOL_API_H_

#include "i_main_ru_handler_api.h"
#include "ru_global_defines.h"

#ifdef RU_RT_MODE
#define MTU_CPU_FPGA_SIZE		1300 //25000 //1300
#else
#define MTU_CPU_FPGA_SIZE		1300
#endif

// Module name
#define ANT_GROUP_POOL_MODULE	"ANT_GROUP_POOL"

// Name of input FIFO for all Data Engines JobDescriptors
#define RU_DATA_ENGINE_FIFO_IN_0   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_0"
#define RU_DATA_ENGINE_FIFO_IN_1   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_1"
#define RU_DATA_ENGINE_FIFO_IN_2   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_2"
#define RU_DATA_ENGINE_FIFO_IN_3   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_3"
#define RU_DATA_ENGINE_FIFO_IN_4   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_4"
#define RU_DATA_ENGINE_FIFO_IN_5   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_5"
#define RU_DATA_ENGINE_FIFO_IN_6   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_6"
#define RU_DATA_ENGINE_FIFO_IN_7   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_7"

#define RU_UPLANE_DL_LAYER_0   "RU_UPLANE_DL_LAYER_0"
#define RU_UPLANE_DL_LAYER_1   "RU_UPLANE_DL_LAYER_1"
#define RU_UPLANE_DL_LAYER_2   "RU_UPLANE_DL_LAYER_2"
#define RU_UPLANE_DL_LAYER_3   "RU_UPLANE_DL_LAYER_3"
#define RU_UPLANE_DL_LAYER_4   "RU_UPLANE_DL_LAYER_4"
#define RU_UPLANE_DL_LAYER_5   "RU_UPLANE_DL_LAYER_5"
#define RU_UPLANE_DL_LAYER_6   "RU_UPLANE_DL_LAYER_6"
#define RU_UPLANE_DL_LAYER_7   "RU_UPLANE_DL_LAYER_7"

#define RU_CPLANE_DL_LAYER_0   "RU_CPLANE_DL_LAYER_0"
#define RU_CPLANE_DL_LAYER_1   "RU_CPLANE_DL_LAYER_1"
#define RU_CPLANE_DL_LAYER_2   "RU_CPLANE_DL_LAYER_2"
#define RU_CPLANE_DL_LAYER_3   "RU_CPLANE_DL_LAYER_3"
#define RU_CPLANE_DL_LAYER_4   "RU_CPLANE_DL_LAYER_4"
#define RU_CPLANE_DL_LAYER_5   "RU_CPLANE_DL_LAYER_5"
#define RU_CPLANE_DL_LAYER_6   "RU_CPLANE_DL_LAYER_6"
#define RU_CPLANE_DL_LAYER_7   "RU_CPLANE_DL_LAYER_7"

#define RU_CPLANE_UL_LAYER_0   "RU_CPLANE_UL_LAYER_0"
#define RU_CPLANE_UL_LAYER_1   "RU_CPLANE_UL_LAYER_1"
#define RU_CPLANE_UL_LAYER_2   "RU_CPLANE_UL_LAYER_2"
#define RU_CPLANE_UL_LAYER_3   "RU_CPLANE_UL_LAYER_3"
#define RU_CPLANE_UL_LAYER_4   "RU_CPLANE_UL_LAYER_4"
#define RU_CPLANE_UL_LAYER_5   "RU_CPLANE_UL_LAYER_5"
#define RU_CPLANE_UL_LAYER_6   "RU_CPLANE_UL_LAYER_6"
#define RU_CPLANE_UL_LAYER_7   "RU_CPLANE_UL_LAYER_7"

#define RU_WEIGHTS             "RU_WEIGHTS"

#define ADDER_RESULTS_MEM_LAYER_PCAP      "ADDER_RESULTS_MEM_LAYER_PCAP"

// Name of fifo between FE FPGA (Antenna I/Q samples) to BF FPGA
#define RU_FE_TO_PE_FPAG_QUEUE "RU_FE_UL_FIFO"
// Name of fifo between BF FPGA to FE FPGA (Antenna I/Q samples)
#define RU_BF_TO_FE_FPAG_QUEUE "RU_FE_DL_FIFO"

#define RU_EMULATOR_IDATA		"RU_EMULATOR_FIFO"

//	Name of FIFO between BF FPGA and DPDK Ethernet Offline Driver (Egress):
#define DPDK_OFFLINE_DRIVER_EGRESS_FIFO_FRONT	"DPDK_OFFLINE_DRIVER_EGRESS_FIFO_FRONT"

#define DEFAULT_RBS_PER_CHUNK	(MAX_NUM_OF_RBS_BH/5)		//	Number of RBs per chunk of a sync flag vector.



//	Must be a divider of MAX_NUM_OF_RBS
#define	MAX_SYNC_FLAG_CHUNKS	MAX_NUM_OF_RBS_BH		// Maximal number of chunks that divides the sync flag vector (maximum can be a single RB per chunk)


/******************************************************************************************//*!
 *@class Ant_Group_Pool_API
 *@brief The purpose of this class is :
 *@brief This class expose the Antenna Groups Pool Module control API.
  *********************************************************************************************/
struct Ant_Group_Pool_API
{
	virtual EResultT      getRBmapMemPtrFromAntGroup_DL  (uint32_t ant_group_id, int8_t double_buf_flag, int8_t ** rb_map_symb_ptr)  = 0; // for DL Path memory buffers
	virtual EResultT      getRBmapMemPtrFromAntGroup_UL  (uint32_t ant_group_id, int8_t double_buf_flag, int8_t ** rb_map_symb_ptr)  = 0; // for UL Path memory buffers
	virtual EResultT      getBfWeightIndexPtrFromAntGroup(uint32_t ant_group_id, uint16_t bf_index     , int8_t ** bf_weight_index_ptr) = 0;
	virtual Complex*      getOutputSymbolDataPointerFromAntGroup(uint32_t ant_group_id) = 0;
	virtual Complex*      getInputSymbolDataPointerFromAntGroup(uint32_t ant_group_id)  = 0;
#ifdef COMPARE_ADDER_OUTPUT
	virtual uint8_t*   	  getOutputPointerFromAdder()								    = 0;
#endif
	virtual Complex32fix* GetAdderInputRegistersPtr(uint32_t antenna_group_number)      = 0;
	virtual uint8_t*      GetAdderSyncFlagPtr(uint32_t antenna_group_number)            = 0;
	virtual void		  ResetAntGroupStateMachineDL() = 0;
	virtual void		  ResetAntGroupStateMachineUL() = 0;
};


//	This JD describes parameters for the Beamformer's multiplier
struct RUbfEngineJobDesc
{
	DataEngineType data_engine_type_;
	uint8_t					bf_weights_indices_table_index_;  // proper half-buffer index of the m_BF_weight_symb_fpga double-buffer
	SymbolRbPointers*		symbol_rbs_data_ptrs_;     //include bypass mode in order to pass samples without mult with c-plane
};


//	This JD describes parameters for the PreEqualizer's multiplier
struct RUPeEngineJobDesc
{
	DataEngineType          data_engine_type_;
	uint8_t					bf_weights_indices_table_index_;// proper half-buffer index of the m_BF_weight_symb_fpga double-buffer
};


//	This structure describes the JD FIFO item.
//	First field defines the type of the item's data engine: BF/PE
//	Next field is a union of BF JD and PE JD.
struct DataEngineFifoItem
{
	DataEngineType data_engine_item_type_;
	union
	{
		RUbfEngineJobDesc bf_job_desc_;
		RUPeEngineJobDesc pe_job_desc_;
	};
};


struct RUbfConfigureMsg
{
	uint8_t num_layers;
};

struct AdderInputPerAntGroup
{
	uint8_t     flag_Synchronization_vector_[MAX_SYNC_FLAG_CHUNKS];                     // Indicate if the calculation specific RB is finished
	uint32_t	flag_sync_vector_size;													//	Actual size of flag_Synchronization_vector_
																						//	Meaning: the number of RBs chunks
																						//	NOTE: all DEs must have the same value of flag_sync_vector_size
	Complex32fix	stream_samples_[MAX_UL_DATA_STREAMS_PER_RB][MAX_NUM_SUB_CURRIERS_BH];	// PE Engine calculation results, adder input for single antenna pool.
};

enum DataDirectionStateMachine
{
	StateNoDirection = 0,
	StateUL = 1,
	StateDL = 2
};

enum PlaneTypeStateMachine
{
	StateUPlane = 0x00,		//	User Plane
	StateCPlane = 0x01,		//	Control Plane
};

struct RUAntGroupStateMachine
{
	uint32_t current_RB_dl;
	uint32_t current_RB_ul;
	uint32_t current_layer_Cplane_dl;
	uint32_t current_layer_Cplane_ul;
	uint32_t current_layer_Uplane;
};

struct  CURRENT_BEAM
{
   uint16_t    		re_mask;
   uint16_t    		beam_id;                // Beam identifier
};



typedef	uint32_t ru_emu_msg_len_type;




#endif /* API_MODULE_API_I_ANT_GROUP_POOL_API_H_ */
