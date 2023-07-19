# Making a simple Ping blocker with P4 and loading it to XDP hook

complete the program in the ping-blocker directory. the major changes needs to be done in the parser.

## compiling for psa architecture
run the command to compile the p4 code to eBPF
```bash
bash compile.sh
```
this will generate the demo.o file

## Setting up the enviorment
run the command from p4-eBPF directory
```bash
bash start.sh
```
this command will create the create 2 network namespaces and will assign virtual interfaces to each. Finally it will also populate the routing table

## Testing out the program
Before attaching the program to the XDP hook try to run the following command
```bash
sudo ip netns exec vnet1 ping 10.0.0.1
```
you will see the ping response coming

now attach the program to XDP hook in vnet0 the veth0 interface using the following command

```bash
sudo ip netns exec vnet0 ip link set veth0 xdpgeneric obj demo.o sec xdp_ingress/xdp-ingress
```

again try to ping from vnet0
```bash
sudo ip netns exec vnet1 ping 10.0.0.1
```

