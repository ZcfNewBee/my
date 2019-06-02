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
	if(type == 3 || type == 11)
	{
		//获取输入的ip头
		struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
		//获取源、目的地址
		u32 daddr = ntohl(in_ip->saddr);
   		u32 saddr = longest_prefix_match(daddr)->iface->ip;
		//申请数据包空间
		u32 ip_total_len = ETHER_HDR_SIZE + IP_BASE_HDR_SIZE + ICMP_HDR_SIZE + IP_HDR_SIZE(in_ip) + 8;
		char *packet = malloc(ip_total_len);
		//获取要发送的数据包的以太网包头
		struct ether_header *eh = (struct ether_header *)packet;
		//获取输入的数据包的以太网包头
		struct ether_header *in_eh = (struct ether_header *)in_pkt;
		//获取要发送的的ip头
		struct iphdr *ip = packet_to_ip_hdr(packet);
		eh->ether_type = htons(ETH_P_IP);

		//初始化ip包头
		ip_init_hdr(ip, saddr, daddr, ip_total_len - ETHER_HDR_SIZE, IPPROTO_ICMP);

		//获取icmp包头
		struct icmphdr *icmp_hdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + IP_BASE_HDR_SIZE);
		memset(icmp_hdr, 0, ICMP_HDR_SIZE);
		icmp_hdr->type = type;
		icmp_hdr->code = code;
		char *data = icmp_hdr + ICMP_HDR_SIZE;
		//拷贝数据
		memcpy(data + 4, in_ip, IP_HDR_SIZE(ip) + 8);
		//设置checksum
		icmp_hdr->checksum = icmp_checksum(icmp_hdr,  ip_total_len - ETHER_HDR_SIZE - IP_BASE_HDR_SIZE);
		//发送数据包
		ip_send_packet(packet, ip_total_len);
	}
	else if(type == 0)
	{
		//获取输入ip头
		struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
		//获取源、目的地址
		u32 daddr = ntohl(in_ip->saddr);
   		u32 saddr = longest_prefix_match(daddr)->iface->ip;
		//申请数据包空间
		u32 ip_total_len = len - IP_HDR_SIZE(in_ip) + IP_BASE_HDR_SIZE;
		char *packet = malloc(ip_total_len);
		//获取要发送的数据包以太网头
		struct ether_header *eh = (struct ether_header *)packet;
		//获取收到的数据包以太网头
		struct ether_header *in_eh = (struct ether_header *)in_pkt;
		//获取要发送的ip头
		struct iphdr *ip = packet_to_ip_hdr(packet);

		eh->ether_type = htons(ETH_P_IP);

		//设置ip地址
		ip_init_hdr(ip, saddr, daddr, ip_total_len - ETHER_HDR_SIZE, IPPROTO_ICMP);

		//获取icmp头
		struct icmphdr *icmp_hdr = (struct icmphdr *)IP_DATA(ip);
		memset(icmp_hdr, 0, ICMP_HDR_SIZE);
		icmp_hdr->type = type;
		icmp_hdr->code = code;
		memcpy(((char*)icmp_hdr) + ICMP_HDR_SIZE - 4,in_pkt + ETHER_HDR_SIZE + IP_HDR_SIZE(in_ip) + 4, len - ETHER_HDR_SIZE - IP_HDR_SIZE(in_ip) - 4);

		icmp->checksum = icmp_checksum(icmp_hdr, tot_size-ETHER_HDR_SIZE-IP_BASE_HDR_SIZE);

		ip_send_packet(packet, ip_total_len);
	}
	//fprintf(stderr, "TODO: malloc and send icmp packet.\n");
}
