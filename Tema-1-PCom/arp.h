/**
 * @file arp.h
 * @author Radu-Andrei Dumitru
 * @brief Functions prototypes for ARP.
 */

#ifndef _ARP_H_
#define _ARP_H_

#include "skel.h"
#include "queue.h"

#define ARP_MAX_SIZE 20

/**
 * @brief Sets the ARP header fields to the given values.
 *
 * @param arp_hdr The base ARP header.
 * @param arp_op The ARP Operation (Request / Reply).
 * @param dest_mac The MAC destination where the packet should arrive.
 * (00:00:00:00:00:00 for a Request)
 * @param dest_ip  The IP of the destination where the packet should arrive.
 * @param source_mac The MAC of the packet's starting point.
 * @param source_ip The IP of the packet's starting point.
 */
void arp_hdr_setter(struct arp_header *arp_hdr, uint16_t arp_op,
					uint8_t dest_mac[ETH_ALEN], uint32_t dest_ip,
					uint8_t source_mac[ETH_ALEN], uint32_t source_ip);

/**
 * @brief Searches the ARP table for an IP. If found, returns the entry,
 * otherwise returns NULL.
 *
 * @param arp_table The ARP table of the router.
 * @param arp_len The length of the ARP table.
 * @param ip The IP that it will be searched in the ARP table.
 * @return struct arp_entry* The found entry in the ARP table.
 */
struct arp_entry *search_arp(arp_entry **arp_table, size_t arp_len,
                             uint32_t ip);

/**
 * @brief Starting from a given packet, creates an ARP Request and sends it.
 *
 * @param route The route where the packet should be sent on.
 */
void arp_requester(rtable_entry *route);

/**
 * @brief Starting from a given ARP REQUEST, creates an ARP REPLY and sends it.
 *
 * @param base The ARP REQUEST.
 */
void arp_replier(packet base);

/**
 * @brief Creates a new entry and adds it to the table. If the maximum length
 * of the table is reached, then it also reallocate memory for the table.
 * @param arp_table The ARP table of the current router.
 * @param arp_max_len The maximum length of the ARP table (needed for realloc).
 * @param arp_len The current length of the ARP table.
 * @param ip The IP that should be added in the entry.
 * @param mac The MAC that should be added in the entry.
 */
void add_arp_entry(arp_entry **arp_table, size_t *arp_max_len, size_t *arp_len,
                 uint32_t ip, uint8_t mac[ETH_ALEN]);

/**
 * @brief When a ARP Reply is recieved, search the ARP Queue in order to send
 * the waiting packet.
 * @param no_mac_dest_queue The queue that holds the packets without
 * destination MAC.
 * @param rtable The routing table.
 * @param rtable_len The length of the routing table.
 * @param arp_table The ARP table of the current router.
 * @param arp_len The length of the ARP table.
 */
void search_queue(queue no_mac_dest_queue, rtable_entry *rtable,
 				  size_t rtable_len, arp_entry **arp_table, size_t arp_len);

/**
 * @brief Calls reply / request functions depending of the type of the ARP
 * recieved.
 * @param pkt The recieved ARP packet.
 * @param no_mac_dest_queue The queue that holds the packet without
 * destination MAC.
 * @param arp_table The ARP table of the current router.
 * @param arp_max_len The maximum length of the ARP table (needed for realloc).
 * @param arp_len The current length of the ARP table.
 * @param rtable The routing table.
 * @param rtable_len The length of the routing table.
 */
void arp_handler(packet pkt, queue no_mac_dest_queue,
				 arp_entry **arp_table, size_t *arp_max_len, size_t *arp_len,
				 rtable_entry *rtable, size_t rtable_len);
#endif