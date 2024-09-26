/******************************************************************
 * tcp_ip_driver.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include <unistd.h>
#include <sys/syscall.h>

#include <rte_malloc.h>
#include <rte_ethdev.h>
#include "dpdk_eth_driver.h"
#include "as_infra_envdep.h"
#include "generic/rte_cycles.h"

static const char *RX_Q_MEMPOOL_NAME_TEMPLATE = "MBUF_POOL_%d_%d";

extern char *program_invocation_name;

// Receive packet (get packet from Rx ring queue if RxThread is running)
// \param[out] pkt              Will be filled with Pointer to incoming packet.
//                              AS_INFRA_NULL if no packet received.
//                              Use rte_pktmbuf_free(pkt) to free the pkt after processing.
// \return EResultT
EResultT ClassifiedQueue::recv(rte_mbuf *&pkt)
{
  if (Parent->IsSwClassifier())
    return (rte_ring_dequeue(intermediateRing_, (void **)&pkt) == 0) ? E_OK : E_FAIL;
  else
    return (Parent->recv(QueueId, pkt) == E_OK) ? E_OK : E_FAIL;
}

// Send burts of packet
// \param[in] tx_pkts           mbuf pointers array to receive
// \param[in] count             mbufs count to receive
// \param[in] free_unsent       Frees unsent mbuf
// \return Number of received mbufs
uint16_t ClassifiedQueue::recvBurst(rte_mbuf **rx_pkts, uint16_t count)
{

  if (Parent->IsSwClassifier())
    return rte_ring_dequeue_burst(intermediateRing_, (void **)rx_pkts, count, NULL);
  else
  {
    uint16_t RetVal;
    RetVal = Parent->recvBurst(QueueId, rx_pkts, count);
    return RetVal;
  }
}

//EgressQueue methods
// Send packet (put packet to Tx ring queue if TxThread is running)
// \param[in] pkt               mbuf to send. The pkt is freed by send() for sent packets (if E_OK)
// \param[in] free_unsent       Frees unsent mbuf
// \return EResultT
EResultT EgressQueue::send(rte_mbuf *pkt, bool free_unsent)
{
  return (Parent->send(QueueId, pkt, free_unsent) == E_OK) ? E_OK : E_FAIL;
}

// Send burts of packet
// \param[in] tx_pkts           mbuf pointers array to send. The *tx_pkts is freed by send() for sent packets
// \param[in] count             mbufs count to send
// \param[in] free_unsent       Frees unsent mbuf
// \return Number of sent mbufs
uint16_t EgressQueue::sendBurst(rte_mbuf **tx_pkts, uint16_t count,
                                bool free_unsent)
{
  return Parent->sendBurst(QueueId, tx_pkts, count, free_unsent);
}

//DPDK device API
// Open port by port_id
// \param[in] port_id       Port id to initialize
// \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
// \return EResultT
EResultT CDPDK_Eth_driver::open(uint16_t port_id,
                                eth_dev_settings_t *dev_settings)
{
	int32_t rez = 0;

  int ports_num = rte_eth_dev_count_avail();
  if (port_id >= ports_num)
  {
    printf("none DPDK port is available port_id=%d, ports_num=%d\n", port_id, ports_num);
	LOG(E_HIGH, "%s:%d: E_FAIL: none DPDK port is available\n", __func__, __LINE__);
    return E_FAIL;
  }

  num_packets_sent_= 0;
  
  // Stop the port before we reinitialize (in case they are active)	
  rte_eth_dev_stop(port_id);

  if (dev_settings)
  {
    m_settings = *dev_settings;
  }

  rte_eth_conf port_conf = {};
  port_conf.rxmode.max_rx_pkt_len = m_settings.max_rx_pkt_len;

  /*
    if (m_settings.max_rx_pkt_len > ETHER_MAX_LEN)
    {
        port_conf.rxmode.jumbo_frame = 1;
    }
    else
    {
        port_conf.rxmode.jumbo_frame = 0;
    }
  */

  port_conf.link_speeds = m_settings.link_speeds;
  port_conf.txmode.mq_mode = m_settings.mq_mode_tx;
  port_conf.txmode.offloads |= m_settings.tx_offloads;
  port_conf.rxmode.mq_mode = m_settings.mq_mode_rx;
  port_conf.rx_adv_conf.rss_conf = m_settings.rss_conf;
  // rez = rte_eth_dev_rss_hash_update(port->port_id, &m_settings.rss_conf);

  // Configure the Ethernet device.
  rez = rte_eth_dev_configure(port_id, m_settings.hw_rx_queues, m_settings.hw_tx_queues, &port_conf);
  if (rez != 0)
  {
    LOG(E_HIGH, "%s:%d: E_FAIL\n", __func__, __LINE__);
    return E_FAIL;
  }

//  //set link flow control
//  // first get the current FC settings of the port
//  rte_eth_fc_conf    flow_control;
//  rez = rte_eth_dev_flow_ctrl_get(port_id, &flow_control);
//
//  m_settings.flow_control = flow_control;    // copy the default definitions of the Port in terms of FC
//  m_settings.flow_control.mode = RTE_FC_FULL; //RTE_FC_NONE; //RTE_FC_TX_PAUSE; //overwrite with the required mode
//  rez = rte_eth_dev_flow_ctrl_set(port_id, &m_settings.flow_control);
//  if (rez != 0)
//  {
//    LOG(E_HIGH, "%s:%d: E_FAIL\n", __func__, __LINE__);
//    //return E_FAIL;
//  }

  // Check that numbers of Rx and Tx descriptors satisfy descriptors limits from the Ethernet device information, otherwise adjust them to boundaries.
  rez = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &m_settings.hw_rx_queue_size, &m_settings.hw_tx_queue_size);
  if (rez != 0)
  {
    LOG(E_HIGH, "%s:%d: E_FAIL\n", __func__, __LINE__);
    return E_FAIL;
  }

  // Create mbuf mempool per queue
  m_mbuf_pool = new rte_mempool *[m_settings.hw_rx_queue_size];

  for (uint16_t q = 0; q < m_settings.hw_rx_queues; q++)
  {
    char msg_pool_name[32] = {};
    sprintf(msg_pool_name, RX_Q_MEMPOOL_NAME_TEMPLATE, port_id, q);
    if (rte_eal_process_type() == RTE_PROC_PRIMARY)
    {
      // Creates a new mempool in memory to hold the mbufs
      m_mbuf_pool[q] = rte_pktmbuf_pool_create(msg_pool_name, m_settings.num_mbufs, m_settings.mbuf_cache_size, 0, m_settings.mbuf_data_room_size, rte_socket_id());
    }
    else
    {
      m_mbuf_pool[q] = rte_mempool_lookup(msg_pool_name);
    }

    if (!m_mbuf_pool[q])
    {
      LOG(E_HIGH, "%s:%d: m_mbuf_pool <%p> rte_errno <%d>\n", __func__, __LINE__, m_mbuf_pool[q], rte_errno);
      return E_FAIL;
    }

    // Allocate and set up RX queue
    rez = rte_eth_rx_queue_setup(port_id, q, m_settings.hw_rx_queue_size, rte_eth_dev_socket_id(port_id), AS_INFRA_NULL, m_mbuf_pool[q]);
    if (rez < 0)
    {
      return E_FAIL;
    }
  }

  // Allocate and set up 1 TX queue per Ethernet port_id.
  for (uint16_t q = 0; q < m_settings.hw_tx_queues; q++)
  {
    rez = rte_eth_tx_queue_setup(port_id, q, m_settings.hw_tx_queue_size, rte_eth_dev_socket_id(port_id), AS_INFRA_NULL);
    if (rez < 0)
    {
      LOG(E_HIGH, "%s:%d: E_FAIL\n", __func__, __LINE__);
      return E_FAIL;
    }
  }

  rte_eth_stats_reset(port_id);

  // Start the Ethernet port_id.
  rez = rte_eth_dev_start(port_id);
  if (rez < 0)
  {
    LOG(E_HIGH, "%s:%d: E_FAIL\n", __func__, __LINE__);
    return E_FAIL;
  }

  // Enable RX in promiscuous mode for the Ethernet device
  if (m_settings.eth_promiscuous_enable)
  {
    rte_eth_promiscuous_enable(port_id);
  }

  m_port_id = port_id;
  m_bInited = true;

  FillQueues();
  return E_OK;
}

// Open port by MAC
// \param[in] mac           MAC address to initialize
// \param[in] dev_settings  Pointer to Ethernet device settings structure. If AS_INFRA_NULL using defaults
// \return EResultT
EResultT CDPDK_Eth_driver::open(const uint8_t mac[ETHER_ADDR_LEN],
                                eth_dev_settings_t *dev_settings)
{
  EResultT ret = E_FAIL;
  uint16_t port_id = 0;

  ret = getPortIdByMac(mac, port_id);

  if (ret == E_OK)
  {
    ret = open(port_id, dev_settings);
  }
  return ret;
}

// Close port
// \return EResultT
EResultT CDPDK_Eth_driver::close()
{
  //remove array of pointers to membufs
  if (classifier_pkts_)
  {
    rte_free(classifier_pkts_);
    classifier_pkts_ = NULL;
  }
  //remove rings
  for (uint8_t r_num = INGRESS_NON_CLASSIFIED_Q; r_num < INGRESS_LAST_CLASSIFIED_QUEUE; ++r_num)
  {
    char RingName[20];
    sprintf(RingName, "TxRing%d", r_num);
    if (rx_queues_[r_num].intermediateRing_ != NULL)
    {
      rte_ring_free(rx_queues_[r_num].intermediateRing_);
      rx_queues_[r_num].intermediateRing_ = NULL;
    }
  }

  if (m_bInited)
  {
    rte_eth_dev_stop(m_port_id);
    rte_eth_dev_close(m_port_id);

    if (m_mbuf_pool)
    {
      for (uint16_t q = 0; q < m_settings.hw_rx_queues; q++)
      {
        rte_mempool_free(m_mbuf_pool[q]);
      }
      delete[] m_mbuf_pool;
      m_mbuf_pool = AS_INFRA_NULL;
    }

    m_bInited = false;
    return E_OK;
  }
  return E_FAIL;
}

// Check if ETH_LINK UP
// \return true if link up
bool CDPDK_Eth_driver::isLinkUp()
{
  bool b_link_up = false;
  rte_eth_link link = {};
  rte_eth_link_get_nowait(m_port_id, &link);
  if (link.link_status)
  {
    b_link_up = true;
  }
  return b_link_up;
}

// Busy wait till ETH_LINK UP using timeout
// \param[in] timeout_ms        Time to wait (ms)
// \param[in] check_period_ms   Check interval (ms)
// \return true if link up
bool CDPDK_Eth_driver::waitLinkUp(uint32_t timeout_ms,
                                  uint32_t check_period_ms)
{
  rte_eth_link link = {};
  uint64_t timer_start = rte_get_timer_cycles();
  uint64_t timer_ms = rte_get_timer_hz() / 1000;

  while (!link.link_status || ((rte_get_timer_cycles() - timer_start) / timer_ms) >= timeout_ms)
  {
    usleep(check_period_ms);
    rte_eth_link_get_nowait(m_port_id, &link);
  }
  return (bool)link.link_status > 0;
}

// Assign a software classifier to eternet device
// If software classifier is used, this function should be called before open device
// \param[in] classifier_api    Pointer to classifier interface class
// \return EResult
EResultT CDPDK_Eth_driver::assignSoftwareClassifier(DpdkClassifyAPI *classifier_api, uint16_t classifier_burst_size)
{
  //allocate array of pointers to packet
  classifier_pkts_ = (rte_mbuf **)rte_zmalloc(NULL, sizeof(struct rte_mbuf *) * classifier_burst_size, 0);
  if (classifier_pkts_ == 0)
  {
    printf("Could not allocate rte_mbuf array\n");
    ASSERT(0);
  }

  classifier_api_ = classifier_api;
  classifier_burst_size_ = classifier_burst_size;
  //creating rings to store classified packets
  for (uint8_t r_num = INGRESS_NON_CLASSIFIED_Q; r_num < INGRESS_LAST_CLASSIFIED_QUEUE; ++r_num)
  {
    char RingName[20];
    sprintf(RingName, "TxRing%d", r_num);
    rx_queues_[r_num].intermediateRing_ = rte_ring_create(RingName, RINGS_SIZE, SOCKET_ID_ANY, RING_F_SP_ENQ | RING_F_SC_DEQ);
    if (rx_queues_[r_num].intermediateRing_ == NULL)
    {
      printf("Could not create ring\n");
      ASSERT(0);
    }
  }

  return E_OK;
}

// Performs reading and classification packets from separate thread
// \return EResultT
EResultT CDPDK_Eth_driver::readAndClassification()
{
  if (classifier_api_ != NULL) //method works only if sowtware classifier was assigned
  {
    //read burst of packets
    uint16_t count = recvBurst(INGRESS_NON_CLASSIFIED_Q, classifier_pkts_, classifier_burst_size_);
    //classify each packet and send it to ring according classification
    for (uint16_t i = 0; i < count; ++i)
    {
      const void *data = (void *)(((char *)classifier_pkts_[i]->buf_addr) + classifier_pkts_[i]->data_off);
      uint16_t packetType = classifier_api_->Classify(data);
      if (packetType >= INGRESS_LAST_CLASSIFIED_QUEUE)
      {
        printf("Invalid classification rules\n");
        ASSERT(0);
      }
      if (rte_ring_enqueue(rx_queues_[packetType].intermediateRing_, (void *)(classifier_pkts_[i])) != 0)
      {
        printf("Could not enqueue packet to ring. Packet dropped\n");
      }
    }
  }
  return E_OK;
}

// get Port id
uint16_t CDPDK_Eth_driver::getPortId()
{
  return m_port_id;
}

//This function returns the pointer to the corresponding classifier MBUF queue handler. NULL if it does not exist
IngressClassifiedQueueAPI *CDPDK_Eth_driver::GetIngressClassifiedQueue(INGRESS_CLASSIFICATOR_Q_TYPE classified_q_val)
{
  ASSERT(classified_q_val < INGRESS_LAST_CLASSIFIED_QUEUE);
  return static_cast<IngressClassifiedQueueAPI *>(&rx_queues_[classified_q_val]);
}

//This function returns the pointer to the corresponding classifier MBUF Egress queue handler.
//NULL if it does not exist
EgressQueueAPI *CDPDK_Eth_driver::GetEgressQueue(EGRESS_Q_TYPE egress_q_val)
{
  ASSERT(egress_q_val < LAST_EGRESS_QUEUE);
  return static_cast<EgressQueueAPI *>(&tx_queues_[egress_q_val]);
}

//Local methods
void CDPDK_Eth_driver::Reset()
{
}

void CDPDK_Eth_driver::FillQueues()
{
  for (uint8_t QueueId = INGRESS_NON_CLASSIFIED_Q; QueueId < INGRESS_LAST_CLASSIFIED_QUEUE; ++QueueId)
    rx_queues_[QueueId].Init(this, callback_ptr_, (INGRESS_CLASSIFICATOR_Q_TYPE)(QueueId));

  for (uint8_t QueueId = 0; QueueId < LAST_EGRESS_QUEUE; ++QueueId)
    tx_queues_[QueueId].Init(this, callback_ptr_, (EGRESS_Q_TYPE)(EGRESS_UNIFIED_QUEUE + QueueId));
}

EResultT CDPDK_Eth_driver::send(uint16_t queue_id, rte_mbuf *pkt, bool free_unsent)
{
  uint16_t sent_pkts = 0;

  if (!pkt)
  {
    return E_FAIL;
  }

  if (!m_bInited)
  {
    goto done;
  }

  if (queue_id > m_settings.hw_tx_queues - 1)
  {
    goto done;
  }
#if(0)
  timespec system_time;
  uint64_t time_in_nsec;
  clock_gettime(CLOCK_MONOTONIC, &system_time);
  time_in_nsec = (uint64_t)system_time.tv_sec*BILLION +  (uint64_t)system_time.tv_nsec;
  printf("New packet #%d is sent ... Data length=%d, Packet length = %d, time: %llu\n", num_packets_sent_, pkt->data_len, pkt->pkt_len, time_in_nsec);
  num_packets_sent_++;
#endif
  sent_pkts = rte_eth_tx_burst(m_port_id, queue_id, &pkt, 1);

done:
  if (sent_pkts != 1)
  {
    if (pkt && free_unsent)
      rte_pktmbuf_free(pkt);
    return E_FAIL;
  }

  return E_OK;
}

uint16_t CDPDK_Eth_driver::sendBurst(uint16_t queue_id, rte_mbuf **tx_pkts, uint16_t count, bool free_unsent /* = true */)
{
  uint16_t sent_pkts = 0;

  if (!tx_pkts)
  {
    return 0;
  }

  if (!m_bInited)
  {
    goto done;
  }

  if (queue_id > m_settings.hw_tx_queues - 1)
  {
    goto done;
  }

  sent_pkts = rte_eth_tx_burst(m_port_id, queue_id, tx_pkts, count);

  if (sent_pkts != count)
  {
    //LOG(E_LOW, "sent_pkts<%u> != count<%u>\n", sent_pkts, count);
    goto done;
  }
done:
  if (free_unsent)
  {
    // free unsent mbufs
    for (uint16_t i = sent_pkts; i < count; i++)
    {
      if (tx_pkts[i])
      {
        rte_pktmbuf_free(tx_pkts[i]);
      }
    }
  }
  return sent_pkts;
}

EResultT CDPDK_Eth_driver::recv(uint16_t queue_id, rte_mbuf *&pkt)
{
  if (!m_bInited)
  {
    return E_FAIL;
  }

  if (queue_id > m_settings.hw_rx_queues - 1)
  {
    return E_FAIL;
  }

  uint16_t rx_packets = rte_eth_rx_burst(m_port_id, queue_id, &pkt, 1); // receive 1 packet - does not work with igb_uio (min read 32 packets)
  if (rx_packets == 0)
  {
    pkt = AS_INFRA_NULL;
  }
  return E_OK;
}

uint16_t CDPDK_Eth_driver::recvBurst(uint16_t queue_id, rte_mbuf **rx_pkts, uint16_t count)
{
  if (!m_bInited)
  {
    return 0;
  }

  if (queue_id > m_settings.hw_rx_queues - 1)
  {
    return 0;
  }

  return rte_eth_rx_burst(m_port_id, queue_id, rx_pkts, count);
}

EResultT CDPDK_Eth_driver::getPortIdByMac(const uint8_t mac[ETHER_ADDR_LEN], uint16_t &port_id)
{
  uint16_t nb_ports = rte_eth_dev_count_avail();
  struct ether_addr addr;

  for (uint16_t i = 0; i < nb_ports; i++)
  {
    rte_eth_macaddr_get(i, &addr);
    if (memcmp(&mac[0], addr.addr_bytes, ETHER_ADDR_LEN) == 0)
    {
      port_id = i;
      return E_OK;
    }
  }
  return E_FAIL;
}

//
EResultT CCommDriverFactory::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
{
  CBaseModule::IInit(callback_ptr, target_api, init_info);

  //Example of initialization of the profiler counter
  char prof_name[] = "COMM";
  comm_prof_.Init(prof_name, callback_ptr); //INIT Profiler counter
  LOG(E_MEDIUM, "init passed");
  return E_OK;
}

EResultT CCommDriverFactory::IColdStart()
{
  return E_OK;
}

EResultT CCommDriverFactory::IConfigure(EConfigId id, void *in, void **out)
{
  char str[] = "Call passed";
  callback_ptr_->ILogData(E_LOW, str);
  dpdk_eth_driver_[num_ip_drivers_].Init(callback_ptr_, num_ip_drivers_);
  *out =
      (void *)(static_cast<DpdkEthDeviceAPI *>(&(dpdk_eth_driver_[num_ip_drivers_])));

  num_ip_drivers_++;
  ASSERT(num_ip_drivers_ <= MAX_COMM_DPDK_DRIVERS);

  return E_OK;
}

class CCommDriverFactory *g_module_ptr = NULL;

//API function to connect with ITarget API
extern "C" uint32_t
IGetConnectAPI(void **target_ptr)
{
  g_module_ptr = new CCommDriverFactory();
  *target_ptr = static_cast<IModuleControlAPI *>(g_module_ptr);
  return 0;
}
