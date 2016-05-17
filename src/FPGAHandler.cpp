#include "FPGAHandler.h"

FPGAHandler::FPGAHandler(int port, const char* dev) :
  Client(port), QuickUSBHandler(),
  fFilename(""), fIsFileOpen(false),
  fIsTDCInReadout(false)
{
  try {
    QuickUSBHandler::Init();
    RegisterTest();
    SendSetupWord();
    RetrieveSetupWord();
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
FPGAHandler::SendSetupWord() const
{
  TDCSetup setup;
  setup.DumpRegister();

  const unsigned short size1 = 50, size2 = 31;

  TDCRegister::word_t* word = setup.GetWords();
  std::vector<uint8_t> part1, part2;
  part1.push_back(0x50); // we set the register to write
  for (unsigned int i=0; i<setup.GetNumWords(); i++) {
    if (i<50) part1.push_back(word[i]);
    else part2.push_back(word[i]);
  }

  //for (unsigned int i=0; i<part1.size(); i++) { std::cout << "--> " << std::dec << i << " :: " << std::hex << static_cast<unsigned short>(part1[i]) << std::endl; }
  try {
    QuickUSBHandler::Write(0x0, part1, size1+1);   usleep(100000);
    QuickUSBHandler::Write(size1+1, part2, size2); usleep(100000);
  } catch (Exception& e) { e.Dump(); }
}

void
FPGAHandler::RetrieveSetupWord() const
{
  std::vector<uint8_t> part1, part2;
  try {
    QuickUSBHandler::Write(0x0, 0x51);     usleep(100000);
    part1 = QuickUSBHandler::Fetch(1, 50); usleep(100000);
    part2 = QuickUSBHandler::Fetch(51, 31);
  } catch (Exception& e) { e.Dump(); }
  for (unsigned int i=0; i<part1.size(); i++) {
    std::cout << std::dec << " setup1[" << i << "] = 0x" << std::hex << static_cast<unsigned int>(part1[i]) << std::endl;
  }
  for (unsigned int i=0; i<part2.size(); i++) {
    std::cout << std::dec << " setup2[" << i << "] = 0x" << std::hex << static_cast<unsigned int>(part2[i]) << std::endl;
  }
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

