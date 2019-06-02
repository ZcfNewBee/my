#include "ip.h"
#include "icmp.h"
#include "rtable.h"
#include "arp.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>

// forward the IP packet from the interface specified by longest_prefix_match, 
// when forwarding the packet, you should check the TTL, update the checksum,
// determine the next hop to forward the packet, then send the packet by 
// iface_send_packet_by_arp
void ip_forward_packet(u32 ip_dst, char *packet, int len)
{
	//fprintf(stderr, "TODO: forward ip packet.\n");
	
	rt_entry_t *rt_entry = longest_prefix_match(ip_dst);
	if(!rt_entry)
	{
		icmp_send_packet(packet, len, ICMP_DEST_UNREACH, ICMP_NET_UNREACH);
		//free(packet);
		return;
	}
	struct iphdr *ip = packet_to_ip_hdr(packet);
	ip->ttl = htonl(ntohl(ip->ttl) - 1);
	if(ip->ttl <= ICMP_EXC_TTL)
	{
		icmp_send_packet(packet, len, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL);
		return;
	}
	ip->checksum = ip_checksum(ip);
	u32 next_hop_ip_dst = rt_entry->gw;
	if (!next_hop_ip_dst)
	{
		next_hop_ip_dst = ip_dst;
	}
	
	iface_send_packet_by_arp(rt_entry->iface, next_hop_ip_dst, packet, len);
}

// handle ip packet
//
// If the packet is ICMP echo request and the destination IP address is equal to
// the IP address of the iface, send ICMP echo reply; otherwise, forward the
// packet.
void handle_ip_packet(iface_info_t *iface, char *packet, int len)
{
	struct iphdr *ip = packet_to_ip_hdr(packet);
	u32 daddr = ntohl(ip->daddr);
	if (daddr == iface->ip) {
		//fprintf(stderr, "TODO: reply to the sender if it is ping packet.\n");
		if(ip->protocol == IPPROTO_ICMP)
		{
			struct icmphdr *icmp_hdr = ((void *)packet) + ETHER_HDR_SIZE + IP_HDR_SIZE(ip);
			if(icmp_hdr->type == ICMP_ECHOREQUEST)
			{
				icmp_send_packet(packet, len, ICMP_ECHOREPLY, ICMP_NET_UNREACH);
			}
		}
	}
	else {
		ip_forward_packet(daddr, packet, len);
	}
}
