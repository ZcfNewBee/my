#include "icmp.h"
#include "ip.h"
#include "rtable.h"
#include "arp.h"
#include "base.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// send icmp packet
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
    
    if (type != ICMP_ECHOREPLY)
	{
		 struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
    	u32 daddr = ntohl(in_ip->saddr);
    	u32 saddr = longest_prefix_match(daddr)->iface->ip;

    	long total_ip_len = 0;
		total_ip_len = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE(in_ip) + 8;
		char* packet = (char*) malloc(total_ip_len);

    	struct icmphdr *icmp_hdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + IP_BASE_HDR_SIZE);

    	struct ether_header *eh = (struct ether_header *)packet;
    	eh->ether_type = htons(ETH_P_IP);

    	struct iphdr *ip_hdr = packet_to_ip_hdr(packet);
		ip_init_hdr(ip_hdr,
                saddr,
                daddr,
                total_ip_len - ETHER_HDR_SIZE,
                IPPROTO_ICMP);

   

   		memset(icmp_hdr, 0, ICMP_HDR_SIZE);
    	icmp_hdr->code = code;
    	icmp_hdr->type = type;
		memcpy(((char*)icmp_hdr + ICMP_HDR_SIZE),
               (char*)in_ip,
               IP_HDR_SIZE(in_ip) + 8);
		icmp_hdr->checksum =
        icmp_checksum(icmp_hdr, total_ip_len-ETHER_HDR_SIZE-IP_BASE_HDR_SIZE);
    	ip_send_packet(packet, total_ip_len);
	}
        
    else
	{
		// 获取输入的ip头
    	struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
    	u32 daddr = ntohl(in_ip->saddr);
    	u32 saddr = longest_prefix_match(daddr)->iface->ip;

    	long total_ip_len = 0;
		total_ip_len = len - IP_HDR_SIZE(in_ip) + IP_BASE_HDR_SIZE;
		char* packet = (char*) malloc(total_ip_len);

    	struct icmphdr *icmp_hdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + IP_BASE_HDR_SIZE);

    	struct ether_header *eh = (struct ether_header *)packet;
    	eh->ether_type = htons(ETH_P_IP);

    	struct iphdr *ip_hdr = packet_to_ip_hdr(packet);

    	ip_init_hdr(ip_hdr,
                saddr,
                daddr,
                total_ip_len - ETHER_HDR_SIZE,
                IPPROTO_ICMP);

   

    	memset(icmp_hdr, 0, ICMP_HDR_SIZE);
    	icmp_hdr->code = code;
    	icmp_hdr->type = type;
		memcpy(((char*)icmp_hdr) + ICMP_HDR_SIZE - 4,
               in_pkt + ETHER_HDR_SIZE + IP_HDR_SIZE(in_ip) + 4,
               len - ETHER_HDR_SIZE - IP_HDR_SIZE(in_ip) - 4);
		icmp_hdr->checksum =
        icmp_checksum(icmp_hdr, total_ip_len-ETHER_HDR_SIZE-IP_BASE_HDR_SIZE);
    	ip_send_packet(packet, total_ip_len);
	}  
}