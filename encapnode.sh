#! /bin/bash

vppctl create host-interface name vpp0
vppctl create host-interface name vpp1
vppctl set interface state host-vpp0 up
vppctl set interface state host-vpp1 up
vppctl set interface ip address host-vpp0 10.0.0.2/24
vppctl set interface ip address host-vpp1 10.0.1.2/24
vppctl ip route add 0.0.0.0/0 via 10.0.1.3
#vppctl classify table acl-miss-next ip6-node ip6-lookup mask l3 ip6 dst
#vppctl classify session acl-hit-next ip6-node ip6-add-hop-by-hop table-index 0 match l3 ip6 dst 2001:3::4 ioam-encap test-encap
#vppctl set int input acl intfc host-vpp0 ip6-table 0
