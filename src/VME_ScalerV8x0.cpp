#include "VME_ScalerV8x0.h"

namespace VME
{
  ScalerV8x0::ScalerV8x0(int32_t bhandle, uint32_t baseaddr) :
    GenericBoard<ScalerV8x0Register,cvA24_U_DATA>(bhandle, baseaddr)
  {
    std::ostringstream os;
    os << "New Scaler module added:" << "\n\t"
      //<< "  Identifier: 0x" << std::hex << GetIdentifier() << "\n\t"
       << "  Serial number: " << std::dec << GetSerialNumber() << "\n\t"
       << "  Version: " << std::dec << GetModuleVersion() << "\n\t"
       << "  Type: " << std::dec << GetModuleType() << "\n\t"
       << "  Manufacturer: 0x" << std::hex << GetManufacturerId();
    PrintInfo(os.str());
    fBuffer = (unsigned int*)malloc(32*1024*1024); // 32MB of buffer!
    if (fBuffer==NULL) {
      throw Exception(__PRETTY_FUNCTION__, "Output buffer has not been allocated!", Fatal);
    }
    gEnd = false;
  }

  ScalerV8x0::~ScalerV8x0()
  {
    free(fBuffer);
    fBuffer = NULL;
  }

  unsigned int
  ScalerV8x0::GetSerialNumber() const
  {
    uint16_t word1, word2;
    try {
      ReadRegister(kV8x0SerialMSB, &word1);
      ReadRegister(kV8x0SerialLSB, &word2);
      return static_cast<unsigned short>(((word1&0xffff)<<16)+(word2&0xffff));
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  unsigned short
  ScalerV8x0::GetModuleVersion() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0HWRevision, &word);
      return static_cast<unsigned short>(word&0xffff);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  unsigned short
  ScalerV8x0::GetModuleType() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0ModelVersion, &word);
      return static_cast<unsigned short>(word&0xffff);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }
  
  unsigned short
  ScalerV8x0::GetManufacturerId() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0OUI, &word);
      return static_cast<unsigned short>(word&0xffff);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  /*unsigned short
  ScalerV8x0::GetIdentifier() const
  {
    uint16_t word;
    try {
      ReadRegister(kIdentifier, &word);
      return word;
    } catch (Exception& e) { e.Dump(); }
    return 0;
    }*/

  unsigned short
  ScalerV8x0::GetGEO() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0GEO, &word);
      return static_cast<unsigned short>(word&0x1f);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  void
  ScalerV8x0::SetPOI(unsigned int poi) const
  {
    try {
      WriteRegister(kV8x0ChannelEnable, poi);
    } catch (Exception& e) { e.Dump(); }
  }

  unsigned int
  ScalerV8x0::GetPOI() const
  {
    uint32_t word;
    try {
      ReadRegister(kV8x0ChannelEnable, &word);
      return static_cast<unsigned int>(word);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  unsigned int
  ScalerV8x0::GetTriggerCounter() const
  {
    uint32_t word;
    try {
      ReadRegister(kV8x0TriggerCounter, &word);
      return static_cast<unsigned int>(word);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  unsigned int
  ScalerV8x0::GetChannelValue(unsigned short channel_id) const
  {
    if (channel_id<0 or channel_id>31) {
      std::ostringstream os; os << "Trying to extract the value of a channel outside allowed range: " << channel_id;
      throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
    }
    uint32_t word;
    ScalerV8x0Register reg = static_cast<ScalerV8x0Register>(kV8x0ChannelValue+(channel_id*4));
    try {
      ReadRegister(reg, &word);
      return static_cast<unsigned int>(word);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }

  ScalerEventCollection
  ScalerV8x0::FetchEvents()
  {
    if (gEnd)
      throw Exception(__PRETTY_FUNCTION__, "Abort state detected... quitting", JustWarning, TDC_ACQ_STOP);
    ScalerEventCollection ec; ec.clear();

    memset(fBuffer, 0, sizeof(unsigned int));

    int count = 0;
    const int blts = 4096; // size of the transfer in bytes
    bool finished;
    std::ostringstream o;

    // Start Readout (check if BERR is set to 0)
    CVErrorCodes ret = CAENVME_BLTReadCycle(fHandle, fBaseAddr+kV8x0OutputBuffer, (char*)fBuffer, blts, cvA32_U_BLT, cvD32, &count);
    finished = ((ret==cvSuccess)||(ret==cvBusError)||(ret==cvCommError)); //FIXME investigate...
    if (finished && gEnd) {
      throw Exception(__PRETTY_FUNCTION__, "Abort state detected... quitting", JustWarning, TDC_ACQ_STOP);
    }
    for (int i=0; i<count; i++) ec.push_back(ScalerEvent(fBuffer[i]));
    
    return ec;
  }

  ScalerV8x0Status
  ScalerV8x0::GetStatus() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0Status, &word);
      return ScalerV8x0Status(word);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }
  
  ScalerV8x0Control
  ScalerV8x0::GetControl() const
  {
    uint16_t word;
    try {
      ReadRegister(kV8x0Control, &word);
      return ScalerV8x0Control(word);
    } catch (Exception& e) { e.Dump(); }
    return 0;
  }
  
  void
  ScalerV8x0::SetControl(const ScalerV8x0Control& control) const
  {
    try {
      WriteRegister(kV8x0Control, control.GetWord());
    } catch (Exception& e) { e.Dump(); }
  }

  void
  ScalerV8x0::abort()
  {
    // Raise flag
    gEnd = true;
  }
  
}
