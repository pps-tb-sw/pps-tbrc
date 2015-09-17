#ifndef VME_ScalerV8x0_h
#define VME_ScalerV8x0_h

#include "VME_GenericBoard.h"

namespace VME
{
  enum ScalerV8x0Register {
    kV8x0ChannelEnable = 0x1100,
    kV8x0Control       = 0x1108,
    kV8x0FWVersion     = 0x1132,
    kV8x0OUI           = 0x402a,
    kV8x0ModelVersion  = 0x4032,
    kV8x0HWRevision    = 0x404e,
    kV8x0SerialMSB     = 0x4f02,
    kV8x0SerialLSB     = 0x4f06
  };
  class ScalerV8x0 : public GenericBoard<ScalerV8x0Register,cvA24_U_DATA>
  {
    public:
      ScalerV8x0(int32_t bhandle, uint32_t baseaddr);
      inline ~ScalerV8x0() {;}

      unsigned int GetSerialNumber() const;
      unsigned short GetModuleVersion() const;
      unsigned short GetModuleType() const;
      unsigned short GetManufacturerId() const;
      //unsigned short GetIdentifier() const;

    private:

  };
}

#endif
