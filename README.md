# HPCC experiments

[Original repository](https://github.com/alibaba-edu/High-Precision-Congestion-Control)

Summary of files relevant for experiments:
- The main HPCC logic is implemented in `simulation/src/point-to-point/model/rdma-hw.cc`. Altered initial sending rates here for experiments where initial sending rate != total link bandwidth
- `simulation/plot_scripts` contains:
  1. hpcc_int.cc: To plot queue occupancy and sending rates
  2. hpcc_plot_old.cc: To plot throughput at a course granularity (1 point every 0.04ms/0.1ms)
  3. hpcc_plot.cc: To plot throughput at a finer granularity (1 point every 10 packets)
  4. hpcc_tput_optical: To plot throughput vs RATE_AI for different optical schedules
- `simulation/mix` contains all config files:
  1. `topology_middle_100.txt` and `topology_middle_25.txt` are the topologies used for various experiments, with the only difference being the bandwidth of middle link being 100Gbps and 25Gbps respectively. H1 to H8 are assigned nodes 2 to 9 respectively, while S1 is assigned 0 and S2 is assigned 1.
  2. `flow_5.txt` is used for experiments with all 5 flows (H1->H2, H1->H5, H3->H5, H3->H2, H7->H2). `flow_2_h12_h15.txt`, `flow_2_h15_h35.txt`, `flow_2_h15_h48.txt` are used for experiments with 2 flows, with the file name representing the flows. e.g. `flow_2_h12_h15.txt` has flows H1->H2 and H1->H5
  3. `config_task1.txt` is the config file used, appropriately choosing `TOPOLOGY_FILE`, `FLOW_FILE` and `RATE_AI` according to the experiment. e.g. `config_optical.txt` is used for the experiment with different optical schedules, varying the value of `RATE_AI`.
  4. Documentation for structures of above files can be found in `config_doc.txt`, and at the end of `flow.txt` and `topology.txt`.


From the original README:

## HPCC simulation
[Project page of HPCC](https://hpcc-group.github.io/) includes latest news of HPCC and extensive evaluation results using this simulator.

This is the simulator for [HPCC: High Precision Congestion Control (SIGCOMM' 2019)](https://rmiao.github.io/publications/hpcc-li.pdf). It also includes the implementation of DCQCN, TIMELY, DCTCP, PFC, ECN and Broadcom shared buffer switch.

We have update this simulator to support HPCC-PINT, which reduces the INT header overhead to 1 to 2 byte. This improves the long flow completion time. See [PINT: Probabilistic In-band Network Telemetry (SIGCOMM' 2020)](https://liyuliang001.github.io/publications/pint.pdf).

## NS-3 simulation
The ns-3 simulation is under `simulation/`. Refer to the README.md under it for more details.

## Traffic generator
The traffic generator is under `traffic_gen/`. Refer to the README.md under it for more details.

## Analysis
We provide a few analysis scripts under `analysis/` to view the packet-level events, and analyzing the fct in the same way as [HPCC](https://liyuliang001.github.io/publications/hpcc.pdf) Figure 11.
Refer to the README.md under it for more details.

## Questions
For technical questions, please create an issue in this repo, so other people can benefit from your questions. 
You may also check the issue list first to see if people have already asked the questions you have :)

For other questions, please contact Rui Miao (miao.rui@alibaba-inc.com).
