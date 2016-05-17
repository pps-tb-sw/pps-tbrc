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
    
    /// Write a set of words to the QuickUSB device
    void Write(uint16_t addr, const std::vector<uint16_t>& words, uint8_t size) const;
    /// Receive a set of words from the QuickUSB device
    std::vector<uint16_t> Fetch(uint16_t addr, uint8_t size) const;

  protected:
    bool fIsStopping;
    
  private:
    std::string fDevice;
    QHANDLE fHandle;
};

#endif
