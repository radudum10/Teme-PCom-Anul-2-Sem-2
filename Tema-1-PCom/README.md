* Dumitru Radu-Andrei 322CB

# Tema 1 PCOM

* A short description of the implementation. The code contains a lot of comments
and I also generated a documentation using doxygen.

## router.c
---

* The router recieves a packet. It extracts the Ether Header from it and checks
whether it is IP Type or ARP Type.

* If it is an IP packet, it also checks if it is an ICMP. If so, it calls the
ICMP Handler, else tries to find a route for the current packet and an ARP
entrym updates the ttl, increments the checksum and sends it further.

* If it is an ARP, calls the ARP Handler.

## router_funs
---
* A comparator for qsort, for descending order.

* Longest Prefix Match implemented with Binary Sort which searches until the
prefix is matched, then further for finding the greatest mask.

* A setter for the Ether Header.

## arp
---
* Setter for the ARP Header.

* arp_requester: create an ARP Request and send it (just some field setting,
nothing spectacular).

* arp_replier: When an ARP Request is recieved and is for the router, then
create and send an ARP Reply. (Again, just some field setting).

* add_arp_entry: If the table exceeds its maximum length, then reallocs. Create
a new entry and then add it to the table.

* search_queue: When an ARP reply is recieved, this function is used in order
to complete the destination MAC of the packet that was not sent, and send it.

* arp_handler: Decides which functions should be called (reply, request etc).

## ip_icmp
---

* Setters for ip_hdr and icmp_hdr.

* send_icmp: When an ICMP Request is recieved, create a reply and send it.

* icmp_handler: Decides if it should send a reply and calls the functions.

* is_garbage: checks if a packet should be thrown away (ttl < 2, invalid
checksum)

* incremental_checksum: When updating the TTL, the computation of the checksum
can be done more quickly by using one's complement sum.