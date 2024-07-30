/******************************************************************
 * i_dpdk_classify_api.h
 * Author: Evgeniy Sokolov
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#ifndef API_MODULE_DPDK_CLASSIFY_API_H_
#define API_MODULE_DPDK_CLASSIFY_API_H_

#include <rte_mbuf.h>
#include <rte_ether.h>

#define DPDK_CLASSIFY_MODULE "DPDK_CLASSIFY"

/*******************************************************************************************//*!
     *@class DpdkClassifyAPI
     *@brief The purpose of this class is :
     *@brief Classification of network packets 
***********************************************************************************************/

struct DpdkClassifyAPI
{
  // Clasiify network packet
  // \param[in] packet       Pointer to mbuf which contains packet
  // \return E_OK or E_FAIL
  virtual EResultT Configure  (ether_addr *mac) = 0;


  // Clasiify network packet
  // \param[in] packet       Pointer to mbuf which contains packet
  // \return Classififation result
  virtual uint16_t Classify  (const void *packet) = 0;
};

#endif /* API_MODULE_DPDK_CLASSIFY_API_H_ */
