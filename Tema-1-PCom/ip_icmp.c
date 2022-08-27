/**
 * @file ip_icmp.c
 * @author Radu-Andrei Dumitru
 * @brief This is the implementation for IP and ICMP functions. There are
 * setters for the headers, a function that sends ICMP replies, a handler, a
 * integrity validator for IP header and the incremental checksum function.
 */

#include "ip_icmp.h"
#include "skel.h"

void ip_hdr_setter(struct iphdr *reply_iphdr, struct iphdr *req_iphdr,
					 uint32_t interf_ip)
{
	reply_iphdr->ihl = 5;
	reply_iphdr->version = 4;

	reply_iphdr->tos = BEST_EFFORT;
	reply_iphdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
	reply_iphdr->id = htons(ICMP_ID);
	reply_iphdr->frag_off = req_iphdr->frag_off;
	reply_iphdr->ttl = req_iphdr->ttl;
	reply_iphdr->protocol = IPPROTO_ICMP;
	reply_iphdr->saddr = interf_ip;
	reply_iphdr->daddr = req_iphdr->saddr;

	reply_iphdr->check = 0;
	reply_iphdr->check = ip_checksum((void *) reply_iphdr,
						 sizeof(struct iphdr));
}

void icmp_hdr_setter(struct icmphdr *reply_icmp, struct icmphdr *req_icmp,
					 uint8_t icmp_type)
{
	reply_icmp->type = icmp_type;
	reply_icmp->code = ICMP_CODE;

	/* Check if it is destination unreachable / time exceeded / echo reply. For
	reply, keep the same id and sequence.
	*/
	if (reply_icmp->type == ICMP_UNREACH ||
		reply_icmp->type == ICMP_TIME_EXCEEDED) {
		reply_icmp->un.echo.id = 0;
		reply_icmp->un.echo.sequence = 0;

	} else if (reply_icmp->type == ICMP_ECHOREPLY) {
		reply_icmp->un.echo.id = req_icmp->un.echo.id;
		reply_icmp->un.echo.sequence = req_icmp->un.echo.sequence;
	}

	// Compute the checksum.
	reply_icmp->checksum = 0;
	reply_icmp->checksum = ip_checksum((void *) reply_icmp,
							 sizeof(struct icmp));
}

void send_icmp(packet req_pkt, uint8_t icmp_type)
{
	// Extracting the headers from the REQUEST packet.
	struct ether_header *req_eth = (struct ether_header *) (req_pkt.payload);
	struct iphdr *req_ip = (struct iphdr *)
							(req_pkt.payload + sizeof(struct ether_header));
	struct icmphdr *req_icmp = (struct icmphdr *) (req_pkt.payload +
						 sizeof(struct ether_header) + sizeof(struct iphdr));

	packet reply;

	reply.len = sizeof(struct ether_header) + sizeof(struct iphdr) +
				sizeof(struct icmphdr);
	memset(reply.payload, 0, MAX_LEN);
	reply.interface = req_pkt.interface;

	// Extracting the headers from the in progress reply packet.
	struct ether_header *rply_eth = (struct ether_header *) (reply.payload);
	struct iphdr *rply_ip = (struct iphdr *)
							(reply.payload + sizeof(struct ether_header));
	struct icmphdr *rply_icmp = (struct icmphdr *)
						(reply.payload + reply.len - sizeof(struct icmphdr));

	/* The old source becomes the new target and the old target becomes the new
	target
	*/
	uint8_t source_mac[ETH_ALEN];
	get_interface_mac(req_pkt.interface, source_mac);

	eth_hdr_setter(rply_eth, req_eth->ether_shost, source_mac, ETHERTYPE_IP);

	uint32_t this_ip = inet_addr(get_interface_ip(req_pkt.interface));
	ip_hdr_setter(rply_ip, req_ip, this_ip);

	icmp_hdr_setter(rply_icmp, req_icmp, icmp_type);

	send_packet(&reply);
}

int icmp_handler(packet pkt, struct iphdr *ip_hdr)
{
	if (ip_hdr->protocol == IPPROTO_ICMP) {
		struct icmphdr *icmp_hdr = (struct icmphdr *) (pkt.payload +
		sizeof(struct ether_header) + sizeof(struct iphdr));

		// If is a REQUEST and is for the current router, send a reply.
		if (icmp_hdr->type == ICMP_ECHO) {
			uint32_t interf_ip = inet_addr(get_interface_ip(pkt.interface));
			if (ip_hdr->daddr == interf_ip) {
				send_icmp(pkt, ICMP_ECHOREPLY);
				return ICMP_REPLY_SENT;
			}
		}
	}

	return ICMP_NO_REPLY_SENT;
}

int is_garbage(packet pkt, struct iphdr *ip_hdr)
{
	// If the checksum is not correct, throw it away.
	if (ip_checksum((void *) ip_hdr, sizeof(struct iphdr)) != 0)
		return 1;

	/* If the time to live is 0 or 1, then send an ICMP to tell that the given
	 * time is exceeded.
	*/
	if (ip_hdr->ttl < 2) {
		send_icmp(pkt, ICMP_TIME_EXCEEDED);
		return 1;
	}

	return 0;
}

void incremental_checksum(struct iphdr *ip_hdr)
{
	ip_hdr->ttl--;

	uint32_t aux_sum;
	uint16_t mask = ((uint16_t) 1) << 8; // 0000 0001 0000 0000

	// Incrementing the first byte of the check sum.
	aux_sum = ntohs(ip_hdr->check) + mask;

	// Adding the carry to the end. (To perform one's complement sum).
	ip_hdr->check = htons((aux_sum + (aux_sum >> 16)));
}