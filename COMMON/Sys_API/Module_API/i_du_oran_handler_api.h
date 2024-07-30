/*
 * i_du_oran_handler_api.h
 *
 *  Created on: July 10, 2019
 *      Authors: E. Maidman and Niv Arad
 */

#ifndef API_MODULE_API_I_DU_ORAN_HANDLER_API_H_
#define API_MODULE_API_I_DU_ORAN_HANDLER_API_H_

#include "i_split_api.h"

// Name of input FIFO for Handler
#define DU_ORAN_HANDLER_MODULE	"DU_ORAN_HANDLER"
/******************************************************************************************//*!
 *@class DuOranHandlerAPI
 *@brief The purpose of this class is :
 *@brief This class expose the DU O-RAN handler API.
  *********************************************************************************************/
struct DuOranHandlerAPI
{
	virtual EResultT	InputPacketPlaneClassifier(uint8_t* input_packet, uint32_t input_packet_size) = 0;
	virtual EResultT	RU_Userplane_Message_Handler_Ul(MESSAGE_DESCRIPTOR* du_message_desc, DATA_DESCRIPTOR* du_message_data) = 0;
	virtual EResultT	dataMsgProcess(MESSAGE_DESCRIPTOR* pMessageDesc, DATA_DESCRIPTOR* pMessageData) = 0;
	virtual int8_t		SlotTick() = 0;
	virtual void		SymbolTick(DataDirection direction) = 0;
	virtual void		SymbolWindowOpenTick(DataDirection direction) = 0;
	virtual void 		SymbolWindowCloseTick(DataDirection direction) = 0;

    //GLConfig configures the gNodeB Glue Logic. It is called at the system start or after GLStop.
    virtual EResultT	GLStop() = 0;
};


// Memories definitions

#define INPUT_USER_PLANE_PACKETS_MEM	"INPUT_USER_PLANE_PACKETS_MEMORY"

#endif /* API_MODULE_API_I_DU_ORAN_HANDLER_API_H_ */
