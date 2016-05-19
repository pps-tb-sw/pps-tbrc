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
  QuickUSBHandler::DumpConfigValues(std::ostream& os) const
  {
    uint16_t reg;
    os << std::hex;
    try {
      reg = GetConfigRegister(kWordWide);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kWordWide << "\n\t";
      reg = GetConfigRegister(kDataAddress);    os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kDataAddress << "\n\t";
      reg = GetConfigRegister(kFIFOConfig);     os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kFIFOConfig << "\n\t";
      reg = GetConfigRegister(kFPGAType);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kFPGAType << "\n\t";
      reg = GetConfigRegister(kCPUConfig);      os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kCPUConfig << "\n\t";
      reg = GetConfigRegister(kSPIConfig);      os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kSPIConfig << "\n\t";
      reg = GetConfigRegister(kSlaveFIFOFlags); os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kSlaveFIFOFlags << "\n\t";
      reg = GetConfigRegister(kI2CTL);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kI2CTL << "\n\t";
      reg = GetConfigRegister(kPortA);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortA << "\n\t";
      reg = GetConfigRegister(kPortB);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortB << "\n\t";
      reg = GetConfigRegister(kPortC);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortC << "\n\t";
      reg = GetConfigRegister(kPortD);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortD << "\n\t";
      reg = GetConfigRegister(kPortE);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortE << "\n\t";
      reg = GetConfigRegister(kPortAConfig);    os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortAConfig << "\n\t";
      reg = GetConfigRegister(kPinFlags);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPinFlags << "\n\t";
      reg = GetConfigRegister(kVersionSpeed);   os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kVersionSpeed << "\n\t";
      reg = GetConfigRegister(kTimeoutHigh);    os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kTimeoutHigh << "\n\t";
      reg = GetConfigRegister(kTimeoutLow);     os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kTimeoutLow;
    } catch (Exception& e) { e.Dump(); }
    os << std::dec;
  }

  void
  QuickUSBHandler::DumpDefaultConfigValues(std::ostream& os) const
  {
    uint16_t reg;
    os << std::hex;
    try {
      reg = GetDefaultConfigRegister(kWordWide);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kWordWide << "\n\t";
      reg = GetDefaultConfigRegister(kDataAddress);    os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kDataAddress << "\n\t";
      reg = GetDefaultConfigRegister(kFIFOConfig);     os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kFIFOConfig << "\n\t";
      reg = GetDefaultConfigRegister(kFPGAType);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kFPGAType << "\n\t";
      reg = GetDefaultConfigRegister(kCPUConfig);      os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kCPUConfig << "\n\t";
      reg = GetDefaultConfigRegister(kSPIConfig);      os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kSPIConfig << "\n\t";
      reg = GetDefaultConfigRegister(kSlaveFIFOFlags); os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kSlaveFIFOFlags << "\n\t";
      reg = GetDefaultConfigRegister(kI2CTL);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kI2CTL << "\n\t";
      reg = GetDefaultConfigRegister(kPortA);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortA << "\n\t";
      reg = GetDefaultConfigRegister(kPortB);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortB << "\n\t";
      reg = GetDefaultConfigRegister(kPortC);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortC << "\n\t";
      reg = GetDefaultConfigRegister(kPortD);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortD << "\n\t";
      reg = GetDefaultConfigRegister(kPortE);          os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortE << "\n\t";
      reg = GetDefaultConfigRegister(kPortAConfig);    os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPortAConfig << "\n\t";
      reg = GetDefaultConfigRegister(kPinFlags);       os << "  0x" << std::setw(4) << std::setfill('0') << reg << " = " << std::bitset<16>(reg) << " <- register " << kPinFlags;
    } catch (Exception& e) { e.Dump(); }
    os << std::dec;
  }

  void
  QuickUSBHandler::Configure() const
  {
    //QuickUsbReadDefault(fHandle, kFIFOConfig, &bitvalue);

    std::ostringstream os;
    try {
      SetWordWide(k8bits);
      //SetDataAddress(0x1ff, true, true); // increment ; enable address bus
      //SetFIFOConfig(0x82); // 0x82 = 0b0000.0000.1000.0010
      SetFIFOConfig(0xa2); // 0xa2 = 0b0000.0000.1010.0010
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
    usleep(10000); // 10 ms delay after every register writing
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
  operator<<(std::ostream& os, const QuickUSBHandler::SettingsRegister& s) {
    switch (s) {
      case QuickUSBHandler::kWordWide:       os << (unsigned short)s << ":SETTING_WORDWIDE"; break;
      case QuickUSBHandler::kDataAddress:    os << (unsigned short)s << ":SETTING_DATAADDRESS"; break;
      case QuickUSBHandler::kFIFOConfig:     os << (unsigned short)s << ":SETTING_FIFO_CONFIG"; break;
      case QuickUSBHandler::kFPGAType:       os << (unsigned short)s << ":SETTING_FPGATYPE"; break;
      case QuickUSBHandler::kCPUConfig:      os << (unsigned short)s << ":SETTING_CPUCONFIG"; break;
      case QuickUSBHandler::kSPIConfig:      os << (unsigned short)s << ":SETTING_SPICONFIG"; break;
      case QuickUSBHandler::kSlaveFIFOFlags: os << (unsigned short)s << ":SETTING_SLAVEFIFOFLAGS"; break;
      case QuickUSBHandler::kI2CTL:          os << (unsigned short)s << ":SETTING_I2CTL"; break;
      case QuickUSBHandler::kPortA:          os << (unsigned short)s << ":SETTING_PORTA"; break;
      case QuickUSBHandler::kPortB:          os << (unsigned short)s << ":SETTING_PORTB"; break;
      case QuickUSBHandler::kPortC:          os << (unsigned short)s << ":SETTING_PORTC"; break;
      case QuickUSBHandler::kPortD:          os << (unsigned short)s << ":SETTING_PORTD"; break;
      case QuickUSBHandler::kPortE:          os << (unsigned short)s << ":SETTING_PORTE"; break;
      case QuickUSBHandler::kPortAConfig:    os << (unsigned short)s << ":SETTING_PORTACCFG"; break;
      case QuickUSBHandler::kPinFlags:       os << (unsigned short)s << ":SETTING_PINFLAGS"; break;
      case QuickUSBHandler::kVersionSpeed:   os << (unsigned short)s << ":SETTING_VERSIONSPEED"; break;
      case QuickUSBHandler::kTimeoutHigh:    os << (unsigned short)s << ":SETTING_TIMEOUT_HIGH"; break;
      case QuickUSBHandler::kTimeoutLow:     os << (unsigned short)s << ":SETTING_TIMEOUT_LOW"; break;
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
