#include "VMEReader.h"
#include "FileConstants.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <signal.h>

using namespace std;

VMEReader* vme;
int gEnd = 0;

void CtrlC(int aSig) {
  if (gEnd==0) { cerr << endl << "[C-c] Trying a clean exit!" << endl; vme->Abort(); }
  else if (gEnd>=5) { cerr << endl << "[C-c > 5 times] ... Forcing exit!" << endl; exit(0); }
  gEnd++;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, CtrlC);
  
  ofstream out_file;
  unsigned int num_events; 

  VME::TDCEventCollection ec;

  time_t t_beg = time(0);

  try {
    const bool with_socket = false;
    const unsigned long tdc_address = 0x12340000;

    vme = new VMEReader("/dev/a2818_0", VME::CAEN_V2718, with_socket);
    vme->AddTDC(tdc_address);

    VME::TDCV1x90* tdc = vme->GetTDC(tdc_address);
    tdc->SetAcquisitionMode(VME::TRIG_MATCH);
    tdc->SetDetectionMode(VME::TRAILEAD);

    t_beg = time(0);

    // Initial dump of the acquisition parameters before writing the files
    cerr << endl << "*** Ready for acquisition! ***" << endl;

    out_file.open("events.dat");
    
    // TDC output files configuration
    if (!out_file.is_open()) {
      throw Exception(__PRETTY_FUNCTION__, "Error opening file", Fatal);
    }
      
    // Data readout from the two TDC boards
    uint32_t word;
    num_events = 0;
    while (true) {
      ec = tdc->FetchEvents();
      if (ec.size()==0) continue; // no events were fetched
      for (VME::TDCEventCollection::const_iterator e=ec.begin(); e!=ec.end(); e++) {
        word = e->GetWord();
        out_file.write((char*)&word, sizeof(uint32_t));
      }
      num_events += ec.size();
    }
    if (out_file.is_open()) out_file.close();
  } catch (Exception& e) {
    // If any TDC::FetchEvent method throws an "acquisition stop" message
    if (e.ErrorNumber()==TDC_ACQ_STOP) {
      try {
        if (out_file.is_open()) out_file.close();
  
        time_t t_end = time(0);
        double nsec_tot = difftime(t_end, t_beg), nsec = fmod(nsec_tot,60), nmin = (nsec_tot-nsec)/60.;
        cerr << endl << "*** Acquisition stopped! ***" << endl
             << "Local time: " << asctime(localtime(&t_end))
             << "Total acquisition time: " << difftime(t_end, t_beg) << " seconds"
             << " (" << nmin << " min " << nsec << " sec)"
             << endl;

        cerr << endl << "Acquired " << num_events << " words in this run" << endl;
    
        delete vme;
      } catch (Exception& e) { e.Dump(); }
      return 0;
    }
    e.Dump();
    return -1;
  }
    
  return 0;
}
