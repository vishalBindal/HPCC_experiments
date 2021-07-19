./waf --run 'scratch/third mix/config_task1.txt' > log_out.txt
python3 convert.py log_out.txt > log_cleaned.txt
cd ../analysis
./run.sh
echo "Read and cleaned trace file"
cd ~/ns-allinone-3.33/ns-3.33/
./waf --run 'scratch/hpcc_plot --resolution=1'
./waf --run 'scratch/hpcc_plot --resolution=3'
./waf --run 'scratch/hpcc_plot_old --resolution=1'
./waf --run 'scratch/hpcc_plot_old --resolution=3'
./waf --run 'scratch/hpcc_int --resolution=1'
./waf --run 'scratch/hpcc_int --resolution=3'
