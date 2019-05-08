#include "icmp.h"
#include "ip.h"
#include "rtable.h"
#include "arp.h"
#include "base.h"

#include <stdio.h>
#include <stdlib.h>

// send icmp packet
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
	if(type == 3 || type == 11)
	{
		//获取输入的ip头
		struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
		//申请数据包空间
		u32 ip_total_len = 20 + 8 + 4 + IP_HDR_SIZE(in_ip) + 8;
		char *packet = malloc(ETHER_HDR_SIZE + ip_total_len);
		//获取要发送的数据包的以太网包头
		struct ether_header *eh = (struct ether_header *)packet;
		//获取输入的数据包的以太网包头
		struct ether_header *in_eh = (struct ether_header *)in_pkt;
		//获取要发送的的ip头
		struct iphdr *ip = packet_to_ip_hdr(packet);
		//设置以太网地址
		memcpy(eh, in_eh + sizeof(u8) * ETH_ALEN, sizeof(u8) * ETH_ALEN);
		memcpy(eh + sizeof(u8) * ETH_ALEN, in_eh, sizeof(u8) * ETH_ALEN);
		eh->ether_type = ETH_P_IP;

		//初始化ip包头
		ip_init_hdr(ip, ntohl(in_ip->daddr), ntohl(in_ip->saddr), ip_total_len, IPPROTO_ICMP);

		//获取icmp包头
		struct icmphdr *icmp_hdr = (struct icmphdr *)IP_DATA(ip);
		icmp_hdr->type = type;
		icmp_hdr->code = code;
		char *data = icmp_hdr + ICMP_HDR_SIZE;
		//拷贝数据
		memset(data, 0, 4);
		memcpy(data + 4, in_ip, IP_HDR_SIZE(ip) + 8);
		//设置checksum
		icmp_hdr->checksum = icmp_checksum(icmp_hdr, ICMP_HDR_SIZE);
		//发送数据包
		ip_send_packet(packet, ip_total_len + ETHER_HDR_SIZE);
	}
	else if(type == 0)
	{
		//获取输入ip头
		struct iphdr *in_ip = packet_to_ip_hdr(in_pkt);
		//申请数据包空间
		char *packet = malloc(len);
		//复制
		memcpy(packet, in_pkt, len);
		//获取要发送的数据包以太网头
		struct ether_header *eh = (struct ether_header *)packet;
		//获取收到的数据包以太网头
		struct ether_header *in_eh = (struct ether_header *)in_pkt;
		//获取要发送的ip头
		struct iphdr *ip = packet_to_ip_hdr(packet);
		//设置以太网地址
		memcpy(eh, in_eh + sizeof(u8) * ETH_ALEN, sizeof(u8) * ETH_ALEN);
		memcpy(eh + sizeof(u8) * ETH_ALEN, in_eh, sizeof(u8) * ETH_ALEN);
		eh->ether_type = ETH_P_IP;

		//设置ip地址
		ip_init_hdr(ip, ntohl(in_ip->daddr), ntohl(in_ip->daddr), ntohl(in_ip->tot_len), IPPROTO_ICMP);

		//获取icmp头
		struct icmphdr *icmp_hdr = (struct icmphdr *)IP_DATA(ip);
		icmp_hdr->type = type;
		icmp_hdr->code = code;
		icmp_hdr->checksum = icmp_checksum(icmp_hdr, ICMP_HDR_SIZE);

		ip->checksum = ip_checksum(ip);

		ip_send_packet(packet, len);
	}
	//fprintf(stderr, "TODO: malloc and send icmp packet.\n");
}
