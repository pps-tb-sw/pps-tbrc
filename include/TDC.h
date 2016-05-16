#ifndef TDC_h
#define TDC_h

#include "USBHandler.h"

#include "TDCSetup.h"
#include "TDCControl.h"
#include "TDCBoundaryScan.h"
#include "TDCStatus.h"
#include "TDCEvent.h"

#include "TDCConstants.h"

/**
 * \defgroup HPTDC HPTDC chip control
 */
/**
 * \brief HPTDC object
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 27 Apr 2015 
 * \ingroup HPTDC
 */
class TDC
{
  public:
    /**
     * \brief TDC acquisition mode
     * \ingroup HPTDC
     */
    enum AcquisitionMode {
      CONT_STORAGE,
      TRIG_MATCH,
    };
    enum DetectionMode {
      PAIR      = 0x0,
      OTRAILING = 0x1,
      OLEADING  = 0x2,
      TRAILEAD  = 0x3
    };
    TDC(unsigned int id, USBHandler* h);
    inline ~TDC() {;}
    
    /// Submit the HPTDC setup word as a TDCSetup object
    inline void SetSetupRegister(const TDCSetup& c) { fSetup = c; }
    /// Retrieve the HPTDC setup word as a TDCSetup object
    inline TDCSetup GetSetupRegister() { return fSetup; }
    
    bool CheckFirmwareVersion() const;
    void SoftReset();
    TDCEventCollection FetchEvents();
    
    void ReadStatus() {
      fStatus = ReadRegister<TDCStatus>(TDC_STATUS_REGISTER);
    }
    
  private:
    /// Set the setup word to the HPTDC internal setup register
    void SendConfiguration();
    /// Read the setup word from the HPTDC internal setup register
    void ReadConfiguration();
    /// Write one register content on the HPTDC inner memory
    template<class T> void WriteRegister(unsigned int r, const T& v);
    /// Retrieve one register content from the HPTDC inner memory
    template<class T> T ReadRegister(unsigned int r);
    
    unsigned int fId;
    USBHandler* fUSB;
    
    TDCSetup fSetup;
    TDCControl fControl;
    TDCBoundaryScan fBS;
    TDCStatus fStatus;
};

#endif
