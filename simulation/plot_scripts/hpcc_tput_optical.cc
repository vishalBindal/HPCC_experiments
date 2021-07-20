/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/gnuplot.h"

using namespace ns3;

#define ll long long

NS_LOG_COMPONENT_DEFINE ("HPCC_plot_optical");

std::string csv_file_str = "/home/vbindal/High-Precision-Congestion-Control/analysis/task1_clean.csv";
std::string std_config_str = "/home/vbindal/High-Precision-Congestion-Control/simulation/mix/config_optical.txt";
std::string out_config_str = "/home/vbindal/High-Precision-Congestion-Control/simulation/mix/config_temp.txt";

int n_flows = 5;
std::vector<std::string> flows = {
  "H1->H2", "H1->H5", "H3->H5", "H3->H2", "H7->H2"
};
std::vector<std::pair<int, int>> flow_node_ports = {
  {3, 500},
  {6, 501},
  {6, 502},
  {3, 503},
  {3, 504}
};
std::vector<std::string> colors = {
  "#4daf4a", "#e41a1c", "#377eb8", "#984ea3", "#ff7f00"
};

std::vector<int> rate_ai_vals = {
  100,150,200,250,300,350,400,450,500,550,600,650,700
};

std::vector<ll> schedules = {
  60000, 120000, 180000, 240000, 300000
}; 

void CreatePlot(std::vector<std::vector<std::vector<double>>> &tput)
{
  for(int j=0;j<(int)schedules.size();++j)
  {
    std::string fileNameWithNoExtension = "plots/plot_tput_HPCC_optical_" + std::to_string(schedules[j] / 6000);
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Throughput v/s RATE\\\\\\_AI ( Schedule: " + std::to_string(schedules[j] / 6000) + " RTT )";

    Gnuplot plot(graphicsFileName);
    plot.SetTitle(plotTitle);
    plot.SetTerminal("png size 1024,768");
    plot.AppendExtra("set key noenhanced");
    plot.SetLegend("RATE\\\\\\_AI (in Mb/s)", "Throughput (in Gb/s)");

    plot.AppendExtra("set yrange [0:+25]");

    for(int i=0;i<n_flows;++i)
    {
      Gnuplot2dDataset dataset;
      dataset.SetTitle(flows[i]);
      dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
      std::string lc = "lt rgb \"" + colors[i] + "\"";
      dataset.SetExtra(lc);

      for(int r=0;r<(int)rate_ai_vals.size();++r)
        dataset.Add(rate_ai_vals[r], tput[i][r][j]);

      plot.AddDataset(dataset);
    }

    std::ofstream plotFile(plotFileName.c_str());
    plot.GenerateOutput(plotFile);
    plotFile.close();
    
    std::string plot_cmd = "gnuplot " + plotFileName;
    system(plot_cmd.c_str());

    std::cout<<"Plot saved as "<<graphicsFileName<<"\n";
  }
}

void run_exp(int r)
{
  std::ifstream infile;
  const char * std_config = std_config_str.c_str();
  infile.open(std_config);

  std::ofstream outfile;
  const char *out_config = out_config_str.c_str();
  outfile.open(out_config);

  std::string line;
  for(int i=0;i<58;++i)
  {
    std::getline(infile, line);
    if(i==21) // modify RATE_AI
      outfile<<"RATE_AI " + std::to_string(rate_ai_vals[r]) + "Mb/s"<<std::endl;
    else outfile<<line<<std::endl;
  }
  infile.close();
  outfile.close();

  std::string run_cmd = "cd ~/High-Precision-Congestion-Control/simulation/ && ./run_exp.sh";
  system(run_cmd.c_str());
}

void plot()
{
  std::vector<std::vector<std::vector<double>>> tput(n_flows, std::vector<std::vector<double>>(rate_ai_vals.size(), std::vector<double>(schedules.size(), 0)));

  for(int r=0;r<(int)rate_ai_vals.size();++r)
  {
    run_exp(r);

    for(int i=0;i<n_flows;++i)
    {
      std::ifstream file; 
      const char* csv_file = csv_file_str.c_str();
      file.open(csv_file);

      ll time_ns;
      int node, port;
      int packet_size;
      while(file>>time_ns)
      {
        file>>node>>port>>packet_size;
        time_ns -= 2e9;
        
        if(node == flow_node_ports[i].first && port == flow_node_ports[i].second)
          for(int j=0;j<(int)schedules.size();++j)
            if(time_ns <= schedules[j])
              tput[i][r][j] += packet_size;
      }
      file.close();
      
      for(int j=0;j<(int)schedules.size();++j)
        tput[i][r][j] = tput[i][r][j] *  8  / schedules[j];     // Convert to GBits/sec
    }
  }
  
  CreatePlot(tput);
}

int
main (int argc, char *argv[])
{

  // CommandLine cmd (__FILE__);
  // cmd.AddValue ("resolution", "Time scale for throughput, 1/2/3 (1: 30ms, 2: 10ms, 3: 1ms)", resolution);
  // cmd.Parse (argc, argv);

  plot();

  return 0;
}
