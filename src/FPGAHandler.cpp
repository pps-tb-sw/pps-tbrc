#include "FPGAHandler.h"

FPGAHandler::FPGAHandler(int port, const char* dev) :
  Client(port), QuickUSBHandler(),
  fFilename(""), fIsFileOpen(false),
  fIsTDCInReadout(false)
{
  try {
    QuickUSBHandler::Init();
    RegisterTest();
  } catch (Exception& e) { e.Dump(); }
}

FPGAHandler::~FPGAHandler()
{
  CloseFile();
  for (unsigned int i=0; i<NUM_HPTDC; i++) {
    //if (fTDC[i]) delete fTDC[i];
  }
}

void
FPGAHandler::RegisterTest() const
{
  std::vector<uint8_t> reg;
  try {
    reg = QuickUSBHandler::Fetch(0x70, 2);
  } catch (Exception& e) { e.Dump(); }
  if (reg[0]!=0x80 or reg[1]!=0x80) {
    std::ostringstream os;
    os << "Register test failed! got 0x" << std::hex << static_cast<unsigned int>(reg[0]) << " and 0x" << std::hex << static_cast<unsigned int>(reg[1]);
    throw Exception(__PRETTY_FUNCTION__, os.str(), Fatal);
  }
  std::ostringstream os;
  os << "Passed the register test (got 0x" << std::hex << static_cast<unsigned int>(reg[0]) << " and 0x" << std::hex << static_cast<unsigned int>(reg[1]) << ")";
  PrintInfo(os.str());
}

void
FPGAHandler::OpenFile()
{
  // Generate a random file name
  srand(time(NULL));
  const char az[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  std::string filename; const size_t len = 5;
  fFilename = "events_";
  //for (size_t i=0; i<len; i++) { fFilename += az[rand()%(sizeof(az)-1)]; } //FIXME commented out for debugging purposes...
  fFilename += ".dat";
  
  std::cout << "Filename: " << fFilename << std::endl;
  
  fOutput.open(fFilename.c_str(), std::fstream::out|std::ios::binary);
  if (!(fIsFileOpen=fOutput.is_open()))
    throw Exception(__PRETTY_FUNCTION__, "Error opening the file! Check you have enough permissions to write!", Fatal);
  
  // First we write the header to the file
  file_header_t th;
  th.magic = 0x30535050; // PPS0 in ASCII
  th.run_id = 0;
  th.spill_id = 0;
  th.num_hptdc = NUM_HPTDC;
  fOutput.write((char*)&th, sizeof(file_header_t));
  
  /*for (unsigned int i=0; i<NUM_HPTDC; i++) {
    TDCSetup s = fTDC[i]->GetSetupRegister();
    fOutput.write((char*)&s, sizeof(TDCSetup));
  }*/
}

int
FPGAHandler::ReadBuffer()
{
  TDCEventCollection ev;
  unsigned int nevts = 0;
  
  if (QuickUSBHandler::fIsStopping) {
    std::cout << __PRETTY_FUNCTION__
              << " QuickUSB handler in a stopping state! Finishing the readout."
              << std::endl;
    return -1;
  }
  
  /*for (unsigned int i=0; i<NUM_HPTDC; i++) {
    ev = fTDC[i]->FetchEvents();
    nevts += ev.size();
    for (TDCEventCollection::iterator e=ev.begin(); e!=ev.end(); e++) {
      fOutput.write((char*)&(*e), sizeof(TDCEvent));
    }
  }*/
  return nevts;
}

void
FPGAHandler::CloseFile()
{
  if (fIsFileOpen) fOutput.close();
}

