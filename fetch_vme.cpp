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

  std::string output_filename = "events.dat";
  if (argc>1) output_filename = argv[1];

  const VME::AcquisitionMode acq_mode = VME::TRIG_MATCH;
  //const VME::AcquisitionMode acq_mode = VME::CONT_STORAGE;
  const VME::DetectionMode det_mode = VME::TRAILEAD;
  
  ofstream out_file;
  unsigned int num_events = 0; 
  VME::TDCV1x90* tdc = NULL;
  VME::TDCEventCollection ec;

  time_t t_beg = time(0);

  file_header_t fh;
  fh.magic = 0x30535050; // PPS0 in ASCII
  fh.run_id = 0;
  fh.spill_id = 0;
  fh.acq_mode = acq_mode;
  fh.det_mode = det_mode;

  unsigned int num_hits[32];
  for (unsigned int i=0; i<32; i++) num_hits[i] = 0;

  try {
    const unsigned long tdc_address = 0x00ee0000;

    vme = new VMEReader("/dev/v1718_0", VME::CAEN_V1718, false);

    try { vme->AddTDC(tdc_address); } catch (Exception& e) { e.Dump(); }
    tdc = vme->GetTDC(tdc_address);
    tdc->SetAcquisitionMode(acq_mode);
    tdc->SetDetectionMode(det_mode);

    if (acq_mode==VME::TRIG_MATCH) {
      tdc->SetWindowWidth(20); // in (25 ns) clock cycles
      tdc->SetWindowOffset(-20);
      tdc->GetTriggerConfiguration().Dump();
    }

    t_beg = time(0);

    // Initial dump of the acquisition parameters before writing the files
    cerr << endl << "*** Ready for acquisition! ***" << endl;

    // TDC output files configuration
    out_file.open(output_filename.c_str());
    if (!out_file.is_open()) { throw Exception(__PRETTY_FUNCTION__, "Error opening file", Fatal); }

    // Write the file header for offline analysis
    out_file.write((char*)&fh, sizeof(file_header_t));
      
    // Data readout from the TDC board
    uint32_t word;
    num_events = 0;
    while (true) {
      if (!tdc) break;
      ec = tdc->FetchEvents();
      if (ec.size()==0) continue; // no events were fetched
      for (VME::TDCEventCollection::const_iterator e=ec.begin(); e!=ec.end(); e++) {
        if (e->GetType()==VME::TDCEvent::TDCMeasurement and !e->IsTrailing()) {
          const unsigned int channel_id = e->GetChannelId();
          num_hits[channel_id]++;
          std::cout << "--> new hit on channel " << channel_id << " :: total on this channel: " << num_hits[channel_id] << std::endl;
        }
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
