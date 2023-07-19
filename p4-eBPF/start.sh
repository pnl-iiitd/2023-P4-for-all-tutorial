echo $p4c=~/p4c/
ip link add veth0 type veth peer name veth1

ip netns add vnet0
ip link set veth0 netns vnet0
ip -n vnet0 addr add 10.0.0.1/24 dev veth0
ip -n vnet0 link set veth0 up
ip -n vnet0 link set lo up 

ip netns add vnet1
ip link set veth1 netns vnet1
ip -n vnet1 addr add 10.0.0.2/24 dev veth1
ip -n vnet1 link set veth1 up
ip -n vnet1 link set lo up
