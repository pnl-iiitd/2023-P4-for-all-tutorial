# Compiling P4 application for DPDK

Complete the `l2fwd.p4` to swap the MAC header for an incoming packet

run the following command to generate the spec file

```bash
p4c-dpdk --arch psa l2fwd.p4 -o l2fwd.spec
```

For running the spec file we navigate to example folder and use pipelines to run the spec file

```bash
sudo ./pipeline -l 0-3 -n 4 -- -s l2fwd.spec
```

You might see that there are few errors. These are just compilation warning that you might see and can be omitted.


Hence we can compile any P4 program to spec file and then load the spec file in a pipeline

