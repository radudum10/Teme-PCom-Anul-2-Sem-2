/**
 * @file router_funs.h
 * @author Radu-Andrei Dumitru
 * @brief Function prototypes for those needed by the router.
 */

#ifndef _ROUTER_FUNS_H_
#define _ROUTER_FUNS_H_

#include "skel.h"

/**
 * @brief Comparator function needed for qsort. If the prefixes are equal then
 * sort by mask. Otherwise, sort by prefixes. (Descending)
 *
 * @param p1 A pointer to a router table entry.
 * @param p2 A pointer to a router table entry.
 * @return int A positive or negative number, depending on the substraction of
 * the prefixes / masks (if the prefixes are equal).
 */
int lpm_comparator(const void *p1, const void *p2);

/**
 * @brief Performs a binary search in order to find the longest prefix match.
 *
 * @param rtable The routing table.
 * @param rtable_len The length of the routing table.
 * @param dest_ip The IP where the packet should arrive.
 * @return rtable_entry* The entry in the table with the longest prefix match.
 */
rtable_entry *get_best_route(rtable_entry *rtable, size_t rtable_len,
							 uint32_t dest_ip);

/**
 * @brief Sets the destination and source MAC of an Ether header to the given
 * MACs.
 *
 * @param eth_hdr The base Ether header.
 * @param dest_mac The MAC destination where the packet should arrive.
 * @param source_mac The MAC of the packet starting point.
 * @param type The type of the packet. (ARP / IPv4 in this case)
 */
void eth_hdr_setter(struct ether_header *eth_hdr, uint8_t dest_mac[ETH_ALEN],
					 uint8_t source_mac[ETH_ALEN], uint16_t type);

#endif