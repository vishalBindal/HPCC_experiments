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
#include <queue>
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

NS_LOG_COMPONENT_DEFINE ("Task1_plot");

int resolution = 1;
ll total_ns = 3e7;
ll obs_ns = 1e5;
int n_points = int(total_ns/obs_ns);
std::string cc = "HPCC";

int max_window_size = 10;

std::string csv_file_str = "/home/vbindal/High-Precision-Congestion-Control/analysis/task1_clean.csv";

// int n_flows = 5;
// std::vector<std::string> flows = {
//   "H1->H2", "H1->H5", "H3->H5", "H3->H2", "H7->H2"
// };
// std::vector<std::pair<int, int>> flow_node_ports = {
//   {3, 500},
//   {6, 501},
//   {6, 502},
//   {3, 503},
//   {3, 504}
// };
std::vector<std::string> colors = {
  "#4daf4a", "#e41a1c", "#377eb8", "#984ea3", "#ff7f00"
};

// int n_flows = 2;
// std::vector<std::string> flows = {
//   "H1->H5", "H3->H5"
// };
// std::vector<std::pair<int, int>> flow_node_ports = {
//   {6, 500},
//   {6, 501}
// };

int n_flows = 2;
std::vector<std::string> flows = {
  "H1->H5", "H4->H8"
};
std::vector<std::pair<int, int>> flow_node_ports = {
  {6, 501},
  {9, 505}
};

void 
 CreatePlot(std::vector<std::vector<std::pair<ll,double>>> &tput)
 {
   std::string fileNameWithNoExtension = "plots/plot_tput_" + cc;
   std::string graphicsFileName        = fileNameWithNoExtension  + "_" + std::to_string(int(total_ns/1e6)) + "_b.png";
   std::string plotFileName            = fileNameWithNoExtension + ".plt";
   std::string plotTitle               = "Throughput v/s Time (" + cc + ")";

   Gnuplot plot (graphicsFileName);
   plot.SetTitle (plotTitle);
   plot.SetTerminal ("png size 1024,768");
  //  plot.SetTerminal ("svg enhanced size 500,500");
   plot.SetLegend ("Time (in ms)", "Throughput (in Gbit/s)");

  //  plot.AppendExtra ("set xrange [0:+"+std::to_string(total_ns/1e6)+"]");
  //  if(resolution==1)
   plot.AppendExtra ("set yrange [0:+25]");

   for(int i=0;i<n_flows;++i)
   {
    Gnuplot2dDataset dataset;
    dataset.SetTitle(flows[i]);
    dataset.SetStyle (Gnuplot2dDataset::LINES); // LINES_POINTS
    std::string lc = "lt rgb \"" + colors[i] + "\"";
    dataset.SetExtra(lc);
    dataset.Add(0,0);
    for(auto p: tput[i])
        dataset.Add(double(p.first)/1e6, p.second);

    plot.AddDataset(dataset);
   }

   std::ofstream plotFile(plotFileName.c_str());

   plot.GenerateOutput(plotFile);

   plotFile.close();

   std::string plot_cmd = "gnuplot " + plotFileName;
   system(plot_cmd.c_str());

   std::cout<<"Plot saved as "<<graphicsFileName<<"\n";
 }

int
main (int argc, char *argv[])
{

  CommandLine cmd (__FILE__);
  cmd.AddValue ("resolution", "Time scale for throughput, 1/2/3 (1: 30ms, 2: 10ms, 3: 1ms)", resolution);
  cmd.AddValue ("max_window_size", "Window size in packets", max_window_size);
  cmd.Parse (argc, argv);

  if(resolution==1) // 30ms
  {
    total_ns = 3e7;
    obs_ns = 1e5;
  }
  else if(resolution==2) // 10ms
  {
    total_ns = 1e7;
    obs_ns = 1e5;
  }
  else{ // 1ms
    total_ns = 1e6;
    obs_ns = 4e4;
  }
  n_points = int(total_ns/obs_ns);

  std::vector<std::vector<std::pair<ll,double>>> tput(n_flows);

  for(int i=0;i<n_flows;++i)
  {
    // count packets
    std::ifstream file; 
    const char* csv_file = csv_file_str.c_str();
    file.open(csv_file);

    std::queue<std::pair<ll,int>> q;
    int base_ns = 0;
    int sum_bytes = 0;

    ll time_ns;
    int node, port;
    int packet_size;
    while(file>>time_ns)
    {
      file>>node>>port>>packet_size;
      time_ns -= 2e9;
      if(node == flow_node_ports[i].first && port == flow_node_ports[i].second)
        if(time_ns <= total_ns)
          {
            q.push({time_ns, packet_size});
            sum_bytes += packet_size;
            if((int)q.size() > max_window_size)
            {
              auto p = q.front();
              sum_bytes -= p.second;
              base_ns = p.first;
              q.pop();
            }
            double tput_cur = sum_bytes * 8 / double(time_ns - base_ns);
            tput[i].push_back({time_ns, tput_cur});
          }
    }
  }
  
  CreatePlot(tput);
}
