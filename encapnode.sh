#! /bin/bash

vppctl create host-interface name vpp0
vppctl create host-interface name vpp1
vppctl set interface state host-vpp0 up
vppctl set interface state host-vpp1 up
vppctl set interface ip address host-vpp0 2001::2/64
vppctl set interface ip address host-vpp1 2001:1::2/64
vppctl ip route add ::/0 via 2001:1::3