#include <iostream>

#include "FileReader.h"

#include "PPSCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TLegend.h"
#include "TStyle.h"

using namespace std;
using namespace VME;

int
main(int argc, char* argv[])
{
  unsigned int channel_id = 1;
  if (argc<2) {
    cerr << "Usage:\n\t" << argv[0] << " <input file>" << endl;
    return -1;
  }
  if (argc>=3) {
    channel_id = atoi(argv[2]);
  }

  TDCMeasurement m;
  unsigned int num_events, num_triggers;
  TH1D* hist_lead = new TH1D("lead", "", 500, 0., 500.);
  TH1D* hist_trail = new TH1D("trail", "", 500, 0., 500.);
  TH1D* hist_strangetot_lead = new TH1D("strangetot_lead", "", 500, 0., 500.);
  TH1D* hist_strangetot_trail = new TH1D("strangetot_trail", "", 500, 0., 500.);
  TH1D* hist_lead_zoom = new TH1D("lead_zoom", "", 600, 255., 325.);
  TH1D* hist_trail_zoom = new TH1D("trail_zoom", "", 600, 255., 325.);
  TH1D* hist_lead_no_trail = new TH1D("leading_no_trail", "", 2, 0., 2.);
  TH1D* hist_lead_no_trail_leadingedge = new TH1D("leading_no_trail_leadingedge", "", 500, 0, 500.);
  TH1D* hist_lead_trail_leadingedge = new TH1D("leading_trail_leadingedge", "", 500, 0, 500.);
  TH1D* hist_tot = new TH1D("tot", "", 500, 0., 100.);
  TH1D* hist_numevts = new TH1D("nevts", "", 10, -.5, 9.5);
  
  FileReader f(argv[1]);
  cout << "Run/burst id: " << f.GetRunId() << " / " << f.GetBurstId() << endl;
  cout << "Acquisition mode: " << f.GetAcquisitionMode() << endl;
  cout << "Detection mode: " << f.GetDetectionMode() << endl;
  //cout << f.GetNumTDCs() << " TDCs recorded" << endl;
  num_triggers = num_events = 0;
  while (true) {
    try {
      if (!f.GetNextMeasurement(channel_id, &m)) break;
      //m.Dump();
      for (unsigned int i=0; i<m.NumEvents(); i++) {
        //std::cout << "--> " << (m.GetToT(i)*25./1024.) << std::endl;
        hist_lead->Fill(m.GetLeadingTime(i)*25./1024.);
        hist_trail->Fill(m.GetTrailingTime(i)*25./1024.);
        hist_lead_zoom->Fill(m.GetLeadingTime(i)*25./1024.);
        hist_trail_zoom->Fill(m.GetTrailingTime(i)*25./1024.);
        hist_lead_no_trail->Fill(m.HasTrailingEdge(i));
	if (!m.HasTrailingEdge(i)) hist_lead_no_trail_leadingedge->Fill(m.GetLeadingTime(i)*25./1024.);
        else                       hist_lead_trail_leadingedge->Fill(m.GetLeadingTime(i)*25./1024.);
	hist_tot->Fill(m.GetToT(i)*25./1024.);
        if (m.GetToT(i)*25./1024>40.) {
          cout << "-----> " << m.GetToT(i)*25./1024 << " -----> " << i << " -- " << m.GetLeadingTime(i)*25./1024 << endl;
          hist_strangetot_lead->Fill(m.GetLeadingTime(i)*25./1024.);
          hist_strangetot_trail->Fill(m.GetTrailingTime(i)*25./1024.);
        }
        //std::cout << "ettt=" << m.GetETTT() << std::endl;
      }
      num_events += m.NumEvents();
      hist_numevts->Fill(m.NumEvents()-.5);
      num_triggers += 1;
    } catch (Exception& e) { e.Dump(); }
  }
  cerr << "total number of triggers: " << num_triggers << endl;
  cerr << "mean number of events per trigger in channel " << channel_id << ": " << ((float)num_events/num_triggers) << endl;
 
  gStyle->SetPadGridX(true); gStyle->SetPadGridY(true);
  gStyle->SetPadTickX(true); gStyle->SetPadTickY(true);
 
  DQM::PPSCanvas c_time("dist_edgetime");
  hist_lead->Draw();
  c_time.AddLegendEntry(hist_lead, "Leading edge");
  hist_trail->Draw("same");
  hist_trail->SetLineColor(kRed+1);
  c_time.AddLegendEntry(hist_trail, "Trailing edge");
  hist_lead->GetXaxis()->SetTitle("Hit edge time (ns)");
  hist_lead->GetYaxis()->SetTitle(Form("Events in channel %d",channel_id));
  c_time.Save("png", "plots/");

  gStyle->SetOptStat(0);
  DQM::PPSCanvas c_strangetot_time("dist_strangetot_edgetime");
  hist_strangetot_lead->Draw();
  c_strangetot_time.AddLegendEntry(hist_strangetot_lead, "Leading edge");
  hist_strangetot_trail->Draw("same");
  hist_strangetot_trail->SetLineColor(kRed+1);
  c_strangetot_time.AddLegendEntry(hist_strangetot_trail, "Trailing edge");
  hist_strangetot_lead->GetXaxis()->SetTitle("Hit edge time (ns) (ToT > 40 ns)");
  hist_strangetot_lead->GetYaxis()->SetTitle(Form("Events in channel %d",channel_id));
  c_strangetot_time.Save("png", "plots/");

  gStyle->SetOptStat(0);
  DQM::PPSCanvas c_time_zoom("dist_edgetime_zoom");
  hist_lead_zoom->Draw();
  hist_trail_zoom->Draw("same");
  hist_trail_zoom->SetLineColor(kRed+1);
  hist_lead_zoom->GetXaxis()->SetTitle("Hit edge time (ns)");
  hist_lead_zoom->GetYaxis()->SetTitle(Form("Events in channel %d",channel_id));
  /*hist_lead_zoom->Fit("gaus", "0");
  hist_trail_zoom->Fit("gaus", "0");
  TF1* f1 = (TF1*)hist_lead_zoom->GetFunction("gaus");
  TF1* f2 = (TF1*)hist_trail_zoom->GetFunction("gaus");
  leg2->AddEntry(hist_lead_zoom, Form("Leading edge  #mu=%.3g, #sigma=%.3g",f1->GetParameter(1),f1->GetParameter(2)), "l");
  leg2->AddEntry(hist_trail_zoom, Form("Trailing edge    #mu=%.3g, #sigma=%.3g",f2->GetParameter(1),f2->GetParameter(2)), "l");
  leg2->Draw();*/
  c_time_zoom.Save("png", "plots/");
  c_time_zoom.Save("pdf", "plots/");
  cout << "integral: " << hist_lead_zoom->Integral() << " / " << hist_trail_zoom->Integral() << endl;

  gStyle->SetOptStat(1111);
  DQM::PPSCanvas c_tot("dist_tot");
  hist_tot->Draw();
  hist_tot->GetXaxis()->SetTitle("Time over threshold (ns)");
  hist_tot->GetYaxis()->SetTitle(Form("Events in channel %d",channel_id));
  hist_tot->GetYaxis()->SetTitleOffset(1.45);
  c_tot.Save("png", "plots/");
  c_tot.Save("pdf", "plots/");

  DQM::PPSCanvas c_nevts("dist_nevts");
  hist_numevts->Draw();
  hist_numevts->GetXaxis()->SetTitle(Form("Hits multiplicity in channel %d / trigger",channel_id));
  hist_numevts->GetYaxis()->SetTitle("Triggers");
  c_nevts.Save("png", "plots/");

  DQM::PPSCanvas c_fraction_no_trailing("dist_fraction_no_trailing_edge");
  hist_lead_no_trail->Draw();
  hist_lead_no_trail->GetXaxis()->SetTitle("Has trailing edge?");
  hist_lead_no_trail->GetYaxis()->SetTitle(Form("Events in channel %d", channel_id));
  hist_lead_no_trail->SetMinimum(0.5);
  c_fraction_no_trailing.Save("png", "plots/");

  DQM::PPSCanvas c_no_trailing_leadingedge("dist_leadingedge_no_trailing_edge");
  hist_lead_trail_leadingedge->Draw();
  hist_lead_no_trail_leadingedge->Draw("same");
  c_no_trailing_leadingedge.AddLegendEntry(hist_lead_no_trail_leadingedge, "No trailing edge in event");
  hist_lead_trail_leadingedge->SetLineColor(kRed);
  c_no_trailing_leadingedge.AddLegendEntry(hist_lead_trail_leadingedge, "Trailing edge in event");
  hist_lead_trail_leadingedge->GetXaxis()->SetTitle("Leading edge (ns)");
  c_no_trailing_leadingedge.Save("png", "plots/");

  return 0;
}
