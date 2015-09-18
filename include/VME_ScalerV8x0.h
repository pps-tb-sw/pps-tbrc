#ifndef VME_ScalerV8x0_h
#define VME_ScalerV8x0_h

#include "VME_GenericBoard.h"

namespace VME
{
  enum ScalerV8x0Register {
    kV8x0ChannelValue  = 0x1000,
    kV8x0ChannelEnable = 0x1100,
    kV8x0Control       = 0x1108,
    kV8x0Status        = 0x110e,
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
      inline ScalerV8x0Control(unsigned short word): fWord(word) {;}
      inline ~ScalerV8x0Control() {;}

      inline unsigned short GetWord() const { return fWord; }
      
      enum AcquisitionMode { TriggerDisabled=0x0, TriggerRandom=0x1, PeriodicalTrigger=0x2 };
      inline void SetAcquisitionMode(const AcquisitionMode& mode) { SetBit(0, mode&0x1); SetBit(1, (mode>>1)&0x1); }
      inline AcquisitionMode GetAcquisitionMode() const {
	unsigned short word1 = GetBit(0), word2 = GetBit(1);
	return static_cast<AcquisitionMode>(word1+(word2<<1));
      }

      enum DataFormat { DF32bit=0x0, DF26bit=0x1 };
      inline void SetDataFormat(const DataFormat& fmt) { SetBit(2, fmt); }
      inline DataFormat GetDataFormat() const { return static_cast<DataFormat>(GetBit(2)); }

      inline void SetBusError(bool enable) { SetBit(4, enable); }
      inline bool GetBusError() const { return GetBit(4); }
      
      inline void SetHeader(bool enable) { SetBit(5, enable); }
      inline bool GetHeader() const { return GetBit(5); }
      
      inline void SetClearMEB(bool enable) { SetBit(6, enable); }
      inline bool GetClearMEB() const { return GetBit(6); }
      
      inline void SetAutoReset(bool enable) { SetBit(7, enable); }
      inline bool GetAutoReset() const { return GetBit(7); }
      
    private:
      inline bool GetBit(unsigned short id) const { return static_cast<bool>((fWord>>id)&0x1); }
      inline void SetBit(unsigned short id, unsigned short value=0x1) {
	if (value==GetBit(id)) return;
	unsigned short sign = (value==0x0) ? -1 : 1; fWord += sign*(0x1<<id);
      }
      unsigned short fWord;
  };
  class ScalerV8x0Status
  {
    public:
      inline ScalerV8x0Status(unsigned short word): fWord(word) {;}
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
      unsigned short fWord;
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
