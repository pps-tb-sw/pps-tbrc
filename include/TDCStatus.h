#ifndef TDCStatus_h
#define TDCStatus_h

#include "TDCRegister.h"

/**
 * 
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 27 Apr 2015
 * \ingroup HPTDC
 */
class TDCStatus : public TDCRegister
{
  public:
    inline TDCStatus() : TDCRegister(TDC_STATUS_BITS_NUM) { SetConstantValues(); }
    inline TDCStatus(const TDCStatus& s) :
      TDCRegister(TDC_STATUS_BITS_NUM, s) { SetConstantValues(); }
    inline TDCStatus(const std::vector<uint8_t>& words) : TDCRegister(TDC_STATUS_BITS_NUM, words) {;}
    
    inline void SetConstantValues() {}

    inline uint16_t Error() const { return static_cast<bool>(GetBits(kError, 11)); } //FIXME
    inline bool HaveToken() const { return static_cast<bool>(GetBits(kHaveToken, 1)); }
    inline uint8_t FIFOOccupancy() const { return static_cast<bool>(GetBits(kReadoutFIFOOccupancy, 8)); }
    inline bool FIFOFull() const { return static_cast<bool>(GetBits(kReadoutFIFOFull, 1)); }
    inline bool FIFOEmpty() const { return static_cast<bool>(GetBits(kReadoutFIFOEmpty, 1)); }
    inline uint32_t L1Occupancy() const { return static_cast<bool>(GetBits(kL1Occupancy, 32)); }
    inline uint8_t TriggerFIFOOccupancy() const { return static_cast<bool>(GetBits(kTriggerFIFOOccupancy, 4)); }
    inline bool TriggerFIFOFull() const { return static_cast<bool>(GetBits(kTriggerFIFOFull, 1)); }
    inline bool TriggerFIFOEmpty() const { return static_cast<bool>(GetBits(kTriggerFIFOEmpty, 1)); }
    inline bool DLLLock() const { return static_cast<bool>(GetBits(kDLLLock, 1)); }

    inline void Dump(int verb=1, std::ostream& os=std::cout) const {
      os << "===================="
         << " TDC Status register dump "
         << "====================" << std::endl;
      if (verb>1) DumpRegister(os);
      os << " Error? " << Error() << std::endl
         << " Have token? " << HaveToken() << std::endl
         << " FIFO full? " << FIFOFull() << std::endl
         << " FIFO empty? " << FIFOEmpty() << std::endl
         << " L1 occupancy: " << static_cast<unsigned int>(L1Occupancy()) << std::endl
         << " TriggerFIFO occupancy: " << static_cast<unsigned int>(TriggerFIFOOccupancy()) << std::endl
         << " TriggerFIFO full? " << TriggerFIFOFull() << std::endl
         << " TriggerFIFO empty? " << TriggerFIFOEmpty() << std::endl
         << " DLL lock? " << DLLLock() << std::endl;
    }
    
  private:
    static const bit kError = 0;
    static const bit kHaveToken = 11;
    static const bit kReadoutFIFOOccupancy = 12;
    static const bit kReadoutFIFOFull = 20;
    static const bit kReadoutFIFOEmpty = 21;
    static const bit kL1Occupancy = 22;
    static const bit kTriggerFIFOOccupancy = 54;
    static const bit kTriggerFIFOFull = 58;
    static const bit kTriggerFIFOEmpty = 59;
    static const bit kDLLLock = 60;
};

#endif
