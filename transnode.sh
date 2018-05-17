#! /bin/bash

vppctl create host-interface name vpp2
vppctl create host-interface name vpp3
vppctl set interface state host-vpp2 up
vppctl set interface state host-vpp3 up
vppctl set interface ip address host-vpp2 10.0.1.3/24
vppctl set interface ip address host-vpp3 10.0.2.3/64
vppctl ip route add 10.0.0.0/24 via 10.0.1.2
vppctl ip route add 10.0.3.0/24 via 10.0.2.4
#vppctl set ioam-trace profile trace-type 0x1f trace-elts 4 trace-tsp 1 node-id 0x2 app-data 0x1234
