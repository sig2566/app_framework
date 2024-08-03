/*
 * i_du_asfh_adapters_factory_api.h
 *
 *  Created on: September 2, 2020
 *      Author: E. Maidman
 */

#ifndef API_MODULE_API_I_DU_ASFH_ADAPTER_FACTORY_API_H_
#define API_MODULE_API_I_DU_ASFH_ADAPTER_FACTORY_API_H_

#include "i_api_common.h"

// Module name
#define DU_ASFH_ADAPTERS_FACTORY_MODULE	"DU_ASFH_ADAPTERS_FACTORY"

#define		MAX_NUM_ADAPTERS_PER_CELL					(2) // number of adapters per Cell can be more than 1 (it might be that we'd like to activate more than 1 Worker Thread/ASFH Adpater per Cell)
#define		MAX_DU_ASFH_ADAPTERS_NUMBER					(3*MAX_NUM_ADAPTERS_PER_CELL) // 3 stands for 3 Cells support (should be dependent on global definition of MAX_NUM_OF_5G_CELLS !!!!)


struct DuAsfhAdapterInputConfig
{
	uint32_t	cell_id;
	uint32_t	adapter_instance_in_cell;
	uint32_t	adapter_instance_in_factory;
	uint8_t		dl_layers_mask;
	uint8_t		ul_layers_mask;
};

/******************************************************************************************//*!
 *@class Du_Asfh_Adapters_Factory_API
 *@brief The purpose of this class is :
 *@brief This class expose the DU ASFH Adapters Factory Module control API.
  *********************************************************************************************/
struct Du_Asfh_Adapters_Factory_API
{
	//virtual void		  Reset() = 0;
};

/*******************************************************************************************//*!
*@class DuAsfhAdapter_API
*@brief The purpose of this class is :
*@brief Interface for DPDK Buffer Manager
*@brief
***********************************************************************************************/
struct DuAsfhAdapter_API
{
	// creates and initiates
	virtual EResultT Init(DuAsfhAdapterInputConfig*	adapter_input_cfg) = 0;
	virtual EResultT DataChannelDlControlHandler(	void**		control_jds_arr,
													uint8_t		dl_layers_mask,
													int8_t**	out_symbol_control_msgs) = 0;

	virtual EResultT DataChannelDlSymbolsHandler(	void**		symbol_sections_arr,
													uint8_t		dl_layers_mask,
													int8_t**	out_symbol_msgs) = 0;
} ;



#endif /* API_MODULE_API_I_DU_ASFH_ADAPTER_FACTORY_API_H_ */
