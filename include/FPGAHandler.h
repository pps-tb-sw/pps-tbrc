#ifndef FPGAHandler_h
#define FPGAHandler_h

#include "Client.h"
#include "QuickUSBHandler.h"
#include "TDC.h"
#include "FileConstants.h"

#include <fstream>

#define NUM_HPTDC 4 // number of HPTDC per FPGA readout

//static bool writing;
void callback(PQBULKSTREAM stream);

/**
 * \defgroup FPGA FPGA board control
 */

namespace DAQ
{
  /**
   * Main driver for a homebrew FPGA designed for the timing detectors' HPTDC
   * chip readout.
   * \brief Driver for timing detectors' FPGA readout
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date 14 Apr 2015
   * \ingroup FPGA
   */
  class FPGAHandler : public Client, private QuickUSBHandler
  {
   public:
    /// Bind to a FPGA through the USB protocol, and to the socket
    FPGAHandler(int port, const char* dev);
    ~FPGAHandler();
    void Stop() { QuickUSBHandler::fIsStopping = true; }
    
    /// Open an output file to store header/HPTDC events
    void OpenFile();
    /// Close a previously opened output file used to store header/HPTDC events
    void CloseFile();
    /// Retrieve the file name used to store data collected from the FPGA
    inline std::string GetFilename() const { return fFilename; }
    
    inline TDC* GetTDC(unsigned int i=0) {
      if (i<0 or i>=NUM_HPTDC) return 0;
      return fTDC[i];
    }
    /*inline void SetTDCSetup(const TDCSetup& s) {
      for (unsigned int i=0; i<NUM_HPTDC; i++) {
        fTDC[i]->SetSetupRegister(s);
      }
    }*/
    bool ErrorState();
    
    void StartAcquisition();
    void StopAcquisition();
    /// Socket actor type retrieval method
    inline SocketType GetType() const { return DETECTOR; }

    PPSTimingMB::TDCControl GetTDCControl() const;
    PPSTimingMB::TDCStatus GetTDCStatus() const;
    
    inline void SetTDCSetup(const PPSTimingMB::TDCSetup& s) { fSetupReg = s; SendSetupWord(); }
    inline PPSTimingMB::TDCSetup GetTDCSetup() const { return fSetupReg; }

   private:
    void RegisterTest() const;
    void SendSetupWord() const;
    void RetrieveSetupWord();

    std::string fFilename;
    std::ofstream fOutput;
    bool fIsFileOpen;
    
    TDC* fTDC[NUM_HPTDC];
    bool fIsTDCInReadout;
    PPSTimingMB::TDCSetup fSetupReg;
  };
}

#endif
