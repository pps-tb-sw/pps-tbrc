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

  unsigned int
  ScalerV8x0::GetChannelValue(unsigned short channel_id) const
  {
    uint32_t word;
    ScalerV8x0Register reg = static_cast<ScalerV8x0Register>(kV8x0ChannelValue+(channel_id*4));
    try {
      ReadRegister(reg, &word);
      return static_cast<unsigned int>(word);
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
  
}
