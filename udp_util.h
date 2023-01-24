#ifndef __UDP_UTIL_H__
#define __UDP_UTIL_H__

#include <stdint.h>

#include <rte_ip.h>
#include <rte_eal.h>
#include <rte_log.h>
#include <rte_udp.h>
#include <rte_flow.h>
#include <rte_mbuf.h>
#include <rte_ring.h>
#include <rte_ether.h>
#include <rte_atomic.h>
#include <rte_ethdev.h>
#include <rte_malloc.h>
#include <rte_mempool.h>

// Control Block
typedef struct control_block_s {
	// used only by the TX
	uint32_t 						src_addr;
	uint32_t 						dst_addr;
	uint16_t						src_port;
	uint16_t						dst_port;

	// used only in the beginning
	struct rte_flow_item_eth		flow_eth;
	struct rte_flow_item_eth		flow_eth_mask;
	struct rte_flow_item_ipv4		flow_ipv4;
	struct rte_flow_item_ipv4		flow_ipv4_mask;
	struct rte_flow_item_udp		flow_udp;
	struct rte_flow_item_udp		flow_udp_mask;
	struct rte_flow_action_mark 	flow_mark_action;
	struct rte_flow_action_queue 	flow_queue_action;

} __rte_cache_aligned control_block_t;

#define ETH_IPV4_TYPE_NETWORK		0x0008

extern uint16_t dst_udp_port;
extern uint32_t dst_ipv4_addr;
extern uint32_t src_ipv4_addr;
extern struct rte_ether_addr dst_eth_addr;
extern struct rte_ether_addr src_eth_addr;

extern uint64_t nr_flows;
extern uint64_t nr_queues;
extern uint16_t nr_servers;
extern uint32_t frame_size;
extern uint32_t udp_payload_size;
extern struct rte_mempool *pktmbuf_pool;
extern control_block_t *control_blocks;

void init_blocks();
void fill_udp_packet(uint16_t i, struct rte_mbuf *pkt);
void fill_udp_payload(uint8_t *payload, uint32_t length);

#endif // __UDP_UTIL_H__
