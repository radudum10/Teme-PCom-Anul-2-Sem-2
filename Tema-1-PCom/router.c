#include "arp.h"
#include "ip_icmp.h"
#include "router_funs.h"
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

void free_mem(rtable_entry *rtable, arp_entry **arp_table, size_t arp_len)
{
	size_t i;
	for (i = 0; i < arp_len; i++) {
		free(arp_table[i]);
	}

	free(arp_table);
	free(rtable);
}

int main(int argc, char *argv[])
{
	packet m;
	int rc;

	rtable_entry *rtable = malloc(RTABLE_ENTRIES * sizeof(rtable_entry));
	DIE(!rtable, "Route table malloc.\n");

	/* The starting size for the ARP Table is 20. If this size is exceeded then
	the table will be realloc'd.
	*/
	arp_entry **arp_table = malloc(ARP_MAX_SIZE * sizeof(arp_entry *));
	DIE(!arp_table, "ARP table malloc.\n");

	size_t rtable_len = read_rtable(argv[1], rtable);
	size_t arp_len = 0;
	size_t arp_max_len = ARP_MAX_SIZE;

	/* This queue will hold packets that currently do not have the destination
	MAC known.
	*/
	queue no_mac_dest_queue = queue_create();

	qsort(rtable, rtable_len, sizeof(rtable_entry), lpm_comparator);

	init(argc - 2, argv + 2);

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_packet");

		struct ether_header *eth_hdr = (struct ether_header *) m.payload;
		struct iphdr *ip_hdr = (struct iphdr *) (m.payload +
								sizeof(struct ether_header));

		// Identifying the Type: IP / ARP.
			if (htons(eth_hdr->ether_type) == ETHERTYPE_IP) {
				// If an Echo Reply has been sent, continue.
				if (icmp_handler(m, ip_hdr))
					continue;

				if (is_garbage(m, ip_hdr))
					continue;

				rtable_entry *route = get_best_route(rtable, rtable_len,
									ip_hdr->daddr);

				if (!route) {
					send_icmp(m, ICMP_DEST_UNREACH);
					continue;
				}

				// Update the TTL and compute the checksum in an efficient way.
				incremental_checksum(ip_hdr);

				struct arp_entry *arp_entry = search_arp(arp_table, arp_len,
											route->next_hop);

				/* If there is no ARP entry for the IP, then create, send a
				REQUEST and put a copy of the current packet in the waiting
				queue.
				*/
				if (!arp_entry) {
					arp_requester(route);

					// Saving a copy of the packet in the queue.
					packet saved;
					memcpy(&saved, &m, sizeof(packet));
					saved.interface = route->interface;
					queue_enq(no_mac_dest_queue, &saved);
					continue;
				}

				uint8_t source_mac[ETH_ALEN];
				get_interface_mac(route->interface, source_mac);
				eth_hdr_setter(eth_hdr, arp_entry->mac, source_mac, ETHERTYPE_IP);

				m.interface = route->interface;
				send_packet(&m);
		} else if (htons(eth_hdr->ether_type) == ETHERTYPE_ARP) {
			arp_handler(m, no_mac_dest_queue, arp_table, &arp_max_len, &arp_len,
						rtable, rtable_len);
		}
	}

	free_mem(rtable, arp_table, arp_len);
	return 0;
}
