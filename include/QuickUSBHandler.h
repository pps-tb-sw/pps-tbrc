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
    std::string fDevice;
    QHANDLE fHandle;
    uint8_t fStreamId;
};

#endif
