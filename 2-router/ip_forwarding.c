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
void ip_forward_packet(u32 ip_dst, char *packet, int len) {
    rt_entry_t *entry = longest_prefix_match(ip_dst);
    if (!entry) {
        icmp_send_packet(packet, len, ICMP_DEST_UNREACH, ICMP_NET_UNREACH);    
        return; 
    }

    u32 sip = ntohl(packet_to_ip_hdr(packet)->saddr);

    struct iphdr *ip = packet_to_ip_hdr(packet);
    ip->ttl = htonl(ntohl(ip->ttl) - 1);
    if (ip->ttl <= 0) {
        icmp_send_packet(packet, len, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL);
        return;
    }
    ip->checksum = ip_checksum(ip);
    u32 next_hop = entry->gw;
    if (!next_hop)
        next_hop = ip_dst;

    iface_send_packet_by_arp(entry->iface, next_hop, packet, len);
}

// handle ip packet
//
// If the packet is ICMP echo request and the destination IP address is equal to
// the IP address of the iface, send ICMP echo reply; otherwise, forward the
// packet.
void handle_ip_packet(iface_info_t *iface, char *packet, int len) {
    struct iphdr *ip  = ((void *)packet) + ETHER_HDR_SIZE;
    struct icmphdr *icmp = ((void *)packet) + ETHER_HDR_SIZE + IP_HDR_SIZE(ip);
    u32 des_ip = ntohl(ip->daddr);
    if (des_ip==iface->ip && icmp->type == ICMP_ECHOREQUEST){
        icmp_send_packet(packet, len, ICMP_ECHOREPLY, ICMP_NET_UNREACH);
    } else {
        ip_forward_packet(des_ip, packet, len);
    }
}