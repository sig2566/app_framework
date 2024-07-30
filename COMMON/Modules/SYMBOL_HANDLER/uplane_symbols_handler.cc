/******************************************************************
 * uplane_symbol_handler.cc
 * Author: E.Maidman
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include "i_ant_group_pool_api.h"
#include "uplane_symbols_handler.h"
#include "ru_mempool_defines.h"


////////////////////////////////////////////////////////////////////////////////////////
//										Common
////////////////////////////////////////////////////////////////////////////////////////


EResultT CUplaneSymbolsHandler::IInit(IModuleControlCallBackAPI *callback_ptr,
        ITarget *target_api){

   	char trace_format[]= "CUplaneSymbolsHandler\t%d";

    callback_ptr->IRegistryTraceEntry(trace_format, &send_burst_trace_id_);

	return E_OK;
}


EResultT CUplaneSymbolsHandler::IColdStart() 
{
	    void *api_p;
		IModuleControlAPI  *module_api_p;


		if (RU_)
		{
			char mod_name[]= "MAIN_RU_HANDLER";			
			ASSERT(callback_ptr_->IGetModule(mod_name, &module_api_p) == E_OK);
			module_api_p->IConfigure(e_GET_API, NULL, &api_p);
			main_ru_handler_module_ptr_ = (RU_Handler_API*)api_p;

			//########### connect to Networking ##########
			char networking_mod_name[]= "MAIN_NETWORKING";
			ASSERT(callback_ptr_->IGetModule(networking_mod_name, &m_networking_p_) == E_OK);
			void *main_networking_api;
			m_networking_p_->IConfigure(e_GET_API, NULL, &main_networking_api);
			main_networking_api_ = (RU_Networking_API*)main_networking_api;
		}

	    return E_OK;
}


EResultT CUplaneSymbolsHandler::IWarmStart() 
{
	//	Connect to DPDK_ETH_offline driver factory module:
	if (RU_)
	{
		//DPDK mempools
		uplane_mempool_ = (rte_mempool*)main_ru_handler_module_ptr_->getUPlaneDlMempool();
		eth_headers_mempool_ = (rte_mempool*)main_ru_handler_module_ptr_->getUplaneEthHeadersMempool();
		zeros_mempool_ = (rte_mempool*)main_ru_handler_module_ptr_->getZerosMempool();
		 //Uplane MBUF ring - we allocated in advance uplane mbufs.
	    char ringNameUplane[128];
		uint32_t buffers_number = 0;
		sprintf(ringNameUplane, "UplaneMbufsPreallocRing");
		buffers_number = (UPLANE_DL_SIZE > 0) ? pow(2,ceil(log2(UPLANE_DL_SIZE))) : 0 ;
		//1.crate ring for U-Plane mbuf
		uplane_mbufs_preallocated_ring_ = rte_ring_create(ringNameUplane, buffers_number, SOCKET_ID_ANY, RING_F_SP_ENQ | RING_F_SC_DEQ);
		if (!uplane_mbufs_preallocated_ring_)
		{
			printf("Could not create U-Plane mbufs ring. ERRNO: %s\n", rte_strerror(rte_errno));
			ASSERT(0);
		}
		rte_mbuf* preallocated_mbufs_arr_uplane[UPLANE_DL_SIZE];
		//2.allocate array of mbuf's
		uint32_t	    ret_val = rte_pktmbuf_alloc_bulk(uplane_mempool_, preallocated_mbufs_arr_uplane, UPLANE_DL_SIZE);
		if(ret_val)
		{
			printf("Could not preallocate U-Plane mbufs. ERRNO = %d\n", ret_val);
			ASSERT(0);
		}
		uint32_t	     	free_space = 0;
		//3.put the array on the ring
		if (rte_ring_enqueue_bulk(	uplane_mbufs_preallocated_ring_,
				(void * const*)(preallocated_mbufs_arr_uplane),
													UPLANE_DL_SIZE,
				       								&free_space) !=
				       										UPLANE_DL_SIZE)
		{
			printf("Could not enqueue %d packets to Ring. Packets dropped. Free space on Ring = %d\n", UPLANE_DL_SIZE, free_space);
		}
		for(uint16_t symbol_counter=0; symbol_counter<uplane_symbols_pool_capacity_; symbol_counter++)
		    {
		    	dl_symbol_[symbol_counter].SetMempoolPtr(uplane_mempool_);
		    	dl_symbol_[symbol_counter].SetMainRuPtr(main_ru_handler_module_ptr_);
		    	dl_symbol_[symbol_counter].SetRingPtr(uplane_mbufs_preallocated_ring_);
		    }
		    Complex temp_zeors_data[MAX_NUM_OF_RBS_BH*NUMBER_SC_IN_RB*sizeof(Complex)];
	    	InitZerosVector(temp_zeors_data,MAX_NUM_OF_RBS_BH*NUMBER_SC_IN_RB);
	    	       //put the zeors data inside the mbuf
	    	pkt_zeors_data_  = rte_pktmbuf_alloc(zeros_mempool_);
		    Complex* data_in_mbuf = rte_pktmbuf_mtod(pkt_zeors_data_, Complex*);
		    memcpy(data_in_mbuf, temp_zeors_data,MAX_NUM_OF_RBS_BH*NUMBER_SC_IN_RB*sizeof(Complex));
		    pkt_zeors_data_->pkt_len = pkt_zeors_data_->data_len=MAX_NUM_OF_RBS_BH*NUMBER_SC_IN_RB*sizeof(Complex);

		    //ethernet headers MBUF ring - we allocated in advance Ethernet header mbufs.
		    char ringName[128];
		    buffers_number = 0;
		    sprintf(ringName, "EthHeaderMbufsPreallocRing");
		    uint32_t num_of_mtus_uplane = UPLANE_Data_SIZE / mtu_cpu_fpga_size_ + ((UPLANE_Data_SIZE % mtu_cpu_fpga_size_) > 0);
		    uint32_t ethernet_header_uplane_dl_size = DBL_BUF*(RU_MAX_THEORETICAL_NUM_LAYERS)*num_of_mtus_uplane* (1 + EXTRA_BUFFERS_BLOCK);
		    buffers_number = (ethernet_header_uplane_dl_size > 0) ? pow(2,ceil(log2(ethernet_header_uplane_dl_size))) : 0 ;
		    //1. create ring
		    eth_haders_mbufs_preallocated_ring_ = rte_ring_create(ringName, buffers_number, SOCKET_ID_ANY, RING_F_SP_ENQ | RING_F_SC_DEQ);
		    if (!eth_haders_mbufs_preallocated_ring_)
		    {
		    	printf("Could not create ETh headrs mbufs ring. ERRNO: %s\n", rte_strerror(rte_errno));
		    	ASSERT(0);
		    }
		    rte_mbuf* preallocated_mbufs_arr[ethernet_header_uplane_dl_size];
		    //2. allocate ring of mbuf's
		    ret_val = rte_pktmbuf_alloc_bulk(eth_headers_mempool_, preallocated_mbufs_arr, ethernet_header_uplane_dl_size);
		    if(ret_val)
		    {
		    	printf("Could not preallocate Eth headers mbufs. ERRNO = %d\n", ret_val);
		     	ASSERT(0);
   		    }
		    //3. put data on the mbuf's
		    for(uint32_t pkt_counter=0; pkt_counter<(ethernet_header_uplane_dl_size); pkt_counter++)
		    {
		    	//put the data inside the new mbufs, will in the run we will need update only the layer num.
		    	//Here we put zero a layer number.
		    	//preallocated_mbufs_arr[pkt_counter]->data_len = ETH_HEADER_BUFF_SIZE - RTE_PKTMBUF_HEADROOM; // predefine data length in order to be able to adjust bellow. Further this field should be updated properly
	    		//rte_pktmbuf_adj(preallocated_mbufs_arr[pkt_counter], (HEADER_SIZE_FOR_CHAINING-RTE_PKTMBUF_HEADROOM) );


#ifdef USE_NXP_PATCH_FOR_CHAIN_USAGE_OPTIMIZATION

				// move data pointer after the head room for allowing the chaining
				//pkt_eth->data_len = UPLANE_DL_ETH_HEADRS_BUFF_SIZE - RTE_PKTMBUF_HEADROOM; // predefine data length in order to be able to adjust bellow. Further this field should be updated properly
		    	rte_mbuf* pkt_eth = preallocated_mbufs_arr[pkt_counter];
		    	rte_pktmbuf_adj(pkt_eth, (HEADER_SIZE_FOR_CHAINING-RTE_PKTMBUF_HEADROOM) );
				EtherHeadr *data_in_mbuf_EtherHeadr_ptr = rte_pktmbuf_mtod(pkt_eth, EtherHeadr*);
#else
				EtherHeadr *data_in_mbuf_EtherHeadr_ptr = rte_pktmbuf_mtod(preallocated_mbufs_arr[pkt_counter], EtherHeadr*);
#endif
				main_networking_api_->AddEthernetHeader_FPGA(data_in_mbuf_EtherHeadr_ptr, DL_ANT_DATA);
		    }

		    free_space = 0;
		    //4. put the array on the ring
		    if (rte_ring_enqueue_bulk(	eth_haders_mbufs_preallocated_ring_,
		           							(void * const*)(preallocated_mbufs_arr),
											ethernet_header_uplane_dl_size,
		       								&free_space) !=
		       										ethernet_header_uplane_dl_size)
		    {
		    	printf("Could not enqueue %d packets to Ring. Packets dropped. Free space on Ring = %d\n", ethernet_header_uplane_dl_size, free_space);
		    }

	}
	else
	{
	    uplane_symbols_pool_capacity_ = MAX_UPLANE_SYMBOLS_POOL_CAPACITY;
	}

	return E_OK;
}

EResultT CUplaneSymbolsHandler::IHotStart() {
	chain_head_counter_=0;
    return E_OK;
}

EResultT CUplaneSymbolsHandler::RestartHot(SysTimeT* sys_time_init) {

	EResultT ret_value = E_OK;
	uint32_t left_receive_window_length_;
	int32_t  symb_nums[uplane_symbols_pool_capacity_];

    memset(&active_symbols_objects_ptrs_[0], 0, sizeof(uplane_symbols_api*) * MAX_NUM_OFDM_PER_SLOT); //set all pointers to NULL

    //!! NOTE !! in Online mode :: Because we are not work in RT yet (the symbol ticks drifts) we set the symbol tick
    //                             to 0 and manage it in symbol handler internally and not base on the sync clock - from the GL.

#ifndef RU_RT_MODE
    current_active_symbol_RT_ = (sys_time_init->offset % SLOT_DURATION)/SYMBL_DURATION;  // Set current symbol with synchronized time.
#else
    current_active_symbol_RT_ = 0;
#endif

    // Initialize all the Symbols from the Pool, except for the last one
    for(int32_t pool_symbol_idx = 0; pool_symbol_idx < uplane_symbols_pool_capacity_ - 1; pool_symbol_idx++)
    {
    	symb_nums[pool_symbol_idx]  = current_active_symbol_RT_ + pool_symbol_idx;  // Add time synchronization symbol (Not necessarily starts from 0)
    	left_receive_window_length_ = pool_symbol_idx + 1;
    	dl_symbol_[pool_symbol_idx].InitSymbol(symb_nums[pool_symbol_idx], left_receive_window_length_); //@@@@

    	// set proper table entry for the new Symbol
		setSymbolObjectPtr(pool_symbol_idx, static_cast<uplane_symbols_api*>(&dl_symbol_[pool_symbol_idx]));
    }
    // Initialize the last symbol as Free
    dl_symbol_[uplane_symbols_pool_capacity_ - 1].ResetSymbol();

    last_opened_symbol_number_	= current_active_symbol_RT_ + uplane_symbols_pool_capacity_ - 2;   //@@@@ Current symbol + (window size -??2)

	return E_OK;
}

EResultT CUplaneSymbolsHandler::IStop(ESeverityT severity) {

    return E_OK;
}
EResultT CUplaneSymbolsHandler::ICall(SysTimeT *sys_time_p, uint32_t param) {

	return E_OK;
}
EResultT CUplaneSymbolsHandler::IConfigure(uint32_t id, void *in, void **out) {

	if (id == e_GET_API)
	{
		*out = (void*) static_cast<uplane_symbols_handler_api*>(this); // point to virtual functions of symbols_handler_api
	}

	return E_OK;
}
EResultT CUplaneSymbolsHandler::IGetInfo(char* module_name, uint32_t *major_ver,
        uint32_t *minor_ver, uint32_t *build_num, char* add_info) {
	if(strcmp(add_info, "DU")==0)
		RU_ = false;

    return E_OK;
}

EResultT CUplaneSymbolsHandler::SetDpdkDriverFpgaDlPtr (void* dpdk_eth_driver_fpga_dl_api)
{
	dpdk_eth_driver_fpga_dl_api_=(DpdkEthDeviceAPI*)dpdk_eth_driver_fpga_dl_api;
	return E_OK;
}

/******************************************************************
 *@brief	Initialization for U-Plane Symbols Handler,e.g.
 *@brief	Memory initialization, etc.
 *@brief
 *@brief	Input:	data_engines_jobdescriptors_fifo_arr - pointer to an
 *@brief				array of DataEngine JDs FIFO.
 *@brief			callback_ptr - callback pointer
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::Init(
		//CMemArea* data_engines_jobdescriptors_fifo_arr,
		CMemArea** data_raw_buffer,
		IModuleControlCallBackAPI* callback_ptr)
{
	EResultT	ret_value = E_OK;

	// copy FIFO objects pointers to Symbol Handler in order to send JDs to all the DEs
	//memcpy(&data_engines_job_descs_send_, &data_engines_jobdescriptors_fifo_arr, sizeof(CMemArea*));
	memcpy(m_ru_raw_data__send_, data_raw_buffer, MAX_DL_DATA_STREAMS_PER_RB*sizeof(CMemArea*));

	//to add copy data like here

	// Initialize callback pointer of the main GL module in order to be able to make ICall from the Symbols Handler
	callback_ptr_ = callback_ptr;

    //connect to Ant groups pool
    //char ant_group_pool_mod_name[] = ANT_GROUP_POOL_MODULE;
    //ASSERT(callback_ptr_->IGetModule(ant_group_pool_mod_name, &ant_pool_module_p_) == E_OK);

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: UPlaneSymbolClassifier
 *@brief  Desc:	need to be called after O-RAN U-Plane App.Layer parser.
 *@brief
 *@brief  Input:	data_packet_ptr - pointer to a new U-Plane
 *@brief  Output:	symbol_object_ptr - pointer to the proper Symbol's object
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::UPlaneSymbolClassifier(MESSAGE_DESCRIPTOR* pMessageDesc, uplane_symbols_api** symbol_object_ptr)
{
	EResultT	ret_value = E_OK;

	*symbol_object_ptr = getSymbolObjectPtr(pMessageDesc->symbol_id_);

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: getSymbolObjectPtr
 *@brief  Desc:	return proper Symbol Object pointer from the symbol number
 *@brief
 *@brief  Input:	symbol_id_ - Symbol number that its Object should be found
 *@brief  Output:	CUpalneSymbol	- pointer to the relevant Symbol's Object
 *@brief
 *******************************************************************/
inline uplane_symbols_api* CUplaneSymbolsHandler::getSymbolObjectPtr(uint32_t symbol_id)
{
#if 0
	CUplaneSymbol* symbol_object_ptr = NULL;
	CUplaneSymbol* current_symbol_object_ptr;

	for(uint32_t pool_symbol_idx=0; (pool_symbol_idx<uplane_symbols_pool_capacity_); pool_symbol_idx++)
	{
		current_symbol_object_ptr = &dl_symbol_[pool_symbol_idx];

		if(symbol_id == current_symbol_object_ptr->getSymbolNumber())
		{
			symbol_object_ptr = current_symbol_object_ptr;
			break;
		}

	}

	//if(!symbol_object_ptr)
	//{
		//	symbol_id_ not found among the symbols in the pool!
	//}

	return static_cast<uplane_symbols_api*>(symbol_object_ptr);
#endif

	return active_symbols_objects_ptrs_[symbol_id];
}

/******************************************************************
 *@brief  Function Name: setSymbolObjectPtr
 *@brief  Desc:	sets proper Symbol Object pointer of the given symbol number to the table of pointers
 *@brief
 *@brief  Input:	symbol_id_ 		- Symbol number that its Object should be found
 *@brief  			symbol_obj_ptr	- pointer to the relevant Symbol's Object
 *@brief  Output:
 *@brief
 *******************************************************************/
inline void CUplaneSymbolsHandler::setSymbolObjectPtr(uint32_t symbol_id, uplane_symbols_api* symbol_obj_ptr)
{
	active_symbols_objects_ptrs_[symbol_id] = symbol_obj_ptr;
}


/******************************************************************
 *@brief  Function Name: checkSymbolsStateMachine
 *@brief  Desc:	pass over all the Symbols in the Pool
 *@brief  		and check (and change if needed) their current status
 *@brief  		(currently will be called on a new Symbol's window close tick)
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::checkSymbolsStateMachine()
{
	EResultT		ret_value				= E_OK;
	CUplaneSymbol*		symbol_object		= NULL;
	UPLANE_SYMBOL_STATE	symbol_state		= SYMBOL_LAST;

	//go over all the Symbols in the Symbols Pool, update their states if needed and send Job Descriptor to BF DE when required
	for(uint32_t symbol_counter=0; symbol_counter<uplane_symbols_pool_capacity_; symbol_counter++)
	{
		dl_symbol_[symbol_counter].checkStateMachineOnSymbolWindowCloseTick(&symbol_state);
		if(SYMBOL_ACTIVE == symbol_state) // if the Symbol moved to active state -> send JDs to all FIFOs
		{

#ifdef RU_RT_MODE
			current_active_symbol_RT_ = dl_symbol_[symbol_counter].getSymbolNumber();
#else
			ASSERT(current_active_symbol_RT_ == dl_symbol_[symbol_counter].getSymbolNumber()) // Check if the System symbol tick align with allocated symbol in the open possessed window
#endif
																			              // In the RT Online mode it indicate if the symbol processed in the boundaries of its size -> we "stand" in RT
			/*
			uint32_t first_antenna	= 0;
			uint32_t antennas_range = NUM_ANTENNAS_PER_ANT_GROUP;
			for(uint32_t bf_de_counter=0; bf_de_counter<BF_ENGINES_NUMBER; bf_de_counter++)
			{
				//Prepare FIFO for BF engine directly
				sendJobDescriptorToBfDe(&dl_symbol_[symbol_counter], bf_engines_job_descs_send_[bf_de_counter], first_antenna, antennas_range);
				first_antenna += NUM_ANTENNAS_PER_ANT_GROUP;
			}
*/

//			if (RU_){
//				//Call ant pool-for multiply BF
//				SysTimeT sys_time_p;
//
//				callback_ptr_->IGetSysTime(&sys_time_p);
//
//				//prof_ant_bf_.Start();
//				//ant_pool_module_p_->ICall(&sys_time_p, (uint32_t)E_DIRECT);
//				//prof_ant_bf_.Stop();
//			}

			break; // quit the loop - since there should be only one Symbol in BF state
		}
	}
	return ret_value;
}


/******************************************************************
 *@brief  Function Name: getAvailableFreeSymbolObject
 *@brief  Desc: get the first available FREE Symbol object among the symbols in the pool
 *@brief
 *@brief
 *@brief  Input:	symbol_object_ptr - pointer to a new Symbol's object passed by reference
 *@brief  Output:	symbol_object_ptr - pointer to a new available FREE Symbol's object
 *@brief  			ret_value - OK if a new available object is found, NOT_OK - otherwise
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::getAvailableFreeSymbolObject(uplane_symbols_api** symbol_object_ptr)
{
	EResultT	ret_value = E_OK;
	UPLANE_SYMBOL_STATE sym_state;

	for(int32_t pool_symbol_idx = 0; pool_symbol_idx < uplane_symbols_pool_capacity_; pool_symbol_idx++)
	{
		sym_state = dl_symbol_[pool_symbol_idx].getSymbolState();
		if(sym_state == SYMBOL_FREE)
		{
			*symbol_object_ptr = static_cast<uplane_symbols_api*>(&dl_symbol_[pool_symbol_idx]);
			break; // quit the loop - since there should be only one Symbol in active state
		}
	}

	ASSERT(symbol_object_ptr) // validate getting non-zero FREE object pointer
	if(!(*symbol_object_ptr))
		ret_value = E_FAIL;
	
	return ret_value;
}


/******************************************************************
 *@brief  Function Name: decrementSymbolsWindow
 *@brief  Desc: go over all Symbols in the Pool and decrement Symbols window length
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *@brief
 *******************************************************************/
void CUplaneSymbolsHandler::decrementSymbolsWindow()
{
	//go over all the Symbols in the Symbols Pool and decrement their window length
	for(uint32_t symbol_counter=0; symbol_counter<uplane_symbols_pool_capacity_; symbol_counter++)
	{
		dl_symbol_[symbol_counter].DecrementSymbolWindowLength();
	}

}


/******************************************************************
 *@brief  Function Name: openNewSymbol
 *@brief  Desc: open a new Symbol
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::openNewSymbol(uint32_t current_symbol_RT)
{
#ifndef RU_RT_MODE
	current_active_symbol_RT_ = current_symbol_RT; //Set symbol value from GL -> synchronized time
#endif
	EResultT	ret_value = E_OK;
	uplane_symbols_api*  symbol_object_ptr = NULL;
	uint32_t 	symbol_number;

	ret_value = getAvailableFreeSymbolObject(&symbol_object_ptr);
	if(E_OK == ret_value)
	{
		last_opened_symbol_number_++;
		last_opened_symbol_number_ %= MAX_NUM_OFDM_PER_SLOT;
		symbol_object_ptr->InitSymbol(last_opened_symbol_number_, (uplane_symbols_pool_capacity_ - 1));
		// set proper table entry for the new Symbol
		setSymbolObjectPtr(last_opened_symbol_number_, symbol_object_ptr);
	}

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: getNextSymbolToHandle
 *@brief  Desc: get the Symbol Id that should pass the BF processing in RU DL or sorting processing in DU UL
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output: active Symbol Id
 *@brief
 *@brief
 *******************************************************************/
uint32_t CUplaneSymbolsHandler::getNextSymbolToHandle()
{
	return current_active_symbol_RT_;
}


////////////////////////////////////////////////////////////////////////////////////////
//										RU
////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************
 *@brief  Function Name: ResetSymbolState
 *@brief  Desc:	Send JD to all BF DEs and then call to all BF Engines for processing
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::ResetSymbolState()
{
	EResultT		ret_value		= E_OK;
	uplane_symbols_api*		symbol_obj_ptr	= getSymbolObjectPtr(current_active_symbol_RT_);

	// Reset the symbol to Free state after BF
	symbol_obj_ptr->ResetSymbol();
	// reset proper table entry to NULL pointer
	setSymbolObjectPtr(current_active_symbol_RT_, (uplane_symbols_api*	)NULL);

	return ret_value;
}


/******************************************************************
 *@brief  Function Name: SetProfiler
 *@brief  Desc:	Set DL profilers from GL to symbol handler
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *******************************************************************/
void CUplaneSymbolsHandler::SetProfiler(CProfileCnt* dl_prof, CProfileCnt* dl_symbol_tick_prof,CProfileCnt* ul_prof,CProfileCnt* ul_symbol_tick_prof)
{
	dl_prof_ = dl_prof;
	ul_prof_ = ul_prof;
	dl_symbol_tick_prof_ = dl_symbol_tick_prof;
	ul_symbol_tick_prof_=ul_symbol_tick_prof;
}


/******************************************************************
*@brief  Function name:	setRUsysVars
*@brief  Description:   Function that sets the RU system variables
*@brief
*@brief  Input:			mtu_cpu_fpga_size_ptr - pointer to the FPGA MTU size
*@brief                 uplane_symbols_pool_capacity_ptr - pointer to the UPlane Symbol pool capacity
*@brief
*@brief  Output:
*@brief
*******************************************************************/
void CUplaneSymbolsHandler::setRUsysVars(uint32_t* mtu_cpu_fpga_size_ptr, uint32_t* uplane_symbols_pool_capacity_ptr)
{
	mtu_cpu_fpga_size_ = *mtu_cpu_fpga_size_ptr;
	if (*uplane_symbols_pool_capacity_ptr > MAX_UPLANE_SYMBOLS_POOL_CAPACITY)
	{
		printf("U-Plane symbol pool capacity is larger than %d\n", MAX_UPLANE_SYMBOLS_POOL_CAPACITY);
		ASSERT(0);
	}
	else
	{
		uplane_symbols_pool_capacity_ = *uplane_symbols_pool_capacity_ptr;
	}
}


/******************************************************************
 *@brief  Function Name: BuildRawDataBuffer
 *@brief  Desc:	Send UPLANE data from JD to the Glue Logic Test
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *******************************************************************/
#if 0
EResultT CUplaneSymbolsHandler::BuildRawDataBuffer()
{

	EResultT		ret_value		= E_OK;
	uplane_symbols_api*		symbol_obj_ptr	= getSymbolObjectPtr(current_active_symbol_);
	SymbolRbPointers* single_rb_data_pointers=symbol_obj_ptr->getRbsDataPointersTable();
	SingleRbDataPointersAndInfo* single_rb_data_pointers_ptr = single_rb_data_pointers->single_rb_data_pointers;
	Complex*  Data_raw_ptr;
	void* popped_item_from_fifo = nullptr;
	uint32_t zeros_flag=0;
	uint32_t size_of_data;
	char	q_name[50];
	uint32_t rb_num_counter=0;

	for(uint32_t layer_num_counter=0; layer_num_counter<RU_MAX_NUM_OF_DL_LAYERS;layer_num_counter++)
	{
		rb_num_counter=0;

		sprintf(q_name,"UplaneDL_Layer_%d",layer_num_counter);
		while(rb_num_counter<MAX_NUM_OF_RBS_BH)
		{
			size_of_data=0;
			//check if the RB is alloceted
			if (single_rb_data_pointers_ptr[rb_num_counter].run_length_[layer_num_counter]==0)
			{
				while ((single_rb_data_pointers_ptr[rb_num_counter].run_length_[layer_num_counter]==0)&& (rb_num_counter!=MAX_NUM_OF_RBS_BH))
				{
					//count all the not alloceted RB in a row
					rb_num_counter=rb_num_counter+1;
					size_of_data=size_of_data+NUMBER_SC_IN_RB;

            	}
				//send zeors for not alloceted RB's
              //  glue_logic_test_p_->SendDemo(temp_zeors_data,size_of_data, sizeof(Complex), q_name);


            }
			else
			{
				//send DATA for alloceted RB's according to run_length
    			size_of_data=single_rb_data_pointers_ptr[rb_num_counter].run_length_[layer_num_counter]*NUMBER_SC_IN_RB;
    			glue_logic_test_p_->SendDemo(single_rb_data_pointers_ptr[rb_num_counter].rb_data_ptr_per_stream_[layer_num_counter], size_of_data, sizeof(Complex), q_name);
    			rb_num_counter=rb_num_counter+(single_rb_data_pointers_ptr[rb_num_counter].run_length_[layer_num_counter]);
            }
		}

	}
	return ret_value;
}
#endif

/******************************************************************
 *@brief  Function Name: PushZerosToChain
 *@brief  Desc:	Create Mbuf with a pointer to the zeros mbuf and push it to the chain.
 *@brief
 *@brief
 *@brief  Input: uint32_t size_of_data - the zeors data size, rte_mbuf** last_packet_ptr- pointer to the last mbuf in the chain
 *@brief  Output:none
 *@brief
 *******************************************************************/
void CUplaneSymbolsHandler::PushZerosToChain(uint32_t size_of_data ,rte_mbuf** last_packet_ptr,rte_mbuf* pkt_chain_head)
{
	rte_mbuf *zeros_packet;
	uint8_t* packet_data;
	uint32_t retVal = rte_ring_dequeue(uplane_mbufs_preallocated_ring_, (void **)&zeros_packet);
	ASSERT(retVal==0);
	zeros_packet->next=nullptr;
	zeros_packet->nb_segs=1;
	zeros_packet->data_off=0;
	zeros_packet->ol_flags=0;
	zeros_packet->buf_len=0;
	zeros_packet->refcnt=1;

	// increment refcnt of the mbuf for using it again without allocation from mempool, but taking it from our preallocated Ring array
	zeros_packet->refcnt++;
	//need to free the next, beacuse the eth header is part of a chain, and when we take this mbuf again we need that
	// the mbuf will be clean.
	// Enqueue the mbuf ptr back to the ring for next usage.
	// We expect that the Ring will be big enough so that when we will use this mbuf again
	// it already should be released by the DPDK Offline Driver (in case of Offline run) or by HW (in case of Online DPDK Driver usage on Send)
	retVal = rte_ring_enqueue(uplane_mbufs_preallocated_ring_, (void *)(zeros_packet));
	ASSERT(retVal==0);



#ifdef RU_RT_MODE
#ifdef PRINT_MEMPOOL_LOG
	//uint32_t in_use_buffers_number 		= rte_mempool_in_use_count(uplane_mempool_);
	//printf("UPlane mempool\t\t%llx: in_use_buffers_number=%d, alloc buff ptr = 0x%llx\n",uplane_mempool_, in_use_buffers_number, zeros_packet);
#endif
#endif

//	uint16_t free_mbufs =  rte_mempool_avail_count(uplane_mempool_);
//	printf("Number of input free bufs %d\n", free_mbufs);
    //Allocate new mbuf
	packet_data = rte_pktmbuf_mtod_offset(pkt_zeors_data_, uint8_t*, 0);
	//put the pointer in the new mbuf
	rte_pktmbuf_attach(zeros_packet, pkt_zeors_data_); // no need to adjust the indirect mbuf ptr later since here we use zeros vector and we always need to point to its start
	zeros_packet->buf_addr=pkt_zeors_data_->buf_addr;
	zeros_packet->pkt_len =  zeros_packet->data_len=size_of_data; //update the data len in the new mbuf
	rte_pktmbuf_chain(pkt_chain_head,zeros_packet );
}

/******************************************************************
 *@brief  Function Name: BuildMbufSendToChain
 *@brief  Desc:	The main function that create the U-Plane mbuf chain.
 *@brief
 *@brief
 *@brief  Input: none
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
#define SPLIT_OUTPUT_ZEROS_TO_CHUNKS // for debug testing of more than 1 mbufs in chain performance (until we'll have a possibility to run real traffic in RT mode coming from the FH)
EResultT CUplaneSymbolsHandler::BuildMbufSendToChain(DataDirection direction)
{

	EResultT		ret_value		= E_OK;
	uplane_symbols_api*		symbol_obj_ptr	= getSymbolObjectPtr(current_active_symbol_RT_); // @@@@ get the current symbol number   @@@@@
	SymbolRbPointers* single_rb_data_pointers = nullptr;
	SingleRbDataPointersAndInfo* single_rb_data_pointers_ptr = nullptr;
	uint16_t run_len = 0;
	uint8_t *run_len_ptr;
	uint32_t size_of_data;
	uint32_t rb_num_counter=0;
	uint32_t j=0;
	rte_mbuf* pkt_eth; //mbuf with the eth header data
	rte_mbuf* pkt_chain_head; //the chain head mbuf
	EgressQueueAPI* qeue;
	rte_mbuf* last_packet_ptr; //pointer to the last mbuf in the chain
	uint32_t size_of_data_before_send = 0; // the total data size we have in the current chain
	uint32_t size_of_data_zeros = 0; // the total data size we have in the current chain
	bool		sending_loop_on = TRUE;
	qeue = dpdk_eth_driver_fpga_dl_api_->GetEgressQueue(EGRESS_UNIFIED_QUEUE); //connect to Q for send

	for(uint32_t layer_num_counter = 0; layer_num_counter < RU_MAX_NUM_OF_DL_LAYERS; layer_num_counter++) //run on all the layers of the symbol
	{
		rb_num_counter = 0;
		if(kDL == direction)
		{
			single_rb_data_pointers = symbol_obj_ptr->getRbsDataPointersTable();
			single_rb_data_pointers_ptr = single_rb_data_pointers->single_rb_data_pointers[layer_num_counter]; //get pointer to RB data
			run_len_ptr = (uint8_t *)&(single_rb_data_pointers_ptr[rb_num_counter].run_length_);
			run_len = (*(uint16_t*)(run_len_ptr));
		}
		size_of_data_before_send = 0;
		size_of_data_zeros = 0;
		sqid_ = rb_num_counter; // for the first packet of the Symbol (this field is similar to RBS field, i.e. first RB in the packet)
		ResetNewMbufChain(&last_packet_ptr, pkt_eth, pkt_chain_head, layer_num_counter); // call to function the reset the chain head, create header mbuf and trailer mbuf

		while(rb_num_counter<MAX_NUM_OF_RBS_BH)
		{
			//the case we have empty section and we need to use the zeros mbuf
			if (run_len == 0)
			{
				bool end_of_packet=TRUE ; //If all packets till end of MTU or till end of RBs will be 0 - this is end of packet
				uint32_t first_rb_num_counter=rb_num_counter; // for allocation where data is there in between zero RBS : To rememember offset
				uint32_t max_num_of_rbs_for_zeros_vector;
				uint32_t left_rbs=MAX_NUM_OF_RBS_BH-rb_num_counter;

				//for run_length=0,increment zeros of RBs till run_length!=0 or till we get to MTU size
				uint32_t mtu_size_for_payload_only = mtu_cpu_fpga_size_ - sizeof(U_plane_CPU2FPGA_Headr) - sizeof(EtherHeadr);
				uint32_t left_bytes_till_end_mtu = mtu_size_for_payload_only;
				left_bytes_till_end_mtu=left_bytes_till_end_mtu-size_of_data_before_send;
				uint32_t left_rbs_till_end_mtu=left_bytes_till_end_mtu/(NUMBER_SC_IN_RB*sizeof(Complex));
				if(left_rbs<left_rbs_till_end_mtu)
					max_num_of_rbs_for_zeros_vector=left_rbs;
				else
					max_num_of_rbs_for_zeros_vector=left_rbs_till_end_mtu; 
				
				if(kDL == direction)
				{
					//for run_length=0,increment zeros of RBs till run_length!=0 or till we get to MTU size
					for(j=0;j<max_num_of_rbs_for_zeros_vector;j++)
					{
						rb_num_counter++;
						run_len_ptr = run_len_ptr + sizeof(SingleRbDataPointersAndInfo);
						run_len = (*(uint16_t*)(run_len_ptr));
						if(run_len!=0 && j<max_num_of_rbs_for_zeros_vector-1)
						{
							bool end_of_packet=FALSE;
							break;
						}

					}
					size_of_data_zeros = size_of_data_zeros + (rb_num_counter-first_rb_num_counter)* (NUMBER_SC_IN_RB*sizeof(Complex));
					size_of_data_before_send = size_of_data_before_send + (rb_num_counter-first_rb_num_counter)* (NUMBER_SC_IN_RB*sizeof(Complex));

				}
				else //UL
				{
					rb_num_counter=rb_num_counter+max_num_of_rbs_for_zeros_vector;
					size_of_data_zeros = size_of_data_zeros + max_num_of_rbs_for_zeros_vector* (NUMBER_SC_IN_RB*sizeof(Complex));
					size_of_data_before_send = size_of_data_before_send + + max_num_of_rbs_for_zeros_vector*(NUMBER_SC_IN_RB*sizeof(Complex));
				}
				sqid_ = rb_num_counter;
				if(size_of_data_zeros > 0) //check if we need to add zeroes packet to the chain
				{
#if (defined RU_RT_MODE && defined SPLIT_OUTPUT_ZEROS_TO_CHUNKS)

					if (kDL == direction)
					{
						//spilt zeros chain for RT send burst testing
						uint32_t one_packet_size_modulo=max_num_of_rbs_for_zeros_vector%CHAIN_ZEROS_SIZE;
						uint32_t number_packet_in_size=max_num_of_rbs_for_zeros_vector/CHAIN_ZEROS_SIZE;
						bool first_time=true;
						for(int i=0;i<CHAIN_ZEROS_SIZE;i++)
						{
							if (first_time)
							{
								size_of_data_zeros=(one_packet_size_modulo+number_packet_in_size)*(NUMBER_SC_IN_RB*sizeof(Complex));
								first_time=false;
							}
							else
							{
								size_of_data_zeros= number_packet_in_size* (NUMBER_SC_IN_RB*sizeof(Complex));
							}
							PushZerosToChain(size_of_data_zeros, &last_packet_ptr, pkt_chain_head); //add the zeros packet to the chain
    						}
					}
					else
					{
						PushZerosToChain(size_of_data_zeros, &last_packet_ptr, pkt_chain_head); //add the zeros packet to the chain
					}
					size_of_data_zeros=0;
#else
					PushZerosToChain(size_of_data_zeros, &last_packet_ptr, pkt_chain_head); //add the zeros packet to the chain
					size_of_data_zeros=0;
#endif
				}
				//if ((sizeof(U_plane_CPU2FPGA_Headr) + sizeof(EtherHeadr) + size_of_data_before_send + NUMBER_SC_IN_RB*sizeof(Complex)) > mtu_cpu_fpga_size_) //check if we pass the MTU size and need to send the chain
				if(end_of_packet==TRUE)
				{


							//save the current chain to send in the end of the symbol
							//also update the chain counter

    						chain_heads_to_send_[chain_head_counter_]=pkt_chain_head;
    						chain_head_counter_++;
 if(kDL == direction)
 {
#ifdef RU_RT_MODE
#ifdef PRINT_MEMPOOL_LOG
						//uint32_t in_use_buffers_number 		= rte_mempool_in_use_count(uplane_mempool_);
						//printf("UPlane mempool\t\t%llx: in_use_buffers_number=%d\n",uplane_mempool_, in_use_buffers_number);
#endif
#endif

 }
	            		//rte_pktmbuf_free(pkt_chain_head); 	// debugging !!!
						//ret_value = E_OK;      				// debugging !!!

					size_of_data_before_send = 0; //rest the size of the data in the chain
					ResetNewMbufChain(&last_packet_ptr, pkt_eth, pkt_chain_head, layer_num_counter);
				}
			}
			else // the case we have data inside the section
			{
				size_of_data = single_rb_data_pointers_ptr[rb_num_counter].pkt_data->data_len; //get the current section size
				if ((size_of_data + sizeof(U_plane_CPU2FPGA_Headr) + sizeof(EtherHeadr)) > mtu_cpu_fpga_size_)
				{
					SpearteSectionPerPerMtu(single_rb_data_pointers_ptr[rb_num_counter].pkt_data, &last_packet_ptr, pkt_chain_head, layer_num_counter, qeue, size_of_data_before_send, direction);
					ResetNewMbufChain(&last_packet_ptr, pkt_eth, pkt_chain_head, layer_num_counter);
					size_of_data_before_send = 0;
				}
				else
				{
					if ((size_of_data_before_send + sizeof(U_plane_CPU2FPGA_Headr)  + sizeof(EtherHeadr) + size_of_data) > mtu_cpu_fpga_size_)
					{
						//save the current chain to send in the end of the symbol
						//also update the chain counter
                        chain_heads_to_send_[chain_head_counter_]=pkt_chain_head;
                         chain_head_counter_++;
 if(kDL == direction)
 {
#ifdef RU_RT_MODE
#ifdef PRINT_MEMPOOL_LOG
							//uint32_t in_use_buffers_number 		= rte_mempool_in_use_count(uplane_mempool_);
							//printf("UPlane mempool\t\t%llx: in_use_buffers_number=%d\n",uplane_mempool_, in_use_buffers_number);
#endif
#endif

							}

							//rte_pktmbuf_free(pkt_chain_head); 	// debugging !!!
							//ret_value = E_OK;      				// debugging !!!

						//dumping the packets
						//rte_pktmbuf_dump (stdout, pkt_chain_head, rte_pktmbuf_pkt_len(pkt_chain_head));

						ResetNewMbufChain(&last_packet_ptr, pkt_eth, pkt_chain_head, layer_num_counter);
						size_of_data_before_send = 0;
					}
					size_of_data_before_send = size_of_data_before_send + size_of_data;
					rte_pktmbuf_chain(pkt_chain_head, single_rb_data_pointers_ptr[rb_num_counter].pkt_data);
				}
				rb_num_counter = rb_num_counter + run_len;
				sqid_ = rb_num_counter;
				run_len_ptr = run_len_ptr + run_len*sizeof(SingleRbDataPointersAndInfo);
				run_len = (*(uint16_t*)(run_len_ptr));
	        }
		}
		if (size_of_data_before_send>0) // check if we have more data to send
		{


					//save the current chain to send in the end of the symbol
					//also update the chain counter
					chain_heads_to_send_[chain_head_counter_]=pkt_chain_head;
					chain_head_counter_++;
if(kDL == direction)
{
#ifdef RU_RT_MODE
#ifdef PRINT_MEMPOOL_LOG
				//uint32_t in_use_buffers_number 		= rte_mempool_in_use_count(uplane_mempool_);
				//printf("UPlane mempool\t\t%llx: in_use_buffers_number=%d\n",uplane_mempool_, in_use_buffers_number);
#endif
#endif

}

				//rte_pktmbuf_free(pkt_chain_head); 	// debugging !!!
				//ret_value = E_OK;      				// debugging !!!
				
			ResetNewMbufChain(&last_packet_ptr, pkt_eth, pkt_chain_head, layer_num_counter);
			size_of_data_before_send = 0;
		}
	}
	sending_loop_on = TRUE;
	while(sending_loop_on)
	{
		if(kDL == direction)
		{

#ifdef PROFILING_TEST
    dl_prof_[0].StopContinue();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].StopContinue();
#endif
    }
		else
		{
#ifdef PROFILING_TEST
    ul_prof_[2].StopContinue();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    ul_symbol_tick_prof_[1].StopContinue();
#endif
		}
		//In the end of the symbol we send all the U-Plane chains from the current symbol
		//callback_ptr_->ITraceData(send_burst_trace_id_, __LINE__, 555);

		uint16_t ret_val=qeue->sendBurst(chain_heads_to_send_, chain_head_counter_, true);

		//callback_ptr_->ITraceData(send_burst_trace_id_, __LINE__, send_burst_trace_id_);

if(kDL == direction)
	{
#ifdef PROFILING_TEST
    dl_prof_[0].Start();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].Start();
#endif

	}
else
{
#ifdef PROFILING_TEST
    ul_prof_[2].Start();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    ul_symbol_tick_prof_[1].Start();
#endif
}
		if(ret_val == chain_head_counter_)
		{
			sending_loop_on = FALSE;
		}
		else
		{
#ifdef RU_RT_MODE
			printf("Could not send %d packets\n",chain_head_counter_-ret_val); // to be replaced with a ERROR LOG !!!
			sending_loop_on = FALSE; // anyway stop the sending loop !!!
#endif
			}
}

	chain_head_counter_=0;
	return ret_value;
}
/******************************************************************
 *@brief  Function Name: SpearteSectionPerPerMtu
 *@brief  Desc:	Separate sections that are bigger than MTU size to different chains.
 *@brief
 *@brief
 *@brief  Input: the relevant section pkt, the last packet in the chain, the chain head, the trailer, layer num and the Q
 *@brief  chain_size - the len of the data we allready have in the chain
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
#if 0
EResultT CUplaneSymbolsHandler::SpearteSectionPerPerMtu(rte_mbuf* pkt_section,rte_mbuf** last_packet_ptr,rte_mbuf*& pkt_chain_head,uint32_t layer_num_counter,EgressQueueAPI* qeue, uint32_t chain_size, DataDirection direction)
{
	EResultT		ret_value		= E_OK;
	uint8_t* packet_data;
	rte_mbuf* pkt_eth;
	rte_mbuf* pkt_send;
	uint32_t data_size_for_send=0;
	uint32_t section_size=pkt_section->data_len; //get section length
	Complex* section_data_ptr = rte_pktmbuf_mtod(pkt_section, Complex*); //get pointer to the start of the data in the section
	Complex* temp_section_data_ptr=section_data_ptr;
    while (section_size>0) //run until we go over on all the section
    {
    	if ((data_size_for_send+chain_size+(NUMBER_SC_IN_RB*sizeof(Complex))+sizeof(U_plane_CPU2FPGA_Headr)  + sizeof(EtherHeadr)  )>mtu_cpu_fpga_size_) //check if we pass the MTU size
    	{
    		//we get to the length we need to send the chain
    		//call to the function that will build a new mbuf with pointer to the relevant data and add it to the chain.
    		BuildNewUplaneDataMbuf(last_packet_ptr, section_data_ptr, pkt_send,data_size_for_send,pkt_chain_head,pkt_section);
    		if(kDL == direction)
    		{
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].StopContinue();
#endif
#ifdef PROFILING_TEST
    dl_prof_[0].StopContinue();
#endif
    			qeue->send(pkt_chain_head, true);
#ifdef PROFILING_TEST
    dl_prof_[0].Start();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].Start();
#endif
    		}
    		else
    			qeue->send(pkt_chain_head, true);
    		section_data_ptr=temp_section_data_ptr;
    		ResetNewMbufChain(last_packet_ptr,pkt_eth,pkt_chain_head,layer_num_counter);
    		data_size_for_send=0;
    		chain_size=0;
       	}
    	else
    	{ //keep to promote the section size and data
    		data_size_for_send=data_size_for_send+(NUMBER_SC_IN_RB*sizeof(Complex));
    		temp_section_data_ptr=temp_section_data_ptr+NUMBER_SC_IN_RB;
    	    section_size=section_size-(NUMBER_SC_IN_RB*sizeof(Complex));
    	}
    }
    if (data_size_for_send>0) //check if we have something more to send
    {
    	BuildNewUplaneDataMbuf(last_packet_ptr, section_data_ptr, pkt_send,data_size_for_send,pkt_chain_head,pkt_section);
    	data_size_for_send=(*last_packet_ptr)->data_len;
    	if(kDL == direction)
    	{
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].StopContinue();
#endif
#ifdef PROFILING_TEST
    dl_prof_[0].StopContinue();
#endif
    		qeue->send(pkt_chain_head, true);
#ifdef PROFILING_TEST
    dl_prof_[0].Start();
#endif
#ifdef STEP_BY_STEP_PROFILING_TEST
    dl_symbol_tick_prof_[3].Start();
#endif
    	}
    	else
    		qeue->send(pkt_chain_head, true);
    }
    rte_pktmbuf_free(pkt_section);
    
return ret_value;
}
#else

EResultT CUplaneSymbolsHandler::SpearteSectionPerPerMtu(rte_mbuf* pkt_section,rte_mbuf** last_packet_ptr,rte_mbuf*& pkt_chain_head,uint32_t layer_num_counter,EgressQueueAPI* qeue, uint32_t chain_size, DataDirection direction)
{
	EResultT                               ret_value                            = E_OK;
	uint8_t* packet_data;
	rte_mbuf* pkt_eth;
	rte_mbuf* pkt_send;
	uint32_t data_size_for_send=0;
	uint32_t section_size=pkt_section->data_len; //get section length
	Complex* section_data_ptr = rte_pktmbuf_mtod(pkt_section, Complex*); //get pointer to the start of the data in the section
	Complex* temp_section_data_ptr=section_data_ptr;
    while (section_size>0) //run until we go over on all the section
    {
    	if ((data_size_for_send+chain_size+(NUMBER_SC_IN_RB*sizeof(Complex))+sizeof(U_plane_CPU2FPGA_Headr)+ sizeof(EtherHeadr))>mtu_cpu_fpga_size_) //check if we pass the MTU size
        {
        	//we get to the length we need to send the chain
            //call to the function that will build a new mbuf with pointer to the relevant data and add it to the chain.
            BuildNewUplaneDataMbuf(last_packet_ptr, section_data_ptr, pkt_send,data_size_for_send,pkt_chain_head,pkt_section);
            //save the current chain to send in the end of the symbol
    		//also update the chain counter
    		chain_heads_to_send_[chain_head_counter_]=pkt_chain_head;
    		chain_head_counter_++;

    		section_data_ptr=temp_section_data_ptr;
            ResetNewMbufChain(last_packet_ptr,pkt_eth,pkt_chain_head,layer_num_counter);
            data_size_for_send=0;
            chain_size=0;

        }
        else
        { //keep to promote the section size and data
        	data_size_for_send=data_size_for_send+(NUMBER_SC_IN_RB*sizeof(Complex));
            temp_section_data_ptr=temp_section_data_ptr+NUMBER_SC_IN_RB;
            section_size=section_size-(NUMBER_SC_IN_RB*sizeof(Complex));
        }
    }
    if (data_size_for_send>0) //check if we have something more to send
    {
    	BuildNewUplaneDataMbuf(last_packet_ptr, section_data_ptr, pkt_send,data_size_for_send,pkt_chain_head,pkt_section);
    	data_size_for_send=(pkt_send)->data_len;
		//save the current chain to send in the end of the symbol
		//also update the chain counter
    	chain_heads_to_send_[chain_head_counter_]=pkt_chain_head;
        chain_head_counter_++;

    }
    rte_pktmbuf_free(pkt_section);
    
	return ret_value;
}

#endif

/******************************************************************
 *@brief  Function Name: createChainEthHead
 *@brief  Desc:	This function create a new mbuf that will point to the EhtHead mbuf and put this mbuf as the head of the chain.
 *@brief
 *@brief
 *@brief  Input: last_packet_ptr- the last packet in the chain, pkt_eth- the header packet and pkt_chain_head the chain head.
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::CreateChainEthHead(rte_mbuf** last_packet_ptr, rte_mbuf* pkt_eth, rte_mbuf*& pkt_chain_head)
{
	EResultT ret_value = E_OK;
	uint8_t* packet_data;

	pkt_chain_head  = rte_pktmbuf_alloc(eth_headers_mempool_);
	//uint16_t free_mbufs =  rte_mempool_avail_count(eth_headers_mempool_);
	//printf("Number of headers free bufs %d\n", free_mbufs);
	ASSERT(pkt_chain_head);
	uint8_t* original_buf_ptr = (uint8_t*)((uint64_t)(pkt_eth->buf_addr) + (uint64_t)(pkt_eth->data_off));
	//get the pinter to the data in the header mbuf
	packet_data = rte_pktmbuf_mtod(pkt_eth, uint8_t*);
	uint32_t section_offset = (uint64_t)packet_data - (uint64_t)original_buf_ptr;
	rte_pktmbuf_attach(pkt_chain_head, pkt_eth);
	if (rte_pktmbuf_adj(pkt_chain_head, section_offset ) == NULL )
    {
    	// if attachment has failed
        rte_pktmbuf_free(pkt_chain_head);
    }
	//put the pointer we got into the new mbuf
	pkt_chain_head->buf_addr=pkt_eth->buf_addr;
    pkt_chain_head->pkt_len = pkt_chain_head->data_len=sizeof(U_plane_CPU2FPGA_Headr)  + sizeof(EtherHeadr) ;
	pkt_chain_head->next = nullptr;
	//update last packet as the head
	*(last_packet_ptr) = pkt_chain_head;
	return ret_value;
}
/******************************************************************
 *@brief  Function Name: ResetNewMbufChain
 *@brief  Desc:	This function reset the chain - create new header tralier mbuf and also an new pionter mbufs.
 *@brief
 *@brief
 *@brief  Input: last_packet_ptr- the last packet in the chain, pkt_eth- the header packet and pkt_chain_head the chain head, also get layer num.
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::ResetNewMbufChain(rte_mbuf** last_packet_ptr, rte_mbuf* pkt_eth, rte_mbuf*& pkt_chain_head, uint32_t layer_num_counter)
{
	EResultT		ret_value		= E_OK;
	CreateMbufEth(pkt_chain_head, layer_num_counter);
	*last_packet_ptr=pkt_eth;
	//pkt_chain_head=pkt_eth;
	return ret_value;
}

/******************************************************************
 *@brief  Function Name: BuildNewUplaneDataMbuf
 *@brief  Desc:	This function create a new mbuf that will point IQ samples inside the section.
 *@brief
 *@brief
 *@brief  Input: last_packet_ptr- the last packet in the chain, pkt_send- pointer to the new packet we need to create, data_size_for_send- the size of the data
 *@brief  start_data_ptr- pointer to the start of the data we need to put in the new mbuf.
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::BuildNewUplaneDataMbuf(rte_mbuf** last_packet_ptr, Complex* start_data_ptr, rte_mbuf*&pkt_send,uint32_t data_size_for_send, rte_mbuf* pkt_chain_head,rte_mbuf* pkt_section)
{
	EResultT		ret_value		= E_OK;
	uint8_t* buff_data;
	//	Get free mbuf from ring:

	uint32_t retVal = rte_ring_dequeue(uplane_mbufs_preallocated_ring_, (void **)&pkt_send);
	ASSERT(retVal==0);
	//clean the mbuf
	pkt_send->next=nullptr;
	pkt_send->nb_segs=1;
	pkt_send->data_off=0;
	pkt_send->ol_flags=0;
	pkt_send->buf_len=0;
	pkt_send->refcnt=1;

	// increment refcnt of the mbuf for using it again without allocation from mempool, but taking it from our preallocated Ring array
	pkt_send->refcnt++;
	//need to free the next, beacuse the eth header is part of a chain, and when we take this mbuf again we need that
	// the mbuf will be clean.
	 // Enqueue the mbuf ptr back to the ring for next usage.
    // We expect that the Ring will be big enough so that when we will use this mbuf again
	// it already should be released by the DPDK Offline Driver (in case of Offline run) or by HW (in case of Online DPDK Driver usage on Send)
	retVal = rte_ring_enqueue(uplane_mbufs_preallocated_ring_, (void *)(pkt_send));
	ASSERT(retVal==0);

  	//put the pointer to the data in the new mbudf
	uint8_t* original_buf_ptr = (uint8_t*)((uint64_t)(pkt_section->buf_addr) + (uint64_t)(pkt_section->data_off)); // start of the input data
	uint32_t section_offset = (uint64_t)start_data_ptr - (uint64_t)original_buf_ptr;
	rte_pktmbuf_attach(pkt_send, pkt_section);
	if (rte_pktmbuf_adj(pkt_send, section_offset ) == NULL )
	{
		// if attachment has failed
		rte_pktmbuf_free(pkt_send);
	}
	buff_data = reinterpret_cast<uint8_t*>(pkt_section->buf_addr);
	pkt_send->buf_addr = static_cast<void*>(buff_data);
	pkt_send->pkt_len = pkt_send->data_len=data_size_for_send; //update the data len
	//update new mbuf data length and pointer to the data
	int32_t Block_size_bytes = mtu_cpu_fpga_size_-(sizeof(U_plane_CPU2FPGA_Headr)  + sizeof(EtherHeadr)); //max block size
	int32_t    trim_size = ( ((int32_t)pkt_send->data_len - (int32_t)Block_size_bytes) > 0 ) ? pkt_send->data_len - Block_size_bytes :0;
	// Remove len bytes of data at the end of the mbuf - to fit one MTU block size.
	if (rte_pktmbuf_trim(pkt_send, trim_size) != 0 )
	{
		printf("failed to trim \n");
	}
	rte_pktmbuf_chain(pkt_chain_head, pkt_send);
  	return ret_value;

}


/******************************************************************
 *@brief  Function Name: AddToMbufChain
 *@brief  Desc:	This function add new mbuf in the end of the chain
 *@brief
 *@brief
 *@brief  Input: last_packet_ptr- the last packet in the chain and new node - the mbuf we need to add to the chain.
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::AddToMbufChain(rte_mbuf *new_node,rte_mbuf** last_packet_ptr)
{
	EResultT		ret_value		= E_OK;
	(*last_packet_ptr)->next = new_node;
	new_node->next = nullptr;
	*last_packet_ptr = new_node;
	return ret_value;
}


/******************************************************************
 *@brief  Function Name: PrepareHeaders
 *@brief  Desc: Prepare Uplane CPU to FPGA Ethernet Header
 *@brief
 *@brief
 *@brief  Input:
 *@brief  Output:
 *@brief
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::PrepareHeaders(rte_mbuf* pkt_eth, uint32_t layer_num)
{
	//===== Ethernet Header =======
	//get a pointer to mbuf data
	EtherHeadr *data_in_mbuf_EtherHeadr_ptr = rte_pktmbuf_mtod(pkt_eth, EtherHeadr*);
    //need to update only layer num - all the other data update done in the start of the run.
	data_in_mbuf_EtherHeadr_ptr->d_addr.addr_bytes[LID_BYTE_INDEX] = layer_num & FIVE_BIT_MASK;           // 5 bits

	
	//==== FPGA Header ========
	// according to 10Ge Back-haul doc (https://confluence.airspan.com/display/FPGA/Phase+1+-+10Ge+Back-haul)
	U_plane_CPU2FPGA_Headr  *data_in_mbuf_FPGAHeadr_ptr = rte_pktmbuf_mtod_offset(pkt_eth, U_plane_CPU2FPGA_Headr*,sizeof(EtherHeadr));

	data_in_mbuf_FPGAHeadr_ptr->U_plane_CPU2FPGA.Eth_SID  = current_active_symbol_RT_ & FIVE_BIT_MASK;          // 5 bits
	data_in_mbuf_FPGAHeadr_ptr->U_plane_CPU2FPGA.Eth_RSV  = 0;         									 // 2 bits
	data_in_mbuf_FPGAHeadr_ptr->U_plane_CPU2FPGA.Eth_SQID = sqid_ & NINE_BIT_MASK;     				    // 9 bits

#if (CPU_BYTE_ORDER == CPU_LITTLE_ENDIAN)
	data_in_mbuf_FPGAHeadr_ptr->Uplane_hdr = BYTE_SWAPPING_16(data_in_mbuf_FPGAHeadr_ptr->Uplane_hdr);
#endif
	pkt_eth->pkt_len = pkt_eth->data_len=sizeof(U_plane_CPU2FPGA_Headr) + sizeof(EtherHeadr);
	
	return E_OK;
}


/******************************************************************
 *@brief  Function Name: CreateMbufEth
 *@brief  Desc:	This function  take mbuf from mbuf ring and update header data mbuf
 *@brief
 *@brief
 *@brief  Input: layer_num_counter, pkt_eth, pkt_trailer
 *@brief  Output:ret_value
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::CreateMbufEth(rte_mbuf*& pkt_eth,uint32_t layer_num_counter)
{

	//take mbuf from ring
	uint32_t retVal = rte_ring_dequeue(eth_haders_mbufs_preallocated_ring_, (void **)&pkt_eth);
	ASSERT(retVal==0)

	//clean mbuf before reuse
	pkt_eth->next=nullptr;
	pkt_eth->nb_segs=1;
#ifdef USE_NXP_PATCH_FOR_CHAIN_USAGE_OPTIMIZATION // this patch is not working without the following reset mbuf call	
	rte_pktmbuf_reset(pkt_eth);
#endif
	// increment refcnt of the mbuf for using it again without allocation from mempool, but taking it from our preallocated Ring array
	pkt_eth->refcnt++;
	//need to free the next, beacuse the eth header is part of a chain, and when we take this mbuf again we need that
	// the mbuf will be clean.
	// Enqueue the mbuf ptr back to the ring for next usage.
    // We expect that the Ring will be big enough so that when we will use this mbuf again
	// it already should be released by the DPDK Offline Driver (in case of Offline run) or by HW (in case of Online DPDK Driver usage on Send)
    retVal = rte_ring_enqueue(eth_haders_mbufs_preallocated_ring_, (void *)(pkt_eth));
    ASSERT(retVal==0)


#ifdef USE_NXP_PATCH_FOR_CHAIN_USAGE_OPTIMIZATION

	//uint16_t defined_headroom_size 	= RTE_PKTMBUF_HEADROOM;
	//uint16_t room_size 		= rte_pktmbuf_data_room_size (eth_headers_mempool_);
	//uint16_t headroom_size 	= rte_pktmbuf_headroom (pkt_eth);
	//uint16_t tailroom_size 	= rte_pktmbuf_tailroom (pkt_eth);


	// move data pointer after the head room for allowing the chaining
	pkt_eth->data_len = UPLANE_DL_ETH_HEADRS_BUFF_SIZE - RTE_PKTMBUF_HEADROOM; // predefine data length in order to be able to adjust bellow. Further this field should be updated properly
	rte_pktmbuf_adj(pkt_eth, (HEADER_SIZE_FOR_CHAINING-RTE_PKTMBUF_HEADROOM) );

	//room_size 		= rte_pktmbuf_data_room_size (eth_headers_mempool_);
	//headroom_size 	= rte_pktmbuf_headroom (pkt_eth);
	//tailroom_size 	= rte_pktmbuf_tailroom	(pkt_eth);
	//uint8_t* buf_ptr	= (uint8_t* )pkt_eth->buf_addr;
	//uint16_t buf_offset	= pkt_eth->data_off;
#endif

	PrepareHeaders(pkt_eth, layer_num_counter);

	return E_OK;
}


/******************************************************************
 *@brief  Function Name: sendJobDescriptorToBfDe
 *@brief  Desc:	Sends current Symbol's descriptor (that its rcv window has been closed now) to BF DE.
 *@brief		The descriptor should include 2 pointers: C-Plane table of BeamIds for the given Symbol and its U-Plane data pointers.
 *@brief		The same descriptor should be sent to all defined BF DEs
 *@brief
 *@brief  Input:	symbol_obj_ptr - pointer to Symbol's object that sends the Descriptor
 *@brief  			de_fifo_pointer - pointer to the DE Job Descriptors FIFO to which we send the descriptor
 *@brief
 *@brief  Output:
 *@brief
 *******************************************************************/
// We don't need symbol JD to FPGA anymore
#if 0
EResultT CUplaneSymbolsHandler::sendJobDescriptorToBfDe(uplane_symbols_api* symbol_obj_ptr,
		CMemArea* de_fifo_pointer)
{
	EResultT				ret_value						= E_OK;
	DataEngineFifoItem*		bf_job_descriptor_ptr			= nullptr;
	SymbolRbPointers*		symbol_rb_data_pointers_table	= nullptr;


  	bf_job_descriptor_ptr = (DataEngineFifoItem*)de_fifo_pointer->AllocFIFO();


  	bf_job_descriptor_ptr->data_engine_item_type_ = kBeamformingDataEngine;;

	uint32_t symbol_number = symbol_obj_ptr->getSymbolNumber();
	bf_job_descriptor_ptr->bf_job_desc_.bf_weights_indices_table_index_ = symbol_number & 0x0001; //toggle double-buffer symbol index of FPGA memory according to Symbol number

	symbol_rb_data_pointers_table = symbol_obj_ptr->getRbsDataPointersTable();
	bf_job_descriptor_ptr->bf_job_desc_.symbol_rbs_data_ptrs_ = symbol_rb_data_pointers_table;

	void* push_de_fifo_ret_ptr;
	push_de_fifo_ret_ptr = de_fifo_pointer->PushFIFO();
	ASSERT(push_de_fifo_ret_ptr); // validate non-zero pointer returned - push succeeded

	return ret_value;
}
#endif


////////////////////////////////////////////////////////////////////////////////////////
//										DU
////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************
 *@brief	Initialization for U-Plane Symbols Handler,e.g.
 *@brief	Memory initialization, etc.
 *@brief
 *@brief	Input:	data_engines_jobdescriptors_fifo_arr - pointer to an
 *@brief				array of DataEngine JDs FIFO.
 *@brief			callback_ptr - callback pointer
 *@brief
 *@brief	Output:	EResultT
 *@brief
 *******************************************************************/
EResultT CUplaneSymbolsHandler::InitDu()
{
	EResultT	ret_value = E_OK;
	uint32_t left_receive_window_length_;
	int32_t symb_nums[uplane_symbols_pool_capacity_];

    // Initialize all the Symbols from the Pool, except for the last one
    for(int32_t pool_symbol_idx = 0; pool_symbol_idx < uplane_symbols_pool_capacity_ - 1; pool_symbol_idx++)
    {
    	symb_nums[pool_symbol_idx] = pool_symbol_idx;
    	left_receive_window_length_ = pool_symbol_idx + 1;
    	dl_symbol_[pool_symbol_idx].InitSymbol(symb_nums[pool_symbol_idx], left_receive_window_length_);

       	// set proper table entry for the new Symbol
    	setSymbolObjectPtr(pool_symbol_idx, static_cast<uplane_symbols_api*>(&dl_symbol_[pool_symbol_idx]));
    }
    // Initialize the last symbol as Free
    dl_symbol_[uplane_symbols_pool_capacity_ - 1].ResetSymbol();

    last_opened_symbol_number_	= uplane_symbols_pool_capacity_ - 2;

	return ret_value;
}

void CUplaneSymbolsHandler::InitZerosVector(Complex zeors_vector[], uint32_t size_of_zeros_data)
{

     for(int32_t Size_count=0 ; Size_count < size_of_zeros_data ; Size_count++)
	{
    	 zeors_vector[Size_count].re=0;
    	 zeors_vector[Size_count].im =0;
	}
}


class CUplaneSymbolsHandler  *g_module = NULL;

//API function to connect with ITarget API
extern "C" uint32_t IGetConnectAPI(void **target_ptr)
{
	g_module = new CUplaneSymbolsHandler();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module);
	return 0;

}



