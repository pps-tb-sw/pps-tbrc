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
  const unsigned int num_channels = 32;
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

  unsigned int num_hits[num_channels];
  for (unsigned int i=0; i<num_channels; i++) num_hits[i] = 0;

  try {
    const unsigned long tdc_address = 0x00ee0000;

    vme = new VMEReader("/dev/usb/v1718_0", VME::CAEN_V1718, false);

    try { vme->AddTDC(tdc_address); } catch (Exception& e) {
      if (e.ErrorNumber()!=TDC_ACQ_START) e.Dump();
    }
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
        const VME::TDCEvent::EventType type = e->GetType();
        if (type==VME::TDCEvent::TDCHeader && (e->GetEventId()%100==0)) {
          cerr << "----> Received trigger #" << e->GetEventId() << endl;
        }
        if (type==VME::TDCEvent::TDCMeasurement) {
          const unsigned int channel_id = e->GetChannelId();
          if (!e->IsTrailing()) {
            num_hits[channel_id]++;
            std::cout << "--> new hit on channel " << channel_id << " :: total on this channel: " << num_hits[channel_id] << std::endl;
          }
          else { // trailing edge
            //std::cout << "trailing edge for channel " << channel_id << std::endl;
          }
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
             << "Local time: " << ctime(&t_end)
             << "Total acquisition time: " << difftime(t_end, t_beg) << " seconds"
             << " (" << nmin << " min " << nsec << " sec)"
             << endl;

        unsigned int num_hits_allchannels = 0;
        for (unsigned int i=0; i<num_channels; i++) { num_hits_allchannels += num_hits[i]; }

        ofstream log("last_run.log");
        log << "Last run stopped at local time: " << ctime(&t_end)
            << "Total acquisition time: " << difftime(t_end, t_beg) << " seconds"
            << " (" << nmin << " min " << nsec << " sec)"
            << endl
            << "Acquired " << num_events << " word(s) in this run, corresponding to " << num_hits_allchannels << " leading edge(s) in all channels" << endl
            << "Leading edges per channel:" << endl;
        for (unsigned int i=0; i<num_channels/2; i++) {
          log << " --> Channel " << setw(2) << i << ": " << setw(5) << num_hits[i]
              << "  |  Channel " << setw(2) << (i+num_channels/2) << ": " << setw(5) << num_hits[i+num_channels/2] << endl;
        }
        log.close();

        cout << endl << "Acquired " << num_events << " word(s) in this run, corresponding to " << num_hits_allchannels << " leading edge(s) in all channels" << endl
             << "Leading edges per channel:" << endl;
        for (unsigned int i=0; i<num_channels/2; i++) {
          cout << " --> Channel " << setw(2) << i << ": " << setw(5) << num_hits[i]
               << "  |  Channel " << setw(2) << (i+num_channels/2) << ": " << setw(5) << num_hits[i+num_channels/2] << endl;
        }
    
        delete vme;
      } catch (Exception& e) { e.Dump(); }
      return 0;
    }
    e.Dump();
    return -1;
  }
    
  return 0;
}
