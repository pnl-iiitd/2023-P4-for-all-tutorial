p4c-ebpf --arch psa --xdp --target kernel -o demo.c demo.p4
cd ~/p4c/
clang -Ibackends/ebpf/runtime -Ibackends/ebpf/runtime/usr/include -O2 -g -c -emit-llvm -o /home/kesahv/Desktop/IIIT-Kottayum/p4-eBPF/ping-blocker/demo.bc /home/kesahv/Desktop/IIIT-Kottayum/p4-eBPF/ping-blocker/demo.c
cd /home/kesahv/Desktop/IIIT-Kottayum/p4-eBPF/ping-blocker
llc -march=bpf -mcpu=generic -filetype=obj -o demo.o demo.bc
