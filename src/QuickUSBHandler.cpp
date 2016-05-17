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
  while (*namePtr!='\0') {
    std::cout << "Found " << namePtr << std::endl;
    namePtr += (strlen(namePtr)+1);
  }

  // Open the first device
  res = QuickUsbOpen(&fHandle, names);
  if (res==0) {
    std::ostringstream e; e << "Cannot open " << names;
    throw Exception(__PRETTY_FUNCTION__, e.str().c_str());
  }
  
}

void
QuickUSBHandler::Reset() const
{
}

void
QuickUSBHandler::Write(uint16_t addr, const std::vector<uint16_t>& words, uint8_t size) const
{
}


std::vector<uint16_t>
QuickUSBHandler::Fetch(uint16_t addr, uint8_t size) const
{
  std::vector<uint16_t> out;
  return out;
}
