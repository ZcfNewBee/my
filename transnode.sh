#! /bin/bash

vppctl create host-interface name vpp2
vppctl create host-interface name vpp3
vppctl set interface state host-vpp2 up
vppctl set interface state host-vpp3 up
vppctl set interface ip address host-vpp2 2001:1::3/64
vppctl set interface ip address host-vpp3 2001:2::3/64
vppctl ip route add 2001::/64 via 2001:1::2
vppctl ip route add 2001:3::/64 via 2001:2::4
vppctl set ioam-trace profile trace-type 0x1f trace-elts 4 trace-tsp 1 node-id 0x2 app-data 0x1234
vppctl set pot profile name example id 0 prime-number 0x7fff0000fa884685 secret_share 0x564cdbdec4eb625d lpc 0x1 polynomial2 0x23f3a227186a bits-in-random 63  