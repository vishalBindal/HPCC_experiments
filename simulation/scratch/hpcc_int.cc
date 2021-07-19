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

NS_LOG_COMPONENT_DEFINE ("Task1_plot");

std::string cc = "HPCC";

int resolution = 1;
ll total_ns = 3e7;

std::string csv_file_str = "/home/vbindal/High-Precision-Congestion-Control/simulation/log_cleaned.txt";

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
 CreatePlotSrate(std::vector<std::vector<std::pair<ll, double>>> &srates)
 {
   std::string fileNameWithNoExtension = "plots/plot_srate_" + cc;
   std::string graphicsFileName        = fileNameWithNoExtension  + "_" + std::to_string(int(total_ns/1e6)) + ".png";
   std::string plotFileName            = fileNameWithNoExtension + ".plt";
   std::string plotTitle               = "Sending rate v/s Time (" + cc + ")";

   Gnuplot plot (graphicsFileName);
   plot.SetTitle (plotTitle);
   plot.SetTerminal ("png size 1024,768");
  //  plot.SetTerminal ("svg enhanced size 500,500");
   plot.SetLegend ("Time (in ms)", "Sending rate (in Gbit/s)");

   plot.AppendExtra ("set xrange [0:+"+std::to_string(total_ns/1e6)+"]");
  //  if(resolution==1)
   plot.AppendExtra ("set yrange [0:+25]");

   for(int i=0;i<n_flows;++i)
   {
     if(srates[i].size()>0)
     {
       Gnuplot2dDataset dataset;
      dataset.SetTitle(flows[i]);
      dataset.SetStyle (Gnuplot2dDataset::LINES); // LINES_POINTS
      std::string lc = "lt rgb \"" + colors[i] + "\"";
      dataset.SetExtra(lc);
      // dataset.Add(0,25.0);
      for(auto p: srates[i])
          dataset.Add(p.first/1e6, p.second);

      plot.AddDataset(dataset);
     }
   }

   std::ofstream plotFile(plotFileName.c_str());

   plot.GenerateOutput(plotFile);

   plotFile.close();

   std::string plot_cmd = "gnuplot " + plotFileName;
   system(plot_cmd.c_str());

   std::cout<<"Srate plot saved as "<<graphicsFileName<<"\n";
 }

void 
 CreatePlotQ(std::vector<std::vector<std::pair<int,int>>> &q, int q_no)
 {
   std::string fileNameWithNoExtension = "plots/plot_q_" + std::to_string(q_no);
   std::string graphicsFileName        = fileNameWithNoExtension  + "_" + std::to_string(int(total_ns/1e6)) + ".png";
   std::string plotFileName            = fileNameWithNoExtension + ".plt";
   std::string plotTitle               = "Queue size v/s Time (" + std::to_string(q_no) + ")";

   Gnuplot plot (graphicsFileName);
   plot.SetTitle (plotTitle);
   plot.SetTerminal ("png size 1024,768");
   plot.SetLegend ("Timestamp", "Queue size (in bytes)");

   for(int i=0;i<n_flows;++i)
   {
     if(q[i].size()>0)
     {
       Gnuplot2dDataset dataset;
      dataset.SetTitle(flows[i]);
      dataset.SetStyle(Gnuplot2dDataset::LINES); // LINES_POINTS
      std::string lc = "lt rgb \"" + colors[i] + "\"";
      dataset.SetExtra(lc);
      for(auto p: q[i])
        dataset.Add(p.first, p.second);

      plot.AddDataset(dataset);
     }
   }

   std::ofstream plotFile(plotFileName.c_str());

   plot.GenerateOutput(plotFile);

   plotFile.close();

   std::string plot_cmd = "gnuplot " + plotFileName;
   system(plot_cmd.c_str());

   std::cout<<"Q "<<q_no<<" plot saved as "<<graphicsFileName<<"\n";
 }

int
main (int argc, char *argv[])
{

  CommandLine cmd (__FILE__);
  cmd.AddValue ("resolution", "Time scale: 1/2/3 (1: 30ms, 2: 10ms, 3: 1ms)", resolution);
  cmd.Parse (argc, argv);

  if(resolution==1)
    total_ns = 3e7;
  else if(resolution==2)
    total_ns = 1e7;
  else total_ns = 1e6;

  std::vector<std::vector<std::pair<ll, double>>> srates(n_flows);
  std::vector<std::vector<std::pair<int,int>>> q1(n_flows);
  std::vector<std::vector<std::pair<int,int>>> q2(n_flows);

  int line_length = 7;
  std::vector<std::string> line(line_length);

  for(int i=0;i<n_flows;++i)
  {
    std::ifstream file; 
    const char* csv_file = csv_file_str.c_str();
    file.open(csv_file);

    bool init_rate = false;
    ll time_ns;
    while(file>>time_ns)
    {
        for(int j=0;j<line_length;++j)
            file>>line[j];
        if(line[1] == flows[i])
            if(time_ns <= total_ns)
            {
                int pos = 0;
                while(line[2][pos] != '>') ++pos;
                ++pos;
                if(!init_rate)
                {
                  double init_rate_val = stod(line[2].substr(5, pos-2));
                  init_rate = true;
                  srates[i].push_back({0, init_rate_val});
                }
                double currate = stod(line[2].substr(pos, 10));
                srates[i].push_back({time_ns, currate});
                if(i<4)
                {
                  q1[i].push_back({stoi(line[4]), stoi(line[3])});
                  if(line[5] != "-1")
                    q2[i].push_back({stoi(line[6]), stoi(line[5])});
                }
                else 
                {
                  q2[i].push_back({stoi(line[4]), stoi(line[3])});
                  q1[i].push_back({stoi(line[6]), stoi(line[5])});
                }
            }
    }
    file.close();
    std::sort(q1[i].begin(), q1[i].end());
    std::sort(q2[i].begin(), q2[i].end());
  }
  
  CreatePlotSrate(srates);
  CreatePlotQ(q1, 1);
  CreatePlotQ(q2, 2);
}
