#include "VMEReader.h"
#include "FileConstants.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <signal.h>

#define NUM_TRIG_BEFORE_FILE_CHANGE 8000
//#define NUM_TRIG_BEFORE_FILE_CHANGE 1

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

  std::string output_filename = "events";
  if (argc>1) output_filename = argv[1];

  const VME::AcquisitionMode acq_mode = VME::TRIG_MATCH;
  const VME::DetectionMode det_mode = VME::TRAILEAD;
  
  ofstream out_file;
  const unsigned int num_channels = 32;
  unsigned int num_events = 0;
  VME::TDCV1x90* tdc = NULL;
  VME::TDCEventCollection ec;

  time_t t_beg = time(0);
  unsigned long num_triggers = 0, num_all_triggers = 0, num_files = 0;

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
    fstream out_file;
    int num_triggers_in_files;

    t_beg = time(0);

    // Initial dump of the acquisition parameters before writing the files
    cerr << endl << "*** Ready for acquisition! ***" << endl;

    // Change outputs file once a minimal amount of triggers is hit
    while (true) {
      time_t start = time(0);
      num_triggers_in_files = 0;

      fh.spill_id = num_files;

      // TDC output files configuration
      fh.acq_mode = tdc->GetAcquisitionMode();
      fh.det_mode = tdc->GetDetectionMode();

      ostringstream filename;
      filename << output_filename
               << "_" << start
               << "_part" << num_files
               //<< "_" << GenerateString(4)
               << ".dat";
      out_file.open(filename.str().c_str(), fstream::out | ios::binary);
      if (!out_file.is_open()) {
        throw Exception(__PRETTY_FUNCTION__, "Error opening file", Fatal);
      }
      out_file.write((char*)&fh, sizeof(file_header_t));
      
      // Data readout from the two TDC boards
      unsigned long nt = 0;
      unsigned long last_trigger = 0;
      unsigned short num_trigger_overflow = 0;
      uint32_t word;
      while (true) {
        ec = tdc->FetchEvents();
        if (ec.size()==0) continue; // no events were fetched
        for (VME::TDCEventCollection::const_iterator e=ec.begin(); e!=ec.end(); e++) {
          const VME::TDCEvent::EventType type = e->GetType();
          if (type==VME::TDCEvent::TDCHeader and e->GetTDCId()==0) {
            nt = e->GetEventId(); // retrieve the trigger number
            if (nt<last_trigger) num_trigger_overflow++;
            nt += num_trigger_overflow*4096;
            last_trigger = nt;
            if (nt!=num_triggers) num_triggers = nt;
            cerr << "------> Received trigger #" << (num_triggers+1) << endl;
          }
          if (type==VME::TDCEvent::TDCMeasurement) {
            const unsigned int channel_id = e->GetChannelId();
            if (!e->IsTrailing()) { // only print the leading edge
              num_hits[channel_id]++;
              std::cout << "--> new hit on channel " << channel_id << " :: total on this channel: " << num_hits[channel_id] << std::endl;
            }
          }
          word = e->GetWord();
          out_file.write((char*)&word, sizeof(uint32_t));
          //e->Dump();
          //if (e->GetType()==VME::TDCEvent::TDCMeasurement) cout << "----> (board " << dec << i << " with address " << hex << atdc->first << dec << ") new event on channel " << e->GetChannelId() << endl;
        }
        num_events += ec.size();
        num_triggers_in_files = num_triggers-num_all_triggers;
          cerr << "--> " << num_triggers << " triggers acquired in this run so far" << endl;
        if (num_triggers_in_files>0 and num_triggers_in_files>=NUM_TRIG_BEFORE_FILE_CHANGE) {
          num_all_triggers = num_triggers;
          break; // break the infinite loop to write and close the current file
        }
      }
      num_files += 1;
      cerr << "---> " << num_triggers_in_files << " triggers written in current TDC output files" << endl;
      if (out_file.is_open()) out_file.close();
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
