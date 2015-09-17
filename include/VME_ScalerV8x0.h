#ifndef VME_ScalerV8x0_h
#define VME_ScalerV8x0_h

#include "VME_GenericBoard.h"

namespace VME
{
  enum ScalerV8x0Register {
    kV8x0ChannelValue  = 0x1000,
    kV8x0ChannelEnable = 0x1100,
    kV8x0Control       = 0x1108,
    kV8x0FWVersion     = 0x1132,
    kV8x0OUI           = 0x402a,
    kV8x0ModelVersion  = 0x4032,
    kV8x0HWRevision    = 0x404e,
    kV8x0SerialMSB     = 0x4f02,
    kV8x0SerialLSB     = 0x4f06
  };
  class ScalerV8x0Control
  {
    public:
      inline ScalerV8x0Control(unsigned int word): fWord(word) {;}
      inline ~ScalerV8x0Control() {;}
    private:
      unsigned int fWord;
  };
  class ScalerV8x0Status
  {
    public:
      inline ScalerV8x0Status(unsigned int word): fWord(word) {;}
      inline ~ScalerV8x0Status() {;}

      inline bool DataReady() const { return fWord&0x1; }
      inline bool AlmostFull() const { return (fWord>>1)&0x1; }
      inline bool Full() const { return (fWord>>2)&0x1; }
      inline bool GlobalDataReady() const { return (fWord>>3)&0x1; }
      inline bool GlobalBusy() const { return (fWord>>4)&0x1; }
      inline bool TermOn() const { return (fWord>>5)&0x1; }
      inline bool TermOff() const { return (fWord>>6)&0x1; }
      inline bool BusError() const { return (fWord>>7)&0x1; }
    private:
      unsigned int fWord;
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

      unsigned int GetChannelValue(unsigned short channel_id) const;
      
      void SetPOI(unsigned int poi) const;
      unsigned int GetPOI() const;

      ScalerV8x0Status GetStatus() const;
      ScalerV8x0Control GetControl() const;
      void SetControl(const ScalerV8x0Control& control) const;
    private:

  };
}

#endif
