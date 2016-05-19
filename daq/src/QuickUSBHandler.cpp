#include "QuickUSBHandler.h"

namespace DAQ
{
  QuickUSBHandler::QuickUSBHandler() :
    fIsStopping(false), fHandle(0)
  {}
  
  QuickUSBHandler::~QuickUSBHandler()
  {
    int ret;
    if (fHandle!=0) {
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

    Configure();
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
  QuickUSBHandler::Configure() const
  {
    //QuickUsbReadDefault(fHandle, kFIFOConfig, &bitvalue);

    std::ostringstream os;
    try {
      SetWordWide(k8bits);
      //SetDataAddress(0x1ff, true, true); // increment ; enable address bus
      SetFIFOConfig(0x82); // 0x82 = 0b0000.0000.1000.0010
      SetCPUConfig((kCLKOUTdisable|kCLKINVenable|kCLKSPD12MHz|kUSBFullSpeedAllow)+0x10); // 0x8012 = 0b1000.0000.0001.0010
      os << "Hardware revision: " << GetHWRevision() << "\n\t"
         << "FPGA type: " << GetFPGAType() << "\n\t"
         << "Slave FIFO flags:" << "\n\t"
         << GetSlaveFIFOFlags() << "\n\t"
         << "Timeouts: low = " << GetTimeoutLow() << " ms / high = " << GetTimeoutHigh() << " ms\n\t"
         << "USB bus speed: " << GetUSBSpeed();
    } catch (Exception& e) { e.Dump(); }

    PrintInfo(os.str());
    /*unsigned short bitvalue;
    QuickUsbReadSetting(fHandle, kWordWide, &bitvalue);
    QuickUsbReadSetting(fHandle, kDataAddress, &bitvalue);
    QuickUsbReadSetting(fHandle, kFIFOConfig, &bitvalue);
    QuickUsbReadSetting(fHandle, kFPGAType, &bitvalue);
    QuickUsbReadSetting(fHandle, kCPUConfig, &bitvalue);
    QuickUsbReadSetting(fHandle, kSPIConfig, &bitvalue);
    QuickUsbReadSetting(fHandle, kSlaveFIFOFlags, &bitvalue);
    QuickUsbReadSetting(fHandle, kI2CTL, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortA, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortB, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortC, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortD, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortE, &bitvalue);
    QuickUsbReadSetting(fHandle, kPortAConfig, &bitvalue);
    QuickUsbReadSetting(fHandle, kPinFlags, &bitvalue);
    QuickUsbReadSetting(fHandle, kVersionSpeed, &bitvalue);
    QuickUsbReadSetting(fHandle, kTimeoutHigh, &bitvalue);
    QuickUsbReadSetting(fHandle, kTimeoutLow, &bitvalue);*/
  }
  
  void
  QuickUSBHandler::Write(uint16_t addr, std::vector<uint8_t>& words, uint16_t size) const
  {
    SetWordWide(k8bits);

    uint8_t* data = &words[0];
    int result = QuickUsbWriteCommand(fHandle, addr, data, size);
    if (result==0) {
      unsigned long error;
      QuickUsbGetLastError(&error);
      std::ostringstream os;
      os << "Cannot write register 0x" << std::hex << addr << " on board " << std::dec << static_cast<unsigned short>(fHandle) << "\n\t"
         << "QuickUSB error: " << error;
      throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
    }
  }
  
  std::vector<uint8_t>
  QuickUSBHandler::Fetch(uint16_t addr, uint16_t size) const
  {
    SetWordWide(k8bits);

    std::vector<uint8_t> out;
    uint8_t* data = new uint8_t[size];
    int result = QuickUsbReadCommand(fHandle, addr, data, &size);
    if (result==0) {
      unsigned long error;
      QuickUsbGetLastError(&error);
      std::ostringstream os;
      os << "Cannot fetch register 0x" << std::hex << addr << " on board " << std::dec << static_cast<unsigned short>(fHandle) << "\n\t"
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

    QuickUsbSetTimeout(fHandle, 1000);
    SetWordWide(k16bits);

    bool increment, enable_addr_bus;
    SetDataAddress(200, increment=false, enable_addr_bus=true);

    int result = QuickUsbReadBulkDataStartStream(fHandle, 0, num_buffers, buffer_byte_size, callback, 0, &fStreamId, 8, 4);
    if (result==0) {
      unsigned long error;
      QuickUsbGetLastError(&error);
      std::ostringstream os;
      os << "Cannot start the bulk transfer on the QuickUSB device " << std::dec << static_cast<unsigned short>(fHandle) << "\n\t"
         << "QuickUSB error: " << error;
      throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
    }
  }
  
  void
  QuickUSBHandler::StopBulkTransfer()
  {
    int result;

    // first we shut down the stream
    result = QuickUsbStopStream(fHandle, fStreamId, true);
    if (result==0) {
      unsigned long error;
      QuickUsbGetLastError(&error);
      std::ostringstream os;
      os << "Cannot stop the bulk transfer with stream ID " << std::dec << static_cast<unsigned short>(fStreamId) << "\n\t"
         << "on the QuickUSB device " << std::dec << static_cast<unsigned short>(fHandle) << "\n\t"
         << "QuickUSB error: " << error;
      throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
    }
    result = QuickUsbStopStream(fHandle, fStreamId, false);
    
    if (result!=0) return;

    unsigned long error;
    QuickUsbGetLastError(&error);
    // if the stream has already finished shutting down
    // we continue normally (not an error, the stream
    // is to be stopped)
    if (error!=QUICKUSB_ERROR_NOT_STREAMING) {
      std::ostringstream os;
      os << "Cannot stop the bulk transfer from buffer to file" << "\n\t"
         << "QuickUSB error: " << error;
      throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
    }
  }

  std::ostream&
  operator<<(std::ostream& os, const QuickUSBHandler::HWRevision& rev) {
    switch (rev) {
      case QuickUSBHandler::CY7C68013AB: os << "CY7C68013 Rev A/B"; break;
      case QuickUSBHandler::CY7C68013A:  os << "CY7C68013A Rev A"; break;
      case QuickUSBHandler::CY7C68013CD: os << "CY7C68013 Rev C/D"; break;
      case QuickUSBHandler::CY7C68013E:  os << "CY7C68013 Rev E"; break;
      default:                           os << "Unknown Revision"; break;
    }
    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, const QuickUSBHandler::USBSpeed& sp) {
    switch (sp) {
      case QuickUSBHandler::USBFullSpeed: os << "Full Speed (12 Mbps)"; break;
      case QuickUSBHandler::USBHighSpeed: os << "High Speed (480 Mbps)"; break;
    }
    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, const QuickUSBHandler::FPGAType& t) {
    switch (t) {
      case QuickUSBHandler::AlteraPassiveSerial: os << "Altera Passive Serial"; break;
      case QuickUSBHandler::XilinxSlaveSerial:   os << "Xilinx Slave Serial"; break;
    }
    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, const QuickUSBHandler::FIFOFlags& ff) {
    os << "  EP2 (write) FIFO: full? " << ff.WriteFIFOFull << " / empty? " << ff.WriteFIFOEmpty << "\n\t"
       << "  EP6  (read) FIFO: full? " << ff.ReadFIFOFull << " / empty? " << ff.ReadFIFOEmpty << "\n\t"
       << "  Pins status: RDY1=" << ff.RDY1 << " / RDY0=" << ff.RDY0;
    return os;
  }
}
