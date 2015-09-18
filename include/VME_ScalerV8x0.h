#ifndef VME_ScalerV8x0_h
#define VME_ScalerV8x0_h

#include <vector>
#include <string.h>

#include "VME_GenericBoard.h"

#define TDC_ACQ_STOP 20001

namespace VME
{
  /**
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
  enum ScalerV8x0DataFormat { DF32bit=0x0, DF26bit=0x1 };
  enum ScalerV8x0TriggerSource { TSExternalTrigger=0x0, TSTimerTrigger=0x1, TSVMETrigger=0x2 };
  
  /**
   * \brief Parser for a scaler event/header
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
  class ScalerEvent
  {
    public:
      inline ScalerEvent(unsigned int word) : fWord(word) {;}
      inline ~ScalerEvent() {;}

      inline unsigned int GetWord() const { return fWord; }

      inline unsigned short GetGEO() const { return ((fWord>>27)&0x1f); }
      inline bool IsHeader(unsigned short geo=0) const {
	bool header_bit = (fWord>>26)&0x1;
	if (geo>0) return (header_bit and GetGEO()==geo);
	return header_bit;
      }

      inline unsigned short GetTriggerNumber(unsigned short geo=0) const {
	if (!IsHeader(geo)) throw Exception(__PRETTY_FUNCTION__, "Trying to extract the trigger number from a non-header ScalerEvent word", JustWarning);
	return (fWord&0xffff);
      }
      inline ScalerV8x0TriggerSource GetTriggerSource(unsigned short geo=0) const {
	if (!IsHeader(geo)) throw Exception(__PRETTY_FUNCTION__, "Trying to extract the trigger source from a non-header ScalerEvent word", JustWarning);
	return static_cast<ScalerV8x0TriggerSource>((fWord>>16)&0x3);
      }
      inline unsigned short GetNumChannels(unsigned short geo=0) const {
	if (!IsHeader(geo)) throw Exception(__PRETTY_FUNCTION__, "Trying to extract the number of enabled channels from a non-header ScalerEvent word", JustWarning);
	return ((fWord>>18)&0x3f);
      }
      
      inline unsigned short GetChannelId() const { return ((fWord>>27)&0x1f); }
      inline unsigned int GetChannelCounter(const ScalerV8x0DataFormat& df) const {
	switch (df) {
          case DF32bit: return fWord;
          case DF26bit: return (fWord&0x3ffffff);
	}
      }
      
    private:
      unsigned int fWord;
  };

  /// A collection of ScalerEvent objects
  typedef std::vector<ScalerEvent> ScalerEventCollection;
  
  /**
   * \brief List of registers to handle a CAEN V8x0 scaler module
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
  enum ScalerV8x0Register {
    kV8x0OutputBuffer   = 0x0000,
    kV8x0ChannelValue   = 0x1000,
    kV8x0ChannelEnable  = 0x1100,
    kV8x0Control        = 0x1108,
    kV8x0Status         = 0x110e,
    kV8x0GEO            = 0x1110,
    kV8x0TriggerCounter = 0x1128,
    kV8x0FWVersion      = 0x1132,
    kV8x0OUI            = 0x402a,
    kV8x0ModelVersion   = 0x4032,
    kV8x0HWRevision     = 0x404e,
    kV8x0SerialMSB      = 0x4f02,
    kV8x0SerialLSB      = 0x4f06
  };
  
  /**
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
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

      inline void SetDataFormat(const ScalerV8x0DataFormat& fmt) { SetBit(2, fmt); }
      inline ScalerV8x0DataFormat GetDataFormat() const { return static_cast<ScalerV8x0DataFormat>(GetBit(2)); }

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
  
  /**
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
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

  /**
   * \brief Handler object for a CAEN V8x0 scaler module
   * \date 17 Sep 2015
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
  class ScalerV8x0 : public GenericBoard<ScalerV8x0Register,cvA32_U_DATA>
  {
    public:
      ScalerV8x0(int32_t bhandle, uint32_t baseaddr);
      ~ScalerV8x0();

      unsigned int GetSerialNumber() const;
      unsigned short GetModuleVersion() const;
      unsigned short GetModuleType() const;
      unsigned short GetManufacturerId() const;
      //unsigned short GetIdentifier() const;
      unsigned short GetGEO() const;
      
      void SetPOI(unsigned int poi) const;
      unsigned int GetPOI() const;

      unsigned int GetTriggerCounter() const;

      unsigned int GetChannelValue(unsigned short channel_id) const;
      ScalerEventCollection FetchEvents();

      ScalerV8x0Status GetStatus() const;
      ScalerV8x0Control GetControl() const;
      void SetControl(const ScalerV8x0Control& control) const;

      void abort();
      
    private:
      unsigned int* fBuffer;
      bool gEnd;
  };
}

#endif
