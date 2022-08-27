/**
 * @file ip_icmp.h
 * @author Radu-Andrei Dumitru
 * @brief Function prototypes for interacting with IP and ICMP headers and for
 * managing ICMP request and replies.
 */

#ifndef _IP_ICMP_H
#define _IP_ICMP_H

#include "skel.h"
#include "router_funs.h"

#define BEST_EFFORT 0

#define ICMP_CODE 0
#define ICMP_ID 1337

#define ICMP_REPLY_SENT 1
#define ICMP_NO_REPLY_SENT 0


/**
 * @brief Sets all the fields of an iphdr struct (The frag_off is taken from an
 * ICMP request).
 * @param reply_iphdr The base IP header.
 * @param req_iphdr The IP header of the packet recieved.
 * @param interf_ip The IP of the current interface.
 */
void ip_hdr_setter(struct iphdr *reply_iphdr, struct iphdr *req_iphdr,
					 uint32_t interf_ip);

/**
 * @brief Sets the code to 0 and the type to the given icmp_type. If the type
 * is destination unreachable / time exceeded, then the id and sequence will be
 * set to 0. Else if the type is an Echo Reply, it sets id and sequence to the
 * same that were in the Echo Request.
 *
 * @param reply_icmp The base IP header.
 * @param req_icmp The ICMP header of the packet recieved.
 * @param icmp_type The type of the ICMP. (Reply / Time Exceeded / Destination
 * unreachable)
 */
void icmp_hdr_setter(struct icmphdr *reply_icmp, struct icmphdr *req_icmp,
					 uint8_t icmp_type);

/**
 * @brief Starting from a Echo Request, creates a new packet, completes it with
 * the setter functions and sends it.
 *
 * @param req_pkt The recieved packet.
 * @param icmp_type The type of the ICMP.(Reply / Time Exceeded / Destination
 * unreachable)
 */
void send_icmp(packet req_pkt, uint8_t icmp_type);

/**
 * @brief Deciding if it should send a reply and calls the functions needed for
 * sending one.
 *
 * @param pkt The recieved packet.
 * @param ip_hdr The header of the recieved packet.
 * @return int 1 (ICMP_REPLY_SENT) if a reply has been sent,
 * 0 (ICMP_NO_REPLY_SENT), otherwise.
 */
int icmp_handler(packet pkt, struct iphdr *ip_hdr);

/**
 * @brief Decides if a packet should be thrown away (invalid checksum
 * / ttl < 2). Also if the problem is with ttl, sends an ICMP Time Exceeded
 * reply.
 *
 * @param pkt The packet that should be inspected.
 * @param ip_hdr The IP header of the packet that should be inspected
 * @return int 1 if an ICMP Time Exceeded reply has been sent, 0 otherwise.
 */
int is_garbage(packet pkt, struct iphdr *ip_hdr);

/**
 * @brief Updates the IPv4 checksum in an efficient way, avoiding using
 * recalculation.
 * @param ip_hdr The IP Header of the packet.
 */
void incremental_checksum(struct iphdr *ip_hdr);
#endif