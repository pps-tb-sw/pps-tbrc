#ifndef QuickUSBHandler_h
#define QuickUSBHandler_h

#include <sstream>
#include <iomanip>
#include <stdint.h>
#include <string.h>
#include <vector>
#include "QuickUSB.h"

#include "Exception.h"

//#define DEBUG

namespace DAQ
{
  /**
   * \brief Generic QuickUSB communication handler
   * \date 17 May 2016
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \ingroup FPGA
   */
  class QuickUSBHandler
  {
   public:
    QuickUSBHandler();
    virtual ~QuickUSBHandler();
    
    enum HWRevision { CY7C68013AB=0x0, CY7C68013A=0x1, CY7C68013CD=0x2, CY7C68013E=0x4 };
    friend std::ostream& operator<<(std::ostream& out, const HWRevision& rev);
    enum USBSpeed { USBFullSpeed=0x0, USBHighSpeed=0x1 };
    friend std::ostream& operator<<(std::ostream& out, const USBSpeed& sp);
    struct FIFOFlags {
      bool WriteFIFOFull, WriteFIFOEmpty, RDY1, ReadFIFOFull, ReadFIFOEmpty, RDY0;
      friend std::ostream& operator<<(std::ostream& out, const FIFOFlags& ff);
    };

    void Init();
    void Reset() const;

    struct Version { QWORD MajorVersion, MinorVersion, BuildVersion; };
    Version GetFWVersion() const;
    Version GetDriverVersion() const;
    Version GetDLLVersion() const;
    
    /// Write a single word to the QuickUSB device
    inline void Write(uint16_t addr, uint8_t word) const {
      std::vector<uint8_t> w; w.push_back(word);
      try { Write(addr, w, 1); } catch (Exception& e) { throw e; }
    }
    /// Write a set of words to the QuickUSB device
    void Write(uint16_t addr, std::vector<uint8_t>& words, uint16_t size) const;
    /// Receive a set of words from the QuickUSB device
    std::vector<uint8_t> Fetch(uint16_t addr, uint16_t size) const;
    void StartBulkTransfer(QVOIDRETURN callback(PQBULKSTREAM));
    void StopBulkTransfer();

   protected:
    bool fIsStopping;
    
   private:
    enum SettingsRegister {
      kWordWide       = 0x01,
      kDataAddress    = 0x02,
      kFIFOConfig     = 0x03,
      kFPGAType       = 0x04,
      kCPUConfig      = 0x05,
      kSPIConfig      = 0x06,
      kSlaveFIFOFlags = 0x07,
      kI2CTL          = 0x08,
      kPortA          = 0x09,
      kPortB          = 0x0a,
      kPortC          = 0x0b,
      kPortD          = 0x0c,
      kPortE          = 0x0d,
      kPortAConfig    = 0x0e,
      kPinFlags       = 0x0f,
      kVersionSpeed   = 0x11,
      kTimeoutHigh    = 0x12,
      kTimeoutLow     = 0x13
    };

    void Configure() const;

    inline void SetConfigRegister(SettingsRegister reg, const uint16_t& word) const {
      int res = QuickUsbWriteSetting(fHandle, reg, word);
      if (res==0) {
        unsigned long error;
        QuickUsbGetLastError(&error);
        std::ostringstream os;
        os << "Failed to set the configuration register 0x" << std::hex << reg << " to 0x" << word << "\n\t"
           << "QuickUSB error: " << error;
        throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
      }
    }
    inline uint16_t GetConfigRegister(SettingsRegister reg) const {
      uint16_t word = 0x0;
      int res = QuickUsbReadSetting(fHandle, reg, &word);
      if (res==0) {
        unsigned long error;
        QuickUsbGetLastError(&error);
        std::ostringstream os;
        os << "Failed to set the configuration register 0x" << std::hex << reg << " to 0x" << word << "\n\t"
           << "QuickUSB error: " << error;
        throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
      }
      return word;
    }

    enum WordWide { k8bits=0, k16bits=1 };
    inline void SetWordWide(const WordWide& ww) const {
      try { SetConfigRegister(kWordWide, ww); } catch (...) { throw; }
    }

    inline void SetDataAddress(uint16_t addr, bool increment=false, bool enable_addr_bus=false) const { 
      try { SetConfigRegister(kDataAddress, (addr&0x1ff)+((!enable_addr_bus)<<14)+((!increment)<<15)); } catch (...) { throw; }
    }
    inline void SetFIFOConfig(uint16_t word) const {
      try { SetConfigRegister(kFIFOConfig, word); } catch (...) { throw; }
    }

    enum FPGAType { kAlteraPassiveSerial=0, kXilinxSlaveSerial=1 };
    inline void SetFPGAType(const FPGAType ft) const {
      try { SetConfigRegister(kFPGAType, static_cast<uint16_t>(ft)&0x1); } catch (...) { throw; }
    }

    enum CPUConfig {
      kCLKOUTdisable=0x0, kCLKOUTenable=0x1,
      kCLKINVdisable=0x0, kCLKINVenable=0x2,
      kCLKSPD12MHz=0x0, kCLKSPD24MHz=0x4, kCLKSPD48MHz=0x8, kCLKSPDreserved=0xc,
      kUSBFullSpeedForce=0x0, kUSBFullSpeedAllow=0x8000
    };
    inline void SetCPUConfig(uint16_t c) const { try { SetConfigRegister(kCPUConfig, c); } catch (...) { throw; } }

    enum SPIConfig {
      kSPIENDIANlsb=0x0, kSPIENDIANmsb=0x1,
      kSPICPOLnormal=0x0, kSPICPOLinverted=0x2,
      kSPICPHAsampleclock=0x0, kSPICPHAclocksample=0x4,
      kSPIPORTE=0x0, kSPIPORTA=0x8,
      kNCEPIN2=0x0, kNCEPIN7=0x10,
      kMISOPIN5=0x0, kMISOPIN2=0x20,
      kGPIFA8gpio=0x0, kGPIFA8gfiadr8=0x8000
    };
    inline void SetSPIConfig(uint16_t c) const { try { SetConfigRegister(kSPIConfig, c); } catch (...) { throw; } }

    inline FIFOFlags GetSlaveFIFOFlags() const {
      FIFOFlags ff;
      try {
        uint16_t flags = GetConfigRegister(kSlaveFIFOFlags);
        ff.WriteFIFOFull = flags&0x1;
        ff.WriteFIFOEmpty = (flags>>1)&0x1;
        ff.RDY1 = (flags>>3)&0x1;
        ff.ReadFIFOFull = (flags>>8)&0x1;
        ff.ReadFIFOEmpty = (flags>>9)&0x1;
        ff.RDY0 = (flags>>11)&0x1;
      } catch (...) { throw; }
      return ff;
    }
    
    enum I2CTL {
      kI2CBusClkSpeed100kHz=0x0, kI2CBusClkSpeed400kHZ=0x1,
      kHandleACK=0x0, kIgnoreACK=0x80,
      kBusError=0x600, kNoACK=0x700, kNormalCompletion=0x800, kSlaveWait=0xa00, kTimeout=0xb00
    };
    inline void SetI2CTL(uint16_t c) const { try { SetConfigRegister(kI2CTL, c); } catch (...) { throw; } }

    enum LogicLevel { kLowLogic=0x0, kHighLogic=0x1 };
    inline void SetPort(const char port, const LogicLevel& lev, bool output_buf) const {
      SettingsRegister reg;
      switch (port) {
        case 'A': case 'a': reg = kPortA; break;
        case 'B': case 'b': reg = kPortB; break;
        case 'C': case 'c': reg = kPortC; break;
        case 'D': case 'd': reg = kPortD; break;
        case 'E': case 'e': reg = kPortE; break;
        default: { std::ostringstream os; os << "Unrecognized port: " << port; throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning); }
      }
      try { SetConfigRegister(reg, (static_cast<uint16_t>(lev)&0x1)+(output_buf<<15)); } catch (...) { throw; }
    }

    inline HWRevision GetHWRevision() const { try { return static_cast<HWRevision>((GetConfigRegister(kVersionSpeed)>>8)&0xff); } catch (...) { throw; } }
    inline USBSpeed GetUSBSpeed() const { try { return static_cast<USBSpeed>(GetConfigRegister(kVersionSpeed)&0x1); } catch (...) { throw; } }

    /// Return the FW timeout high (in ms)
    inline uint16_t GetTimeoutHigh() const { try { return GetConfigRegister(kTimeoutHigh); } catch (...) { throw; } }
    /// Return the FW timeout low (in ms)
    inline uint16_t GetTimeoutLow() const { try { return GetConfigRegister(kTimeoutLow); } catch (...) { throw; } }

    std::string fDevice;
    QHANDLE fHandle;
    uint8_t fStreamId;
  };
}

#endif
