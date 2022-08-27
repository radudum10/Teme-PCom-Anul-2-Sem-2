/**
 * @file arp.c
 * @author Radu-Andrei Dumitru
 * @brief This is the implementation for ARP protocol. It contains a setter for
 * the ARP header, a search of the ARP Table for a given IP address, a function
 * that adds a new entry to the table and request / reply functions with a
 * handler. 
 */

#include "router_funs.h"
#include "arp.h"
#include "skel.h"

void arp_hdr_setter(struct arp_header *arp_hdr, uint16_t arp_op,
					uint8_t dest_mac[ETH_ALEN], uint32_t dest_ip,
					uint8_t source_mac[ETH_ALEN], uint32_t source_ip)
{
	// The default values for ARP.
	arp_hdr->htype = htons(ETHER_HTYPE);
	arp_hdr->ptype = htons(ETHERTYPE_IP);
	arp_hdr->hlen = ETH_ALEN;
	arp_hdr->plen = IPV4_PROTO_LEN;

	// The operation can be ARPOP_REQUEST / ARPOP_REPLY.
	arp_hdr->op = htons(arp_op);

	// Setting the source MAC and IP.
	memcpy(arp_hdr->sha, source_mac, ETH_ALEN);
	arp_hdr->spa = source_ip;

	// Setting the target MAC and IP.
	memcpy(arp_hdr->tha, dest_mac, ETH_ALEN);
	arp_hdr->tpa = dest_ip;
}

struct arp_entry *search_arp(arp_entry **arp_table, size_t arp_len,
                             uint32_t ip)
{
	size_t i;
	for (i = 0; i < arp_len; i++) {
		if (arp_table[i]->ip == ip)
			return arp_table[i];
	}

	return NULL;
}

void add_arp_entry(arp_entry **arp_table, size_t *arp_max_len, size_t *arp_len,
                 uint32_t ip, uint8_t mac[ETH_ALEN])
{
    /* If the maximum length has been reached, then increase the size of the
    table.
    */
	if (*arp_len == *arp_max_len) {
		arp_entry **aux = realloc(arp_table, (++(*arp_max_len)) *
							sizeof(arp_entry *));

		DIE(!aux, "Realloc failed.\n");

		arp_table = aux;
	}

    // Create a new entry and add it to the table.
	arp_entry *new = malloc(sizeof(arp_entry));
	DIE(!new, "New entry failed.\n");

	new->ip = ip;
	memcpy(new->mac, mac, ETH_ALEN);
	arp_table[(*arp_len)++] = new;
}

void search_queue(queue no_mac_dest_queue, rtable_entry *rtable,
 				  size_t rtable_len, arp_entry **arp_table, size_t arp_len)
{
	queue aux = queue_create();

	while (!queue_empty(no_mac_dest_queue)) {
		packet *crt = (packet *) (queue_deq(no_mac_dest_queue));
		struct iphdr *ip_hdr = (struct iphdr *)
								(crt->payload + sizeof(struct ether_header));
		struct ether_header *eth_hdr = (struct ether_header *) crt->payload;

		rtable_entry *route = get_best_route(rtable, rtable_len,
                                             ip_hdr->daddr);
		arp_entry *existing = search_arp(arp_table, arp_len, route->next_hop);

		if (existing) {
			memcpy(eth_hdr->ether_dhost, existing->mac, ETH_ALEN);
			get_interface_mac(route->interface, eth_hdr->ether_shost);
			send_packet(crt);
		} else {
			queue_enq(no_mac_dest_queue, crt);
		}
	}

	while (!queue_empty(aux)) {
		queue_enq(no_mac_dest_queue, queue_deq(aux));
	}
}

void arp_requester(rtable_entry *route)
{
	packet request;

	struct ether_header *eth_hdr = (struct ether_header *) request.payload;
	struct arp_header *arp_hdr = (struct arp_header *) (request.payload
								+ sizeof(struct ether_header));

    request.len = sizeof(struct ether_header) + sizeof(struct arp_header);
    memset(request.payload	, 0, sizeof(request.payload));
	request.interface = route->interface;

	// The request should be sent to Broadcast. (FF:FF:FF:FF:FF:FF).
	uint8_t broadcast_mac[ETH_ALEN];
	memset(broadcast_mac, 0xFF, ETH_ALEN);

	/* The request source is the MAC from the interface that it will be sent
	on.
	*/
	uint8_t source_mac[6];
	get_interface_mac(route->interface, source_mac);
	eth_hdr_setter(eth_hdr, broadcast_mac, source_mac, ETHERTYPE_ARP);

	// The Target MAC Address in ARP Header is 00:00:00:00:00:00.
	uint8_t no_mac[ETH_ALEN];
	memset(no_mac, 0x00, ETH_ALEN);
	// Extracting the source IP.
	uint32_t source_ip = inet_addr(get_interface_ip(route->interface));

	arp_hdr_setter(arp_hdr, ARPOP_REQUEST, no_mac, route->next_hop,
				source_mac, source_ip);

	send_packet(&request);
}

void arp_replier(packet base)
{
	struct arp_header *base_arp = (struct arp_header *) (base.payload +
								sizeof(struct ether_header));

    // Create a new packet for the reply.
	packet reply;
	struct ether_header *reply_eth = (struct ether_header *) reply.payload;
	struct arp_header *reply_arp = (struct arp_header *) (reply.payload +
									sizeof(struct ether_header));


	reply.len = sizeof(struct ether_header) + sizeof(struct arp_header);
	memset(reply.payload, 0, sizeof(reply.payload));
    /* The reply should be sent on the same interface as the one where the
    request came on.
    */
	reply.interface = base.interface;

	/* The target becomes the source of the request and the found host becomes
	the source.
	*/
	uint8_t source_mac[6];
	get_interface_mac(base.interface, source_mac);

	eth_hdr_setter(reply_eth, base_arp->sha, source_mac,
	 ETHERTYPE_ARP);

	arp_hdr_setter(reply_arp, ARPOP_REPLY, base_arp->sha,
					base_arp->spa, source_mac, base_arp->tpa);

	send_packet(&reply);
}

void arp_handler(packet pkt, queue no_mac_dest_queue,
				 arp_entry **arp_table, size_t *arp_max_len, size_t *arp_len,
				 rtable_entry *rtable, size_t rtable_len)
{
	struct arp_header *arp_hdr = (struct arp_header *) (pkt.payload +
										sizeof(struct ether_header));
	// Identifying the ARP Operation Type.
	if (ntohs(arp_hdr->op) == ARPOP_REQUEST) {
		arp_replier(pkt);

	} else if(htons(arp_hdr->op) == ARPOP_REPLY) {
		uint32_t interf_ip = inet_addr(get_interface_ip(pkt.interface));
		if (arp_hdr->tpa == interf_ip) {
			add_arp_entry(arp_table, arp_max_len, arp_len,
						arp_hdr->spa, arp_hdr->sha);
			search_queue(no_mac_dest_queue, rtable, rtable_len, arp_table,
							*arp_len);
		}
	}
}