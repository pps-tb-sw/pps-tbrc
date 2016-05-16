#include <iostream>
#include <vector>

#include "FileReader.h"

#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

using namespace std;
using namespace VME;

int
main(int argc, char* argv[])
{
  unsigned int channel1_id = 0, channel2_id = 0;
  if (argc<4) {
    cout << "Usage:\n\t" << argv[0] << " <input file:str> <channel 1:int> <channel 2:int>" << endl;
    return -1;
  }
  else {
    channel1_id = atoi(argv[2]);
    channel2_id = atoi(argv[3]);
  }

  TDCMeasurement m;
  int num_events1, num_events2;
  vector<double> time1, time1_ettt, time2, time2_ettt;
  TH1D* h_diff = new TH1D("diff", "", 200, -2., 2.);
  TH1D* h_diff_ettt = new TH1D("diff_ettt", "", 200, -2., 2.);
  
  num_events1 = num_events2 = 0;

  FileReader f1(argv[1]);
  while (true) {
    try {
      if (!f1.GetNextMeasurement(channel1_id, &m)) break;
      time1.push_back(m.GetLeadingTime()*25./1024);
      time1_ettt.push_back(m.GetLeadingTime()*25./1024-m.GetETTT());
      num_events1 += m.NumEvents();
    } catch (Exception& e) {
      e.Dump();
    }
  }
    
  FileReader f2(argv[1]);
  while (true) {
    try {
      if (!f2.GetNextMeasurement(channel2_id, &m)) break;
      time2.push_back(m.GetLeadingTime()*25./1024);
      time2_ettt.push_back(m.GetLeadingTime()*25./1024-m.GetETTT());
      num_events2 += m.NumEvents();
    } catch (Exception& e) {
      e.Dump();
    }
  }
  
  if (time1.size()!=time2.size()) {
    cerr << "Not the same number of events in both channels! aborting..." << endl;
    return -1;
  }

  for (unsigned int i=0; i<time1.size(); i++) {
    h_diff->Fill(time1[i]-time2[i]);
    h_diff_ettt->Fill(time1_ettt[i]-time2_ettt[i]);
  }

  cerr << "number of events:" << endl
       << "  channel " << channel1_id << ": " << num_events1 << " / " << time1.size() << endl
       << "  channel " << channel2_id << ": " << num_events2 << " / " << time2.size() << endl;

  gStyle->SetPadGridX(true); gStyle->SetPadGridY(true);
  gStyle->SetPadTickX(true); gStyle->SetPadTickY(true);
  
  TCanvas* c = new TCanvas;
  TLegend* leg = new TLegend(.15, .75, .4, .85);
  h_diff->Draw();
  leg->AddEntry(h_diff, "No correction");
  h_diff_ettt->Draw("same");
  h_diff_ettt->SetLineColor(kRed+1);
  leg->AddEntry(h_diff_ettt, "ETTT correction");
  h_diff->GetXaxis()->SetTitle(Form("Leading time ch %d - leading time ch %d (ns)",channel1_id,channel2_id));
  h_diff->GetYaxis()->SetTitle("Triggers");
  leg->Draw();
  c->SaveAs("timediff_leading.png");

  return 0;
}
