#include "FileReader.h"

FileReader::FileReader(std::string file)
{
  Open(file);
}

FileReader::~FileReader()
{
  if (fFile.is_open()) fFile.close();
}

void
FileReader::Open(std::string file)
{
  fFile.open(file.c_str(), std::ios::in|std::ios::binary);
  
  if (!fFile.is_open()) {
    std::stringstream s;
    s << "Error while trying to open the file \""
      << file << "\" for reading!";
    throw Exception(__PRETTY_FUNCTION__, s.str(), JustWarning, 40000);
  }
  
  struct stat st;
  // Retrieve the file size
  if (stat(file.c_str(), &st) == -1) {
    std::stringstream s;
    s << "Error retrieving size of \"" << file << "\"!";
    fFile.close();
    throw Exception(__PRETTY_FUNCTION__, s.str(), JustWarning, 40001);
  }
  
  if (!fFile.good()) {
    fFile.close();
    throw Exception(__PRETTY_FUNCTION__, "Can not read file header!", JustWarning, 40002);
  }
  fFile.read((char*)&fHeader, sizeof(file_header_t));
  fNumEvents = (st.st_size-sizeof(file_header_t))/sizeof(uint32_t);
  if (fHeader.magic!=0x30535050) {
    fFile.close();
    throw Exception(__PRETTY_FUNCTION__, "Wrong magic number!", JustWarning, 40003);
  }
  fWriteTime = st.st_mtime;
  fReadoutMode = fHeader.acq_mode;
}

void
FileReader::Dump() const
{
  std::stringstream s;
  char buff[80];
  strftime(buff, 80, "%c", localtime(&fWriteTime));
  s << "File written on: " << buff << "\n\t"
    << "Run number: " << fHeader.run_id << "\n\t"
    << "Readout mode: " << fHeader.acq_mode << "\n\t"
    << "Number of events: " << fNumEvents;
  PrintInfo(s.str());
}

bool
FileReader::GetNextEvent(TDCEvent* ev)
{
  uint32_t buffer;
  fFile.read((char*)&buffer, sizeof(uint32_t));
  ev->SetWord(buffer);
#ifdef DEBUG
  std::cerr << "Event type: " << ev->GetType();
  if (ev->GetType()==TDCEvent::TDCMeasurement)
    std::cerr << "  channel " << std::setw(2) << ev->GetChannelId() << "  trail? " << ev->IsTrailing();
  std::cerr << std::endl;
#endif
  if (fFile.eof()) return false;
  return true;
}

bool
FileReader::GetNextMeasurement(unsigned int channel_id, TDCMeasurement* mc)
{
  TDCEvent ev;
  std::vector<TDCEvent> ec;
  /*do { GetNextEvent(&ev);
    std::cout << ev.GetChannelId() << " type: " << ev.GetType() << std::endl; 
  } while (ev.GetType()!=TDCEvent::TDCHeader);*/
  /*do {
    GetNextEvent(&ev);
    if (ev.GetChannelId()!=channel_id) continue;
    ec.push_back(ev);
    std::cout << ev.GetType() << std::endl;
  } while (ev.GetType()!=TDCEvent::TDCHeader);*/

  if (fReadoutMode==CONT_STORAGE) {
    bool has_lead = false, has_trail = false, has_error = false;
    while (true) {
      if (!GetNextEvent(&ev)) return false;
      if (ev.GetChannelId()!=channel_id) { continue; }

      ec.push_back(ev);

      switch (ev.GetType()) {
        case TDCEvent::TDCHeader:
        case TDCEvent::TDCTrailer:
          //std::cerr << "Event Id=" << ev.GetEventId() << std::endl;
          break;
        case TDCEvent::TDCMeasurement:
          //std::cerr << "Leading measurement? " << (!ev.IsTrailing()) << std::endl;
          if (ev.IsTrailing()) has_trail = true;
          else has_lead = true;
          break;
        case TDCEvent::GlobalHeader:
        case TDCEvent::GlobalTrailer:
        case TDCEvent::ETTT:
          break;
        case TDCEvent::TDCError:
          has_error = true;
          //std::cerr << " ---> Error flags: " << ev.GetErrorFlags() << std::endl;
          break;
        case TDCEvent::Filler:
        case TDCEvent::Trigger:
          break;
      }
      if (has_lead and has_trail) break;
      if (ev.GetType()==TDCEvent::Trigger) break;
    }
    if (has_error) throw Exception(__PRETTY_FUNCTION__, "Measurement has at least one error word.", JustWarning, 41000);
  }
  else if (fReadoutMode==TRIG_MATCH) {
    bool has_error = false;
    while (true) {
      if (!GetNextEvent(&ev)) return false;
      //ev.Dump();
      /*std::cout << "0x" << std::hex << ev.GetType();
      if (ev.GetType()==TDCEvent::TDCMeasurement) {
        std::cout << "\t" << ev.IsTrailing() << "\t" << ev.GetChannelId();
      }
      std::cout << std::endl;*/

      if (ev.GetType()==TDCEvent::TDCMeasurement) {
        if (ev.GetChannelId()!=channel_id) continue;
      }
      else if (ev.GetType()==TDCEvent::TDCError) {
        has_error = true;
      }
      ec.push_back(ev);

      if (ev.GetType()==TDCEvent::GlobalTrailer) break;
    }
    //if (has_error) throw Exception(__PRETTY_FUNCTION__, "Measurement has at least one error word.", JustWarning, 41000);
  }
  else {
    std::ostringstream os;
    os << "Unrecognized readout/acquisition mode: " << fReadoutMode;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning, 40004);
  }
  try { mc->SetEventsCollection(ec); } catch (Exception& e) { e.Dump(); }
  return true;
}

