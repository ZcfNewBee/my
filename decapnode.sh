#! /bin/bash
vppctl create host-interface name vpp0
vppctl create host-interface name vpp1
vppctl set interface state host-vpp0 up
vppctl set interface state host-vpp1 up
vppctl set interface ip address host-vpp0 2001:3::4/64
vppctl set interface ip address host-vpp1 2001:2::4/64
vppctl ip route add ::/0 via 2001:2::3
vppctl classify table acl-miss-next ip6-node ip6-lookup mask l3 ip6 dst
vppctl classify session acl-hit-next ip6-node ip6-lookup table-index 0 match l3 ip6 dst 2001:3::5 ioam-decap test-decap
vppctl set int input acl intfc host-vpp1 ip6-table 0 
vppctl set ioam-trace profile trace-type 0x1f trace-elts 4 trace-tsp 1 node-id 0x3 app-data 0x1234
vppctl set pot profile name example id 0 validate-key 0x7fff0000fa88465d prime-number 0x7fff0000fa884685 secret_share 0x7a08fbfc5b93116d lpc 0x3 polynomial2 0x3ff738597ce bits-in-random 63
vppctl set ioam rewrite analyse