#include "udp_util.h"

// Shuffle the UDP source port array
void shuffle(uint16_t* arr, uint32_t n) {
	if(n < 2) {
		return;
	}

	for(uint32_t i = 0; i < n - 1; i++) {
		uint32_t j = i + rte_rand() / (UINT64_MAX / (n - i) + 1);
		uint16_t tmp = arr[j];
		arr[j] = arr[i];
		arr[i] = tmp;
	}
}

// Create and initialize the Control Blocks for all flows
void init_blocks() {
	// allocate the all control block structure previosly
	control_blocks = (control_block_t *) rte_zmalloc("control_blocks", nr_flows * sizeof(control_block_t), RTE_CACHE_LINE_SIZE);

	// choose UDP source port for all flows
	uint16_t src_udp_port;
	uint16_t ports[nr_flows];
	for(uint32_t i = 0; i < nr_flows; i++) {
		ports[i] = rte_cpu_to_be_16((i % (nr_flows/nr_servers)) + 1);
	}

	// shuffle port array
	shuffle(ports, nr_flows);

	for(uint32_t i = 0; i < nr_flows; i++) {
		src_udp_port = ports[i];

		control_blocks[i].src_addr = src_ipv4_addr;
		control_blocks[i].dst_addr = dst_ipv4_addr;

		control_blocks[i].src_port = src_udp_port;
		control_blocks[i].dst_port = rte_cpu_to_be_16(dst_udp_port + (i % nr_servers));

		control_blocks[i].flow_mark_action.id = i;
		control_blocks[i].flow_queue_action.index = i % nr_queues;
		control_blocks[i].flow_eth.type = ETH_IPV4_TYPE_NETWORK;
		control_blocks[i].flow_eth_mask.type = 0xFFFF;
		control_blocks[i].flow_ipv4.hdr.src_addr = control_blocks[i].dst_addr;
		control_blocks[i].flow_ipv4.hdr.dst_addr = control_blocks[i].src_addr;
		control_blocks[i].flow_ipv4_mask.hdr.src_addr = 0xFFFFFFFF;
		control_blocks[i].flow_ipv4_mask.hdr.dst_addr = 0xFFFFFFFF;
		control_blocks[i].flow_udp.hdr.src_port = control_blocks[i].dst_port;
		control_blocks[i].flow_udp.hdr.dst_port = control_blocks[i].src_port;
		control_blocks[i].flow_udp_mask.hdr.src_port = 0xFFFF;
		control_blocks[i].flow_udp_mask.hdr.dst_port = 0xFFFF;
	}
}

// Fill the UDP packets from Control Block data
void fill_udp_packet(uint16_t i, struct rte_mbuf *pkt) {
	// get control block for the flow
	control_block_t *block = &control_blocks[i];

	// ensure that IP/UDP checksum offloadings
	pkt->ol_flags |= (RTE_MBUF_F_TX_IPV4 | RTE_MBUF_F_TX_IP_CKSUM | RTE_MBUF_F_TX_UDP_CKSUM);

	// fill Ethernet information
	struct rte_ether_hdr *eth_hdr = (struct rte_ether_hdr *) rte_pktmbuf_mtod(pkt, struct ether_hdr*);
	eth_hdr->dst_addr = dst_eth_addr;
	eth_hdr->src_addr = src_eth_addr;
	eth_hdr->ether_type = ETH_IPV4_TYPE_NETWORK;

	// fill IPv4 information
	struct rte_ipv4_hdr *ipv4_hdr = rte_pktmbuf_mtod_offset(pkt, struct rte_ipv4_hdr *, sizeof(struct rte_ether_hdr));
	ipv4_hdr->version_ihl = 0x45;
	ipv4_hdr->total_length = rte_cpu_to_be_16(frame_size - sizeof(struct rte_ether_hdr));
	ipv4_hdr->time_to_live = 255;
	ipv4_hdr->packet_id = 0;
	ipv4_hdr->next_proto_id = IPPROTO_UDP;
	ipv4_hdr->fragment_offset = 0;
	ipv4_hdr->src_addr = block->src_addr;
	ipv4_hdr->dst_addr = block->dst_addr;
	ipv4_hdr->hdr_checksum = 0;

	// fill UDP information
	struct rte_udp_hdr *udp_hdr = rte_pktmbuf_mtod_offset(pkt, struct rte_udp_hdr *, sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr));
	udp_hdr->dst_port = block->dst_port;
	udp_hdr->src_port = block->src_port;
	udp_hdr->dgram_len = rte_cpu_to_be_16(sizeof(struct rte_udp_hdr) + udp_payload_size);
	udp_hdr->dgram_cksum = 0;

	// fill the payload of the packet
	uint8_t *payload = ((uint8_t*)udp_hdr) + sizeof(struct rte_udp_hdr);
	fill_udp_payload(payload, udp_payload_size);

	// fill the packet size
	pkt->data_len = frame_size;
	pkt->pkt_len = pkt->data_len;
}

// Fill the payload of the UDP packet
void fill_udp_payload(uint8_t *payload, uint32_t length) {
	for(uint32_t i = 0; i < length; i++) {
		payload[i] = 'A';
	}
}
