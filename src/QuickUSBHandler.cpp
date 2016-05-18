#include "QuickUSBHandler.h"

QuickUSBHandler::QuickUSBHandler() :
  fIsStopping(false), fHandle(0)
{}

QuickUSBHandler::~QuickUSBHandler()
{
  int ret;
  if (fHandle!=NULL) {
    if ((ret=QuickUsbClose(fHandle))!=0) {
      std::ostringstream o;
      o << "Failed to release interface! Returned value: " << ret;
      throw Exception(__PRETTY_FUNCTION__, o.str(), Fatal);
    }
  }
  fIsStopping = true;
}

void
QuickUSBHandler::Init()
{
  char names[120];
  int res = QuickUsbFindModules(names, 120);
  
  // Check for no modules and bail if we don't find any
  if (*names=='\0') throw Exception(__PRETTY_FUNCTION__, "Couldn't find any QuickUSB module on this computer!", Fatal);

  // Print out the name of each module found
  char* namePtr = names;
  std::ostringstream os;
  while (*namePtr!='\0') {
    os << "Found " << namePtr << "\n\t";
    namePtr += (strlen(namePtr)+1);
  }

  // Open the first device
  res = QuickUsbOpen(&fHandle, names);
  if (res==0) {
    std::ostringstream e; e << "Cannot open " << names;
    throw Exception(__PRETTY_FUNCTION__, e.str().c_str());
  }

  Version fw = GetFWVersion(), driver = GetDriverVersion(), dll = GetDLLVersion();
  os << "FW version: " << fw.MajorVersion << "." << fw.MinorVersion << "." << fw.BuildVersion << "\n\t"
     << "Driver version: " << driver.MajorVersion << "." << driver.MinorVersion << "." << driver.BuildVersion << "\n\t"
     << "DLL version: " << dll.MajorVersion << "." << dll.MinorVersion << "." << dll.BuildVersion;
  PrintInfo(os.str());
  
}

QuickUSBHandler::Version
QuickUSBHandler::GetFWVersion() const
{
  QWORD major, minor, build;
  QuickUsbGetFirmwareVersion(fHandle, &major, &minor, &build);

  Version out;
  out.MajorVersion = major;
  out.MinorVersion = minor;
  out.BuildVersion = build;
  return out;
}

QuickUSBHandler::Version
QuickUSBHandler::GetDriverVersion() const
{
  QWORD major, minor, build;
  QuickUsbGetDriverVersion(&major, &minor, &build);

  Version out;
  out.MajorVersion = major;
  out.MinorVersion = minor;
  out.BuildVersion = build;
  return out;
}

QuickUSBHandler::Version
QuickUSBHandler::GetDLLVersion() const
{
  QWORD major, minor, build;
  QuickUsbGetDllVersion(&major, &minor, &build);

  Version out;
  out.MajorVersion = major;
  out.MinorVersion = minor;
  out.BuildVersion = build;
  return out;
}

void
QuickUSBHandler::Reset() const
{
}

void
QuickUSBHandler::Write(uint16_t addr, std::vector<uint8_t>& words, uint16_t size) const
{
  uint8_t* data = &words[0];
  int result = QuickUsbWriteCommand(fHandle, addr, data, size);
  if (result==0) {
    unsigned long error;
    QuickUsbGetLastError(&error);
    std::ostringstream os;
    os << "Cannot write register 0x" << std::hex << addr << " on board " << std::dec << fHandle << "\n\t"
       << "QuickUSB error: " << error;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
}

std::vector<uint8_t>
QuickUSBHandler::Fetch(uint16_t addr, uint16_t size) const
{
  std::vector<uint8_t> out;
  uint8_t* data = new uint8_t[size];
  int result = QuickUsbReadCommand(fHandle, addr, data, &size);
  if (result==0) {
    unsigned long error;
    QuickUsbGetLastError(&error);
    std::ostringstream os;
    os << "Cannot fetch register 0x" << std::hex << addr << " on board " << std::dec << fHandle << "\n\t"
       << "QuickUSB error: " << error;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
  for (unsigned short i=0; i<size; i++) { out.push_back(data[i]); }
  delete [] data;
  if (out.size()!=size) {
    std::ostringstream os;
    os << "Mismatch between the requested and retrieved words sizes: asked " << size << " and got " << out.size();
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
  return out;
}

void
QuickUSBHandler::StartBulkTransfer(QVOIDRETURN callback(PQBULKSTREAM))
{
  const unsigned int num_buffers = 8, buffer_byte_size = 0x10000;
  int result = QuickUsbReadBulkDataStartStream(fHandle, 0, num_buffers, buffer_byte_size, callback, 0, &fStreamId, 8, 4);
  if (result==0) {
    unsigned long error;
    QuickUsbGetLastError(&error);
    std::ostringstream os;
    os << "Cannot start the bulk transfer on the QuickUSB device " << std::dec << fHandle << "\n\t"
       << "QuickUSB error: " << error;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
}

void
QuickUSBHandler::StopBulkTransfer()
{
  int result = QuickUsbStopStream(fHandle, fStreamId, false);
  if (result==0) {
    unsigned long error;
    QuickUsbGetLastError(&error);
    std::ostringstream os;
    os << "Cannot stop the bulk transfer with stream ID " << std::dec << fStreamId
       << " on the QuickUSB device " << std::dec << fHandle << "\n\t"
       << "QuickUSB error: " << error;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
}
