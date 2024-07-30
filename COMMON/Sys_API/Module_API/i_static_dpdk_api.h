/*
 * i_static_dpdk_api.h
 *
 *  Created on: Sep 1, 2020
 *      Author: mlevanovsky
 */

#ifndef COMMON_SYS_API_MODULE_API_I_STATIC_DPDK_API_H_
#define COMMON_SYS_API_MODULE_API_I_STATIC_DPDK_API_H_

#define STATIC_DPDK	"STATIC_DPDK"



#define mbuf_mtod_offset(m, t, o)	\
	((t)((char *)(m)->buf_addr + (m)->data_off + (o)))
#define mbuf_mtod(m, t) rte_pktmbuf_mtod_offset(m, t, 0)

enum proc_type_t {
	PROC_AUTO = -1,   /* allow auto-detection of primary/secondary */
	PROC_PRIMARY = 0, /* set to zero, so primary is the default */
	PROC_SECONDARY,

	PROC_INVALID
};

typedef int (lcore_function_t)(void *);
/******************************************************************************************//*!
 *@class Dpdk_Shared_Libary_API
 *@brief The purpose of this class is :
 *@brief This class expose the DPDK SHARED LIBARY Module control API.
  *********************************************************************************************/
struct Dpdk_Shared_Libary_API
{

//mbuf create, updtae and chain functions
virtual void mbuf_free(struct rte_mbuf *m)=0;
virtual void mbuf_reset(struct rte_mbuf *m)=0;


virtual uint16_t  mbuf_refcnt_update(struct rte_mbuf *m, int16_t value)=0;
virtual int mbuf_alloc_bulk(struct rte_mempool *pool, struct rte_mbuf **mbufs, unsigned count)=0;
virtual void mbuf_attach(struct rte_mbuf *mi, struct rte_mbuf *m)=0;
virtual char *mbuf_adj(struct rte_mbuf *m, uint16_t len)=0;
virtual int mbuf_trim(struct rte_mbuf *m, uint16_t len)=0;
virtual int mbuf_add_to_chain(struct rte_mbuf *head, struct rte_mbuf *tail)=0;
virtual struct rte_mbuf *mbuf_alloc(struct rte_mempool *mp)=0;
virtual char *mbuf_prepend(struct rte_mbuf *m,uint16_t len)=0;
virtual void mbuf_dump(FILE *f, const struct rte_mbuf *m, unsigned dump_len)=0;

virtual void* rte_malloc(const char *type, size_t size, unsigned align)=0;
virtual int eal_remote_launch(lcore_function_t *f, void *arg, unsigned slave_id)=0;

//ethernet headers functions
virtual void ethernet_header_macaddr_get(uint16_t port_id, struct rte_ether_addr *mac_addr)=0;
virtual void acl_free(struct rte_acl_ctx *ctx)=0;
virtual struct rte_acl_ctx* acl_create(const struct rte_acl_param *param)=0;
virtual int  acl_add_rules(struct rte_acl_ctx *ctx, const struct rte_acl_rule *rules,uint32_t num)=0;
virtual int acl_build(struct rte_acl_ctx *ctx, const struct rte_acl_config *cfg)=0;
virtual uint16_t ethernet_header_dev_count_avail()=0;
virtual int acl_classify(const struct rte_acl_ctx *ctx,const uint8_t **data,
		 uint32_t *results, uint32_t num,
		 uint32_t categories)=0;
virtual void ethernet_header_dev_stop(uint16_t port_id)=0;
virtual int ethernet_header__dev_configure(uint16_t port_id, uint16_t nb_rx_queue,uint16_t nb_tx_queue, const struct rte_eth_conf *eth_conf)=0;
virtual int ethernet_header_dev_adjust_nb_rx_tx_desc(uint16_t port_id,uint16_t *nb_rx_desc,uint16_t *nb_tx_desc)=0;
virtual int ethernet_header_rx_queue_setup(uint16_t port_id, uint16_t rx_queue_id,uint16_t nb_rx_desc, unsigned int socket_id,const struct rte_eth_rxconf *rx_conf,struct rte_mempool *mb_pool)=0;
virtual int ethernet_header_tx_queue_setup(uint16_t port_id, uint16_t tx_queue_id,uint16_t nb_tx_desc, unsigned int socket_id,const struct rte_eth_txconf *tx_conf)=0;
virtual int ethernet_header_dev_start(uint16_t port_id)=0;
virtual void ethernet_header_promiscuous_enable(uint16_t port_id)=0;
virtual void ethernet_header_dev_close(uint16_t port_id)=0;
virtual void ethernet_header_link_get_nowait(uint16_t port_id, struct rte_eth_link *link)=0;
virtual uint16_t ethernet_header_tx_burst(uint16_t port_id, uint16_t queue_id,struct rte_mbuf **tx_pkts,uint16_t nb_pkts)=0;

virtual uint16_t ethernet_header_rx_burst(uint16_t port_id, uint16_t queue_id,struct rte_mbuf **tx_pkts,uint16_t nb_pkts)=0;
virtual int ethernet_header_dev_configure(uint16_t port_id, uint16_t nb_rx_queue,uint16_t nb_tx_queue, const struct rte_eth_conf *eth_conf)=0;
virtual enum proc_type_t eal_process_type()=0;
virtual int ethernet_header_dev_socket_id(uint16_t port_id)=0;
virtual int ethernet_header_stats_reset(uint16_t port_id)=0;
virtual uint32_t align32pow2_rte(uint32_t x)=0;
virtual void ether_addr_copy(const struct rte_ether_addr *ea_from,
				   struct rte_ether_addr *ea_to)=0;

//mempool functions
virtual uint32_t mempool_avail_count(const struct rte_mempool *mp)=0;
virtual uint32_t mempool_in_use_count(struct rte_mempool *mp)=0;
virtual struct rte_mempool *mempool_create_by_ops(const char *name, unsigned int n, unsigned int cache_size, uint16_t priv_size, uint16_t data_room_size,int socket_id, const char *ops_name)=0;
virtual struct rte_mempool *mempool_lookup(const char *vname)=0;
virtual void mempool_free(struct rte_mempool *mp)=0;

//ring functions
virtual uint32_t ring_count(struct rte_ring *  r )=0;
virtual uint32_t ring_get_capacity(struct rte_ring *  r )=0;
virtual uint32_t ring_enqueue_bulk(struct rte_ring *r, void * const *obj_table,  unsigned int n, unsigned int *free_space)=0;
virtual int ring_dequeue(struct rte_ring *r, void **obj_p)=0;
virtual int ring_enqueue(struct rte_ring *r, void *obj_p)=0;
virtual struct rte_ring* ring_create(char* name, uint32_t count, int socket_id, uint32_t flags)=0;
virtual void ring_free(struct rte_ring *r)=0;
virtual int32_t ring_dequeue_burst(struct rte_ring *r, void **obj_table,unsigned int n, unsigned int *available)=0;
virtual uint32_t ring_dequeue_bulk(struct rte_ring *r,void **obj_table, unsigned int n,
    		unsigned int *available)=0;
virtual struct rte_mempool *mempool_create(const char *name, unsigned n,
   	unsigned cache_size, uint16_t priv_size, uint16_t data_room_size,
   	int socket_id)=0;

//
virtual char* str_error_rte(int errnum)=0;
virtual void malloc_rte_free(void *ptr)=0;
virtual uint64_t get_timer_cycles()=0;
virtual uint64_t get_timer_hz()=0;
virtual void *zmalloc_rte(const char *type, size_t size, unsigned align)=0;
virtual int strsplit_rte(char *string, int stringlen, char **tokens, int maxtokens, char delim)=0;
virtual int eal_init(int argc, char **argv)=0;
/*
virtual void exit_rte(int exit_code, const char *format)=0;
*/
};

#endif /* COMMON_SYS_API_MODULE_API_I_STATIC_DPDK_API_H_ */
