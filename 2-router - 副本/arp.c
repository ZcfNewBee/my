#include "arp.h"
#include "base.h"
#include "types.h"
#include "packet.h"
#include "ether.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "log.h"

// send an arp request: encapsulate an arp request packet, send it out through
// iface_send_packet
void arp_send_request(iface_info_t *iface, u32 dst_ip) {

    // ether_dhost 目的MAC
    // ether_shost 源MAC
    // ether_type 包类型ID，这里设置为ARP对应的0x0806
    void *packet = malloc(ETHER_HDR_SIZE + sizeof(struct ether_arp));
    struct ether_header *eth = (struct ether_header *) packet;
    memset(eth->ether_dhost, 0xFF, ETH_ALEN);
    memcpy(eth->ether_shost, iface->mac, ETH_ALEN);
    eth->ether_type = htons(ETH_P_ARP);

    // arp_hrd;	    硬件地址转化成网络字节序
    // arp_pro;		协议转化成网络字节序
    // arp_hln;		硬件地址长度，6
    // arp_pln;		协议长度，4
    // arp_op;		ARP请求操作码转化成网络字节序
	// arp_sha[ETH_ALEN];	源硬件地址
	// arp_spa;		源协议地址
	// arp_tha[ETH_ALEN];	目的硬件地址
	// arp_tpa;		目的协议地址
    struct ether_arp *arp = (struct ether_arp *)(packet + ETHER_HDR_SIZE);
    arp->arp_hrd = htons(ARPHRD_ETHER);
    arp->arp_pro = htons(ETH_P_IP);
    arp->arp_hln = ETH_ALEN;
    arp->arp_pln = sizeof(u32);
    arp->arp_op = htons(ARPOP_REQUEST);
    arp->arp_spa = htonl(iface->ip);
    arp->arp_tpa = htonl(dst_ip);
    memcpy(arp->arp_sha, iface->mac, ETH_ALEN);
    memset(arp->arp_tha, 0, ETH_ALEN);
    
    int len = ETHER_HDR_SIZE + sizeof(struct ether_arp);
    iface_send_packet(iface, packet, len);
    return;
}

void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr) {

    // 构造方法与arp_send_request相同，不同是arp_op这里是ARPOP_REPLY
    void *packet = malloc(ETHER_HDR_SIZE + sizeof(struct ether_arp));

    struct ether_header *eth = (struct ether_header *) packet;
    memcpy(eth->ether_dhost, req_hdr->arp_sha, ETH_ALEN);
    memcpy(eth->ether_shost, iface->mac, ETH_ALEN);
    eth->ether_type = htons(ETH_P_ARP);

    struct ether_arp *arp = (struct ether_arp *)(packet + ETHER_HDR_SIZE);
    arp->arp_hrd = htons(ARPHRD_ETHER);
    arp->arp_pro = htons(ETH_P_IP);
    arp->arp_hln = ETH_ALEN;
    arp->arp_pln = sizeof(u32);
    arp->arp_op = htons(ARPOP_REPLY);
    arp->arp_spa = htonl(iface->ip);
    arp->arp_tpa = htonl(req_hdr->arp_spa);
    memcpy(arp->arp_sha, iface->mac, ETH_ALEN);
    memcpy(arp->arp_tha, req_hdr->arp_sha, ETH_ALEN);

    int len = ETHER_HDR_SIZE + sizeof(struct ether_arp);
    iface_send_packet(iface, packet, len);
    return;
}

void handle_arp_packet(iface_info_t *iface, char *packet, int len) {

    struct ether_arp *req_hdr = (struct ether_arp *)(packet + ETHER_HDR_SIZE);

    u16 arp_op = ntohs(req_hdr->arp_op);
    u32 arp_dstip = ntohl(req_hdr->arp_tpa);
    u32 arp_srcip = ntohl(req_hdr->arp_spa);
    if (arp_op  == ARPOP_REQUEST && arp_dstip == iface->ip){
            arp_send_reply(iface, req_hdr);
    } else if (arp_op == ARPOP_REPLY) {
        arpcache_insert(arp_srcip, req_hdr->arp_sha);
    }
    
    return;
}

// send (IP) packet through arpcache lookup
//
// Lookup the mac address of dst_ip in arpcache. If it is found, fill the
// ethernet header and emit the packet by iface_send_packet, otherwise, pending
// this packet into arpcache, and send arp request.
void iface_send_packet_by_arp(iface_info_t *iface, u32 dst_ip, char *packet, int len) {
    struct ether_header *eh = (struct ether_header *)packet;
    memcpy(eh->ether_shost, iface->mac, ETH_ALEN);
    eh->ether_type = htons(ETH_P_IP);

    u8 dst_mac[ETH_ALEN];
    int found = arpcache_lookup(dst_ip, dst_mac);
    if (found) {
        // log(DEBUG, "lookup "IP_FMT" success, pend this packet", HOST_IP_FMT_STR(dst_ip));
        memcpy(eh->ether_dhost, dst_mac, ETH_ALEN);
        iface_send_packet(iface, packet, len);
    } else {
        // log(DEBUG, "lookup "IP_FMT" failed, pend this packet", HOST_IP_FMT_STR(dst_ip));
        arpcache_append_packet(iface, dst_ip, packet, len);
    }
}
