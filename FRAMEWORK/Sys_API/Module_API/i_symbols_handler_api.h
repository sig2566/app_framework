/******************************************************************
 * i_symbols_handler_api.h
 * Author: Niv Arad
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_SYMBOLS_HANDLER_API_H_
#define API_MODULE_API_I_SYMBOLS_HANDLER_API_H_
#include "i_api_common.h"
#include "i_split_api.h"
#include "i_main_ru_handler_api.h"

#define MAX_UPLANE_SYMBOLS_POOL_CAPACITY (15)

// Module name
#define SYMBOL_HANDLER_MODULE	"SYMBOL_HANDLER"

enum UPLANE_SYMBOL_STATE
{
	SYMBOL_FREE,
	SYMBOL_RECEIVE,
	SYMBOL_ACTIVE,
	SYMBOL_IFFT,
	SYMBOL_LAST
};


struct uplane_symbols_api
{
	virtual EResultT			UPlaneSectionsParser(int8_t* message_ptr) = 0;
	virtual EResultT			UplaneSectionIdProcess(MESSAGE_DESCRIPTOR* message_descriptor, DATA_DESCRIPTOR* uplane_section_id_ptr) = 0;
	virtual uint32_t			getSymbolNumber() = 0;
	virtual UPLANE_SYMBOL_STATE		getSymbolState() = 0;
	virtual EResultT			checkStateMachineOnSymbolWindowCloseTick(UPLANE_SYMBOL_STATE* symbol_state) = 0;
	virtual SymbolRbPointers*	getRbsDataPointersTable() = 0;
	virtual void				DecrementSymbolWindowLength() = 0;
	virtual EResultT			InitSymbol(uint32_t symb_num, uint32_t left_rec_win_len) = 0;
	virtual void				ResetSymbol() = 0;
	virtual Complex*			CopyToRing(Complex* input_ptr, uint32_t Length) = 0;
};



/*******************************************************************************************//*!
*@class symbols_handler_api
*@brief The purpose of this class is :
*@brief API for CB to map the CB data into TB symbol map per layers
*@brief The MapCBData(uint32_t *data, uint32_t cb_length, uint32_t offset ) is used to call the data mapping
*@brief Params:
*@brief uint32_t *data - pointer to data after interleaving and scrambling and before QAM modulation. Data is kept as bit little endian bits stream.
*@brief uint32_t cb_length  - the lenght of the data in num bits
*@brief uint32_t offset - offset of the CB data in bits.
***********************************************************************************************/
struct uplane_symbols_handler_api
{
    virtual EResultT Init(
    		//CMemArea* data_engines_jobdescriptors_fifo_arr,
			CMemArea** data_raw_buffer,
			IModuleControlCallBackAPI* callback_ptr
			) = 0;
    virtual EResultT IHotStart() = 0;
    virtual EResultT RestartHot(SysTimeT* sys_time_init) = 0;
    virtual EResultT InitDu() = 0;
    virtual EResultT UPlaneSymbolClassifier(MESSAGE_DESCRIPTOR* pMessageDesc, uplane_symbols_api** symbol_object_ptr) = 0;
    virtual uplane_symbols_api* getSymbolObjectPtr(uint32_t symbol_id) = 0;
    virtual void     setSymbolObjectPtr(uint32_t symbol_id, uplane_symbols_api* symbol_obj_ptr) = 0;
    virtual EResultT checkSymbolsStateMachine() = 0;
    virtual EResultT getAvailableFreeSymbolObject(uplane_symbols_api** symbol_object_ptr) = 0;
    virtual void     decrementSymbolsWindow() = 0;
    virtual EResultT openNewSymbol(uint32_t current_symbol_RT) = 0;
    virtual uint32_t getNextSymbolToHandle() = 0;
    virtual EResultT ResetSymbolState() = 0;
    virtual EResultT SetDpdkDriverFpgaDlPtr (void* dpdk_eth_driver_fpga_api)=0;
#if 0
    virtual EResultT	sendJobDescriptorToBfDe(uplane_symbols_api* symbol_obj_ptr,
    			CMemArea* de_fifo_pointer) = 0;
#endif
  // virtual EResultT BuildRawDataBuffer()=0;
    virtual EResultT BuildMbufSendToChain(DataDirection direction)=0;
	virtual void	 SetProfiler(CProfileCnt* dl_prof, CProfileCnt* dl_symbol_tick_prof,CProfileCnt* ul_prof_,CProfileCnt* ul_symbol_tick_prof) = 0;
	virtual void     setRUsysVars(uint32_t* mtu_cpu_fpga_size_ptr, uint32_t* uplane_symbols_pool_capacity_ptr) = 0;
};



#endif /* API_MODULE_API_I_SYMBOLS_HANDLER_API_H_ */
