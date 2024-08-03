/*
 * i_handler_api.h
 *
 *  Created on: June 11, 2018
 *      Author: D. Matas
 */

#ifndef API_MODULE_API_I_HANDLER_API_H_
#define API_MODULE_API_I_HANDLER_API_H_


//for uplink for GL test only!!!
#define UL_SYMBOL_DATA_OUT    "UL_SYMBOL_DATA_OUT"
//#define COMPARE_WITH_DMRS //if defined - compare of equalized data is with dmrs symbols ,if not defined - compare only data
#define LLR_DATA_OUT    	  "LLR_DATA_OUT"
#define TB_DATA_OUT    		  "TB_DATA_OUT"
#define MAX_ESTIMATION_DATA_SLOT_IN_SIZE   (MAX_NUM_OF_RBS_FH * NUMBER_SC_IN_RB * MAX_NUM_OFDM_PER_SLOT * MAX_NUM_OF_RX_ANTENNAS)
#define MAX_ESTIMATION_DATA_SLOT_OUT_SIZE   (MAX_NUM_OF_RBS_FH * NUMBER_SC_IN_RB * MAX_NUM_OFDM_PER_SLOT * MAX_NUM_OF_LAYERS)
#define MAX_LLR_DATA_SLOT_OUT_SIZE   (MAX_NUM_OF_RBS_FH * NUMBER_SC_IN_RB * MAX_NUM_OFDM_PER_SLOT * MAX_NUM_OF_LAYERS * MAX_QM)

// DU framework mode
#define _NO_ORAN_
//#define COMPARE_WITH_DMRS
struct Equalized_Out_Data // per TB
{
	uint32_t  num_of_tbs;   //only for python display
    uint32_t  size_of_data_out;
    uint32_t  num_of_layers; //only for python display
    uint32_t  first_rb_num; //only for python display
    uint32_t  rb_size;      //only for python display
    Complex data_out[MAX_ESTIMATION_DATA_SLOT_OUT_SIZE];
};

struct llr_Out_Data // per TB
{
	uint32_t  num_of_tbs;
    uint32_t  size_of_data_out;
    int8_t  data_out[MAX_LLR_DATA_SLOT_OUT_SIZE];
};

struct DL_HandlerIO
{
    uint32_t  RV;
    uint32_t  RB_offset;
    uint32_t  RB_size;
    uint32_t  slot_number;
    uint32_t  MCS;
    uint32_t  num_of_layers;
    uint32_t  DmrsConfigType;
    uint32_t  DmrsAddPos;
    uint32_t  DmrsMappingType;
    uint32_t  DmrsTypeAPos;
    uint32_t  EnableDoubleSymbolDMRS;
    uint32_t  deme[20];
    uint32_t  TB_size; //calculated in Matlab
    uint32_t  TB_scrambled_out_size; //calculated a priori in Matlab, and will be calculated in algorithm in C
    uint8_t   *in;
};

struct UL_HandlerIO
{
    uint32_t  RV;
    uint32_t  RB_offset;
    uint32_t  RB_size;
    uint32_t  slot_number;
    uint32_t  symbol_start;
    uint32_t  symbol_length;
    uint32_t  MCS;
    uint32_t  num_of_layers;
    uint32_t  DmrsConfigType;
    uint32_t  DmrsAddPos;
    uint32_t  DmrsMappingType;
    uint32_t  DmrsTypeAPos;
    uint32_t  EnableDoubleSymbolDMRS;
    uint32_t  TB_size; //calculated in Matlab
    uint32_t  TB_for_compare; //For sorting case - sometimes other TB should be compared
 };

//for debug
struct WaveFormStructIO
{
    uint32_t    Mu;                //5G numerology
    uint32_t    num_rb;            //Carrier BW (number of active RBs
    uint32_t    nbwp_start;        //Carrier BW part starting point. Ncrb = Nprb + NbwpStart
    uint32_t    SFI;           //slot format
    uint32_t    CP;            //cyclic prefix type: NORMAL,EXTENDED,NO
    uint32_t    enable_window; //Enable shaping window
    float       alpha;         //Alpha factor for RC window (Raised-Cosine)
};

// for Slot Manager Configuration
struct SlotManagerConfigureMsg
{
	uint8_t num_rx_ant;
};
//Transport block output
struct tb_Out_Data
{
	uint32_t  size_of_data_out;
    uint8_t  data_out[MAX_TB_SIZE_IN_BYTES];
};

//enum FrameDuplex {TDD, FDD};
//enum CP {NORMAL, EXTENDED, NO};

// Name of input FIFO for Handler
#define HANDLER_FIFO_IN   "HANDLER_FIFO_IN"
#define HANDLER_MODDULE   "MAIN_5G_HANDLER"
/******************************************************************************************//*!
 *@class Handler_API
 *@brief The purpose of this class is :
 *@brief This class expose the Glue Logic control API. It is controlled by MAC commands though FAPI
 *@brief messages or by Unit/Regression tests
  *********************************************************************************************/
struct Handler_API
{
    virtual EResultT    FAPIParser(char *Cellmessage) = 0;
    //GLConfig configures the gNodeB Glue Logic. It is called at the system start or after GLStop.
    virtual EResultT    GLConfig(WaveFormStructIO *config_data, CONFIGREQUESTStruct *enodeBStruct_p ) = 0;
    virtual EResultT    GLStop() = 0;
};
#endif /* API_MODULE_API_I_HANDLER_API_H_ */
