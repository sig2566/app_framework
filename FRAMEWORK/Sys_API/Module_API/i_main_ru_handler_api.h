/*
 * i_main_ru_handler_api.h
 *
 *  Created on: May 13, 2019
 *      Author: E. Maidman
 */

#ifndef API_MODULE_API_I_MAIN_RU_HANDLER_API_H_
#define API_MODULE_API_I_MAIN_RU_HANDLER_API_H_

#include "i_split_api.h"
#include "ru_global_defines.h"




enum TIME_PARAMETERS
{
	SYMBOL_DL_PARAM	= 12,
	SYMBOL_UL_PARAM	= 13,
	SLOT_DL_PARAM	= 15,
	SLOT_UL_PARAM	= 16
};

enum TimeEventType
{
	SymbolTickDLEvent,
	SymbolTickULEvent,
	SlotTickDLEvent,
	SlotTickULEvent
};

// Name of input FIFO for all GL test-> dummy DPDK driver API (input fifo Ques from Oran packet to DPDK)
#define RU_ORAN_UPLANE_DL_LAYER_0_QUEUE   "RU_UPLANE_DL_LAYER_0_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_1_QUEUE   "RU_UPLANE_DL_LAYER_1_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_2_QUEUE   "RU_UPLANE_DL_LAYER_2_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_3_QUEUE   "RU_UPLANE_DL_LAYER_3_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_4_QUEUE   "RU_UPLANE_DL_LAYER_4_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_5_QUEUE   "RU_UPLANE_DL_LAYER_5_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_6_QUEUE   "RU_UPLANE_DL_LAYER_6_QUEUE_TO_DPDK"
#define RU_ORAN_UPLANE_DL_LAYER_7_QUEUE   "RU_UPLANE_DL_LAYER_7_QUEUE_TO_DPDK"

#define RU_ORAN_CPLANE_LAYER_0_QUEUE   "RU_CPLANE_LAYER_0_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_1_QUEUE   "RU_CPLANE_LAYER_1_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_2_QUEUE   "RU_CPLANE_LAYER_2_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_3_QUEUE   "RU_CPLANE_LAYER_3_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_4_QUEUE   "RU_CPLANE_LAYER_4_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_5_QUEUE   "RU_CPLANE_LAYER_5_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_6_QUEUE   "RU_CPLANE_LAYER_6_QUEUE_TO_DPDK"
#define RU_ORAN_CPLANE_LAYER_7_QUEUE   "RU_CPLANE_LAYER_7_QUEUE_TO_DPDK"

#define STITCHER_INPUT_MEM_LAYER_0      "STITCHER_INPUT_MEM_LAYER_0"
#define STITCHER_INPUT_MEM_LAYER_1      "STITCHER_INPUT_MEM_LAYER_1"
#define STITCHER_INPUT_MEM_LAYER_2      "STITCHER_INPUT_MEM_LAYER_2"
#define STITCHER_INPUT_MEM_LAYER_3      "STITCHER_INPUT_MEM_LAYER_3"


#define RU_DATA_ENGINE_FIFO_IN_0   "RU_DATA_ENGINE_FIFO_IN_JOB_DESCS_0"


#define DPDK_OFFLINE_DRIVER_EGRESS_FIFO	"DPDK_OFFLINE_DRIVER_EGRESS_FIFO"
#define WORKER_THREAD



#define TOTAL_NUMBER_MEMPOOLS			(7) //for U-Plane (indirect), C-Plane (JDs), Ethernet Headers
// cache_size:
// If cache_size is non-zero, the rte_mempool library will try to limit the accesses to the common lockless pool,
// by maintaining a per-lcore object cache.
// This argument MUST be lower or equal to CONFIG_RTE_MEMPOOL_CACHE_MAX_SIZE and n / 1.5 (n - number of buffers in the pool).
// It is advised to choose cache_size to have "n modulo cache_size == 0": if this is not the case,
// some elements will always stay in the pool and will never be used.
// The access to the per-lcore table is of course faster than the multi-producer/consumer pool.
// The cache can be disabled if the cache_size argument is set to 0; it can be useful to avoid losing objects in cache.
#ifdef RU_RT_MODE
#define	MEMPOOL_CACHE_SIZE				(0/*256*/)
#else
#define	MEMPOOL_CACHE_SIZE				(0)
#endif
//----------------------------------------------------------------------------------------------------------------------------------
#define	MEMPOOL_SOCKET_ID				(0) // we have single socket system at the moment
#define POOL_OPS_NAME					"stack" // - for using stack memory - cannot be used for DPAA2 sending !!!!


#define	EXTRA_BUFFERS_BLOCK             (1)    // Extra Cplane buffers for edge case (in the end of every test) , Needed only in Offline case (non-RT)
#define	EXTRA_DATA_SIZE                 (1000) //in the mbuf we have buff offset - this param represent the extra data needed in the mbuf (to the needed data )

#define	Cplane_implementation_num       (2)	// ME - what is this ???
#define	NUM_OF_TESTS                    (18)

#define	UPLANE_Data_SIZE		        (NUMBER_SC_IN_RB)*(sizeof(Complex))*(MAX_NUM_OF_RBS_BH)
#define MAX_NUM_OF_MTUs_UPLANE 		    ((uint16_t)(UPLANE_Data_SIZE / MTU_CPU_FPGA_MIN_SIZE) + (uint8_t)((UPLANE_Data_SIZE % MTU_CPU_FPGA_MIN_SIZE) > 0))


#define ETHERNET_HEADER_UPLANE_DL_MAX_SIZE (DBL_BUF*(RU_MAX_THEORETICAL_NUM_LAYERS)*MAX_NUM_OF_MTUs_UPLANE* (1 + EXTRA_BUFFERS_BLOCK))
#define UPLANE_DL_SIZE                     (DBL_BUF*(RU_MAX_THEORETICAL_NUM_LAYERS)*MAX_NUM_SUB_CURRIERS_BH* (1 + EXTRA_BUFFERS_BLOCK))
//==============================================================================================================================


// for debug
struct RuWaveFormStructIO
{
    uint32_t    Mu;                //5G numerology
    uint32_t    num_rb;            //Carrier BW (number of active RBs
    uint32_t    nbwp_start;        //Carrier BW part starting point. Ncrb = Nprb + NbwpStart
    uint32_t    SFI;           //slot format
    uint32_t    CP;            //cyclic prefix type: NORMAL,EXTENDED,NO
    uint32_t    enable_window; //Enable shaping window
    float       alpha;         //Alpha factor for RC window (Raised-Cosine)
};


/*
struct OutputDlSymbolDesc
{
	uint32_t	antenna_group_number_;
	Complex*	symbol_data_ptr_;
};
*/

// ============================ DL and UL Definitions =================================
struct  BF_weight_mem_typ       // for each RB specify the beam_weight and the RE mask
{
    Complex    bf_weights;             // the beam_weight
};


struct  BF_weight_map_typ       // for each RB specify the beam_id and the RE mask
{
	uint16_t    bypass_flag;         // flag indicate whan bypass valid => i.e layer_count>8
    uint16_t    beam_id;             // the beam id
    uint16_t    re_mask;
    uint16_t	allocation_id;		 // allocation index may be used for verification with Uplan info
    uint16_t	rb_runlength;		 // consequent amount of RBs on current allocation (for RT optimization of BF/PE
    uint16_t	save_header_place;	 // amount of bytes the PE need to save in its output buffer before this RB for section header
};
// ============================ DL Definitions =================================

#ifdef _RU_
struct SingleRbDataPointersAndInfo // Job Descriptor for BF DE
{
	Complex*	rb_data_ptr_; //need to remove in the future - after we will work only with mbuf
	struct rte_mbuf* pkt_data;
	uint16_t	run_length_; //RBs run-length number for applying to sequentially from this RB and ahead
	//uint8_t		bypass_flag_;
};
// The C-Plane JD for DL direction
struct  BF_Jd_Map
{
    uint16_t              re_mask1;
    uint16_t    	      beam_id1;                // Beam identifier
    uint16_t    	      re_mask2;
    uint16_t    	      beam_id2;                // Beam identifier
    UINT8                 flag_use_previous_beam:1;       // Flag which indicate whatever the FPGA will use the current beam ID, or that of the previous RB
    UINT8                 reserved:7;

};
#else
struct SingleRbDataPointersAndInfo // Job Descriptor for sorting DE
{
	//Complex*	rb_data_ptr_per_stream_[MAX_NUM_OF_RX_ANTENNAS];
	Complex*	rb_data_ptr_per_stream_[MAX_DATA_STREAMS_PER_RB_DU];
	uint16_t	run_length_; //RBs run-length number for applying to sequentially from this RB and ahead
	//uint8_t		bypass_flag_;
};
#endif


struct SymbolRbPointers
{
	SingleRbDataPointersAndInfo single_rb_data_pointers[MAX_DL_DATA_STREAMS_PER_RB][MAX_NUM_OF_RBS_BH];
};


// ============================ UL Definitions =================================

#define AGGREGATE_MODE	1	//	1 - for aggregation of sections per packet
								//	0 - for one section per packet

enum ERbAllocationT // RB Allocation type for Stitcher Engine (U-Plane build)
{
	E_NON_ALLOCATED = 0,	//	E_NON_ALLOCATED value must be 0 because when resetting pre_equalizer_allocation_info_map_
							//	we use memset to 0
	E_FIRST_ON_PACKET,
	E_FIRST_ON_SECTION,
	E_ALLOCATED,         	// any other than First-on-packet or First-on-section
	E_LAST_RB_ALLOCATION_TYPE
};

struct  Section_Header_struct
{
	uint8_t  section_header_data[U_PLANE_SECTION_HEADER_MAX_SIZE];
	uint32_t section_header_len;
};
// The C-Plane JD for UL direction
struct  PE_Jd_Map
{
    uint16_t    		re_mask1;
    uint16_t    		beam_id1;                // Beam identifier
    uint16_t    		re_mask2;
	uint16_t    		beam_id2;                // Beam identifier
	UINT8               flag_use_previous_beam:1;       // Flag which indicate whatever the FPGA will use the current beam ID, or that of the previous RB
	UINT8        		flag_save_header_place:1;	 // Flag which indicate if the FPGA needs to insert section header in PE output or not
	UINT8        		flag_header_long_length:1;	 	 // Flag which indicate the size in bytes of the section header
	UINT8               reserved:5;
	uint8_t             section_header[U_PLANE_SECTION_HEADER_SIZE_WITHOUTCOMP];

};

struct  PreEqualizer_Stitcher_RB_Info       // for each RB specify RB-type and U-Plane headers
{
	ERbAllocationT	rb_allocation_type_;
    uint16_t    	re_mask;
    uint16_t    	num_of_rbs;
    uint8_t         packet_header[ECPRI_RADIO_APP_HEADER_MAX_SIZE];
};


// Name of input FIFO for Handler
#define RU_HANDLER_FIFO_IN	"RU_HANDLER_FIFO_IN"
#define RU_HANDLER_MODULE	"MAIN_RU_HANDLER"
/******************************************************************************************//*!
 *@class RU_Handler_API
 *@brief The purpose of this class is :
 *@brief This class expose the RU Glue Logic control API.
  *********************************************************************************************/
struct RU_Handler_API
{
	virtual EResultT	InputPacketPlaneClassifier(uint8_t* input_packet, uint32_t input_packet_size) = 0;
#ifdef WORKER_THREAD
	virtual EResultT 	RunUPlaneDL() = 0;
	virtual EResultT	UPlaneDLProcess() = 0;
	virtual EResultT 	RunCPlane() = 0;
	virtual EResultT	CPlaneProcess() = 0;
	virtual EResultT 	RunTimeEvents() = 0;
	virtual EResultT	IHotStart() = 0;
#endif
	virtual EResultT	DU_Control_Message_Handler_Dl(MESSAGE_DESCRIPTOR* du_message_desc, ALLOCATION* du_message_alloc) = 0;
	virtual EResultT	DU_Userplane_Message_Handler_Dl(MESSAGE_DESCRIPTOR* du_message_desc, DATA_DESCRIPTOR* du_message_data) = 0;
	virtual EResultT    DU_Control_Message_Handler_Ul(MESSAGE_DESCRIPTOR* du_message_desc, ALLOCATION* du_message_alloc) = 0;
	virtual EResultT	Userplane_receive_message_Handler_DL(uint8_t* input_packet, MESSAGE_DESCRIPTOR* msg_classifier) = 0;
	virtual EResultT	Userplane_receive_message_Handler_UL(uint8_t* input_packet, MESSAGE_DESCRIPTOR* msg_classifier,DATA_DESCRIPTOR* current_data_message) = 0;
	virtual EResultT    InputPacketRecive(OranMessagePlaneType massageType)=0;

	virtual void		SetCurrentSlot(SysTimeT sys_time, DataDirection direction) = 0;
	virtual void		SetCurrentSymbol(uint32_t symbol_number, DataDirection direction) = 0;
	virtual void		SlotTick(DataDirection direction) = 0;
	virtual void		SymbolTick(DataDirection direction) = 0;
    virtual void		SymbolWindowOpenTick(DataDirection direction) = 0;
    virtual void		SymbolWindowCloseTick(DataDirection direction) = 0;
    virtual EResultT	PacketsAnalyzing(DataDirection direction,OranMessagePlaneType plane_type, int32_t *missing_packages) = 0;
    virtual EResultT	checkAndGetUplinkUplaneMessage(uint8_t** uplane_msg_ptr) = 0;
#ifndef RU_RT_MODE
	virtual Complex*	getOutputSymbolDataFromAntennaGroup(uint32_t ant_group_number) = 0;
#endif
	//virtual Complex*	getInputSymbolDataFromAntennaGroup(uint32_t ant_group_number) = 0;
#ifdef COMPARE_ADDER_OUTPUT
	virtual uint8_t*	getOutputFromAdder() = 0;
#endif
	virtual uint32_t 	getDlBfSymbolNumber() = 0;
	virtual EResultT	getDlMessageCount(uint32_t* missed_ctrl_mes, uint32_t* rec_ctrl_mes, uint32_t* missed_dat_mes, uint32_t* rec_dat_mes) = 0;
	virtual EResultT    printMissedMessageCount() = 0;

    //GLConfig configures the gNodeB Glue Logic. It is called at the system start or after GLStop.
    virtual EResultT	GLConfig(RuWaveFormStructIO *config_data) = 0;
    virtual EResultT	GLStop() = 0;

    //connect to ru_time_events_handler
    //virtual EResultT    GetConnectToTimeEventsHandler(DataDirection index_ul_dl, TimeEvent index_slot_symb)=0;

  //DPDK  api connect
    virtual void* getUPlaneMempool() = 0;
    virtual void* getUPlaneDlMempool() = 0;
    virtual void* getEthHeadersMempool() = 0;
    virtual void* getZerosMempool() = 0;
    virtual void* getCurrentMbufPointer() = 0;
    virtual void* getUplaneEthHeadersMempool()=0;
    virtual void* GetMbufAndLayerNumberForStitcher(uint32_t* layer_count)=0;

};


// Memories definitions

#define INPUT_USER_PLANE_PACKETS_MEM	"INPUT_USER_PLANE_PACKETS_MEMORY"
#define RU_TASKS_QUEUE_MEM	"RU_TASKS_QUEUE_MEMORY"


#endif /* API_MODULE_API_I_MAIN_RU_HANDLER_API_H_ */
