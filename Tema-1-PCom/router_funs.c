/**
 * @file router_funs.c
 * @author Radu-Andrei Dumitru
 * @brief This contains a comparator needed for qsort and the Longest Prefix
 * Match done with Binary Search. Also, it contains a function that sets the
 * Ether header to given parameters.
 */

#include "router_funs.h"

int lpm_comparator(const void *p1, const void *p2)
{
	struct route_table_entry *e1 = (struct route_table_entry *) p1;
	struct route_table_entry *e2 = (struct route_table_entry *) p2;

	if (e1->prefix == e2->prefix)
		return e2->mask - e1->mask;

	return e2->prefix - e1->prefix;
}

rtable_entry *get_best_route(rtable_entry *rtable, size_t rtable_len,
							 uint32_t dest_ip)
{
	unsigned int mid, left = 0, right = rtable_len - 1;
	int best = -1;

	while (left <= right) {
		mid = left + (right - left) / 2;

		if ((dest_ip & rtable[mid].mask) == rtable[mid].prefix) {
			best = mid;
			right = mid - 1;
		} else if (((dest_ip & rtable[mid].mask) > rtable[mid].prefix)) {
			right = mid - 1;
		} else {
			left = mid + 1;
		}
	}

	if (best == -1)
		return NULL;

	return &rtable[best];
}

void eth_hdr_setter(struct ether_header *eth_hdr, uint8_t dest_mac[ETH_ALEN],
					 uint8_t source_mac[ETH_ALEN], uint16_t type)
{
	memcpy(eth_hdr->ether_dhost, dest_mac, ETH_ALEN);
	memcpy(eth_hdr->ether_shost, source_mac, ETH_ALEN);
	eth_hdr->ether_type = ntohs(type);
}