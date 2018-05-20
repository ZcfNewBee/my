#! /bin/bash

ip netns add ns0
ip link add vpp0 type veth peer name vethns0
ip link add vpp1 type veth peer name vethns1
ip link set vethns0 netns ns0
ip netns exec ns0 ip link set lo up
ip netns exec ns0 ip link set vethns0 up
ip netns exec ns0 ip addr add 2001::1/64 dev vethns0
ip netns exec ns0 ip -6 route add ::/0 via 2001::2
ip netns exec ns0 ip addr add 10.0.0.1 dev vethns0
ip netns exec ns0 ip -6 route add 0.0.0.0/0 via 10.0.0.2
ip netns exec ns0 ethtool -K vethns0 rx off tx off
ip link set vpp0 up
ip link set vpp1 up
ip link set vethns1 up
ip link add name br0 type bridge
ip link set vethns1 master br0
ip link set ens37 master br0
ip link set br0 up
