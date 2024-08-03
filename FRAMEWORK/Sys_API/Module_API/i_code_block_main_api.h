/*
 * i_code_block_main_api.h
 *
 *  Created on: May 3, 2018
 *      Author: dmatas
 */

#ifndef API_MODULE_API_I_CODE_BLOCK_MAIN_API_H_
#define API_MODULE_API_I_CODE_BLOCK_MAIN_API_H_

#include "i_slot_builder_api.h"


//TB parameters configuration data 38.212-7.2.2


// **********************************************************************************************
// *@struct TBParameters
// *@brief The purpose of this class is :
// *@brief inner class of  TB - Hold parameters of TB - used mainly in Code-Block
// ********************************************************************************************
struct TBParameters
{
	//uint32_t	C_; //number of code-blocks
	uint32_t    tb_size;
	uint32_t	C_tag; //number of code-blocks (mostly similar to C)
	uint32_t	K;
	uint32_t	k0;
	uint32_t	K_tag;
	uint32_t	Zc;
	uint32_t	N;
	uint32_t	Kb;
	uint32_t	Ncb;
	uint32_t	i_Ls;
	uint32_t	LDPC_graph;
	uint32_t    Qm; //for Interleaving use
	//uint32_t	Er;

};


struct CodeBlockMainIO
{
	uint8_t 	*in;  //input-Pointer to TB data (TX_REQUEST) of relevant codeblock
	int32_t 	in_size; //input-number of bits in codeblock
	int32_t 	run_crc;  //if only 1 code-block - Don't run CRC (run_crc=0) !
	struct 		TBParameters	TB_par;
    uint32_t    code_block_Er; //Parameter of output length per code-block
    uint32_t    *PRBS_ptr; //Point to PRBS signal for scrambler xor
    uint32_t    TB_out_offset; //offset in TB for indexing of scrambler xor
    TB_SlotBuilder_api  *slot_builder_module_p_; //Point to component of TB_SlotBuilder
	uint8_t 	*out_cb;  //out-Pointer to CodeBlock out (after scrambling)


};


// Name of input FIFO for code_block_main
#define CODE_BLOCK_MAIN_FIFO_IN   "CODE_BLOCK_MAIN_FIFO_IN_JOB_REQ"

//Profiler counter names
//CProfileCnt ldpc_prof_;
#define LDPC_PROF   "LDPC"
//CProfileCnt interleaver_prof_;
#define INTERLEAVE_PROF  "INTERLEAVE"
//CProfileCnt CRC_prof_;
#define CRC_PROF    "CRC"

//CProfileCnt test_prof_;
#define TEST_PROF    "TEST"


#endif /* API_MODULE_API_I_CODE_BLOCK_MAIN_API_H_ */
