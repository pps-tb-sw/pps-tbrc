#ifndef TDCSetup_h
#define TDCSetup_h

#include <iostream>
#include <iomanip>

#include "TDCRegister.h"
#include "TDCConstants.h"

/**
 * Object handling the setup word provided by/to the HPTDC chip
 * \brief Setup word to be sent to the HPTDC chip
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 16 Apr 2015
 * \ingroup HPTDC
 */
class TDCSetup : public TDCRegister
{
  public:
    typedef enum {
      E_100ps=0, E_200ps, E_400ps, E_800ps, E_1p6ns, E_3p12ns, E_6p25ns, E_12p5ns
    } EdgeResolution;
    typedef enum { DT_5ns=0, DT_10ns, DT_30ns, DT_100ns } DeadTime;
    typedef enum {
      W_100ps=0, W_200ps, W_400ps, W_800ps, W_1p6ns, W_3p2ns, W_6p25ns,
      W_12p5ns, W_25ns, W_50ns, W_100ns, W_200ns, W_400ns, W_800ns
    } WidthResolution;
    typedef enum {
      VernierError=0x1, CoarseError=0x2, ChannelSelectError=0x4,
      L1BufferParityError=0x8, TriggerFIFOParityError=0x10,
      TriggerMatchingError=0x20, ReadoutFIFOParityError=0x40,
      ReadoutStateError=0x80, SetupParityError=0x100,
      ControlParityError=0x200, JTAGInstructionParityError=0x400
    } EnabledError;
    typedef enum {
      DLL_40MHz=0x0, DLL_160MHz=0x1, DLL_320MHz=0x2, DLL_Illegal=0x3
    } DLLSpeedMode;
    typedef enum {
      Serial_pll_clock_80=0x0, Serial_pll_clock_160=0x1, Serial_pll_clock_40=0x2, Serial_aux_clock=0x3
    } SerialClockSource;
    typedef enum {
      IO_clock_40=0x0, IO_pll_clock_80=0x1, IO_pll_clock_160=0x2, IO_aux_clock=0x3
    } IOClockSource;
    typedef enum {
      Core_clock_40=0x0, Core_pll_clock_80=0x1, Core_pll_clock_160=0x2, Core_aux_clock=0x3
    } CoreClockSource;
    typedef enum {
      DLL_clock_40=0x0, DLL_pll_clock_40=0x1, DLL_pll_clock_160=0x2, DLL_pll_clock_320=0x3, DLL_aux_clock=0x4
    } DLLClockSource;
    typedef enum {
      RO_Fixed=0x0, RO_pll_80Mbits_s=0x1
    } ReadoutSpeed;
    typedef enum {
      SS_NoStrobe=0x0, SS_DSStrobe=0x1, SS_LeadingTrailingStrobe=0x2, SS_LeadingEdge=0x3
    } SerialStrobeType;
    typedef enum {
      RSC_40Mbits_s=0x0, RSC_20Mbits_s=0x1, RSC_10Mbits_s=0x2, RSC_5Mbits_s=0x3,
      RSC_2p5Mbits_s=0x4, RSC_1p25Mbits_s=0x5, RSC_625kbits_s=0x6,
      RSC_312p5kbits_s=0x7
    } ReadoutSingleCycleSpeed;
  
  public:
    TDCSetup() : TDCRegister(TDC_SETUP_BITS_NUM) { SetConstantValues(); }
    TDCSetup(const TDCSetup& c) : TDCRegister(TDC_SETUP_BITS_NUM, c) { 
      SetConstantValues();
    }
    
    //////////////////////// Public set'ers and get'ers ////////////////////////
    
    /// Mark events with error if global error signal is set.
    inline void SetEnableErrorMark(const bool em) {
      SetBits(kEnableErrorMark, em, 1); }
    inline bool GetEnableErrorMark() const {
      return static_cast<bool>(GetBits(kEnableErrorMark, 1));
    }
    /// Bypass TDC chip if global error signal is set.
    inline void SetEnableErrorBypass(const bool eb) {
      SetBits(kEnableErrorBypass, eb, 1);
    }
    inline bool GetEnableErrorBypass() const {
      return static_cast<bool>(GetBits(kEnableErrorBypass, 1));
    }
    /// Enable internal error types for generation of global error signals.
    inline void SetEnableError(const uint16_t& err) {
      SetBits(kEnableError, err, 11);
    }
    inline uint16_t GetEnableError() const {
      return static_cast<uint16_t>(GetBits(kEnableError, 11));
    }
    /// Enable of serial read-out (otherwise parallel read-out)
    inline void SetEnableSerial(const bool es) {
      SetBits(kEnableSerial, es, 1);
    }
    inline bool GetEnableSerial() const {
      return static_cast<bool>(GetBits(kEnableSerial, 1));
    }
    /// Enable of read-out via JTAG
    inline void SetEnableJTAGReadout(const bool jr) {
      SetBits(kEnableJTAGReadout, jr, 1);
    }
    inline bool GetEnableJTAGReadout() const {
      return static_cast<bool>(GetBits(kEnableJTAGReadout, 1));
    }
    /// Effective size of readout FIFO
    inline void SetReadoutFIFOSize(int rfs) {
      uint8_t size;
      if (rfs<2) return; // FIXME error!
      else if (rfs==2) size = 0x0; // no hit
      else if (rfs<=4) size = 0x1; // 1 hit
      else if (rfs<=8) size = 0x2; // 2 hits
      else if (rfs<=16) size = 0x3; // 4 hits
      else if (rfs<=32) size = 0x4; // 32 hits
      else if (rfs<=64) size = 0x5; // 64 hits
      else if (rfs<=128) size = 0x6; // 128 hits
      else if (rfs<=256) size = 0x7; // 128 hits
      else return; // FIXME error!
      SetBits(kReadoutFIFOSize, size, 3);
    }
    inline int GetReadoutFIFOSize() const {
      uint8_t sz = static_cast<uint8_t>(GetBits(kReadoutFIFOSize, 3));
      return (0x1<<(sz+1));
    }
    /// Set the offset in reject counter (defines reject latency together with coarse count offset)
    inline void SetRejectCountOffset(uint16_t rco) {
      SetBits(kRejectCountOffset, rco, 12);
    }
    /// Extract the offset in reject counter
    inline uint16_t GetRejectCountOffset() const {
      return static_cast<uint16_t>(GetBits(kRejectCountOffset, 12));
    }
    /// Set the search window (in multiples of clock cycles: 0=25 ns, 1=50 ns, ...)
    inline void SetSearchWindow(uint16_t sw) {
      SetBits(kSearchWindow, sw, 12);
    }
    /// Extract the search window (in multiples of clock cycles: 0=25 ns, 1=50 ns, ...)
    inline uint16_t GetSearchWindow() const {
      return static_cast<uint16_t>(GetBits(kSearchWindow, 12));
    }
    /// Set the matching window (in multiples of clock cycles: 0=25 ns, 1=50 ns, ...)
    inline void SetMatchWindow(uint16_t mw) {
      SetBits(kMatchWindow, mw, 12);
    }
    /// Extract the matching window (in multiples of clock cycles: 0=25 ns, 1=50 ns, ...)
    inline uint16_t GetMatchWindow() const {
      return static_cast<uint16_t>(GetBits(kMatchWindow, 12));
    }
    inline void SetEdgeResolution(const EdgeResolution r) {
      SetBits(kLeadingResolution, r, 3);
    }
    inline EdgeResolution GetEdgeResolution() const {
      return static_cast<EdgeResolution>(GetBits(kLeadingResolution, 3));
    }
    /**
     * Set the maximum number of hits that can be recorded for each event.
     * It is always rounded to the next power of 2 (in the range 0-128), and
     * if lower than 0 or bigger than 128 then set to unimited.
     * \brief Set the maximum number of hits per event
     */
    inline void SetMaxEventSize(int sz=-1) {
      uint8_t size;
      if (sz<0) size = 0x9; // no limit
      else if (sz==0) size = 0x0; // no hit
      else if (sz<=1) size = 0x1; // 1 hit
      else if (sz<=2) size = 0x2; // 2 hits
      else if (sz<=4) size = 0x3; // 4 hits
      else if (sz<=8) size = 0x4; // 8 hits
      else if (sz<=16) size = 0x5; // 16 hits
      else if (sz<=32) size = 0x6; // 32 hits
      else if (sz<=64) size = 0x7; // 64 hits
      else if (sz<=128) size = 0x8; // 128 hits
      else size = 0x9; // no limit
      SetBits(kMaxEventSize, size, 4);
    }
    /// Extract the maximum number of hits per event
    inline uint8_t GetMaxEventSize() const { 
      uint8_t max = static_cast<uint8_t>(GetBits(kMaxEventSize, 4));
      if (max==0) return 0;
      else if (max<0xA) return (1<<(max-1));
      else return -1;
    }
    /**
     * Set whether or not hits are rejected once FIFO is full.
     * \brief Reject hits when readout FIFO full.
     */
    inline void SetRejectFIFOFull(const bool rej=true) {
      SetBits(kRejectFIFOFull, rej, 1);
    }
    /**
     * Extract whether or not hits are rejected once FIFO is full.
     * \brief Are hits rejected when readout FIFO is full?
     */
    inline bool GetRejectFIFOFull() const {
      return static_cast<bool>(GetBits(kRejectFIFOFull, 1));
    }
    /// Enable the readout of buffer occupancies for each event (for debugging purposes)
    inline void SetEnableReadoutOccupancy(const bool ro=true) {
      SetBits(kEnableReadoutOccupancy, ro, 1);
    }
    inline bool GetEnableReadoutOccupancy() const {
      return static_cast<bool>(GetBits(kEnableReadoutOccupancy, 1));
    }
    /// Enable the readout of separators for each event (for debugging purposes, valid if readout of occupancies is enabled)
    inline void SetEnableReadoutSeparator(const bool ro=true) {
      if (!GetEnableReadoutOccupancy()) {
        std::cerr << "Warning: Trying to enable the separator readout "
                  << "while the occupancy readout is disabled!" << std::endl
                  << "Enabling this occupancy readout automatically..."
                  << std::endl;
        SetEnableReadoutOccupancy(true);
      }
      SetBits(kEnableReadoutSeparator, ro, 1);
    }
    inline bool GetEnableReadoutSeparator() const {
      return static_cast<bool>(GetBits(kEnableReadoutSeparator, 1));
    }
    /// Set offset for the event counter
    inline void SetEventCountOffset(uint16_t eco) {
      SetBits(kEventCountOffset, eco, 12);
    }
    /// Set offset for the trigger time tag counter to set effective trigger latency
    inline void SetTriggerCountOffset(uint16_t tco) {
      SetBits(kTriggerCountOffset, tco, 12);
    }
    /// Extract trigger time tag count offset
    inline uint16_t GetTriggerCountOffset() const {
      return static_cast<uint16_t>(GetBits(kTriggerCountOffset, 12));
    }
    /// Set the time offset for one single channel
    inline void SetChannelOffset(int channel, uint16_t offset) {
      if (channel>=TDC_NUM_CHANNELS or channel<0) return;
      SetBits(kOffset0-9*channel, offset, 9);
    }
    /// Return the offset for one single channel
    inline uint16_t GetChannelOffset(int channel) const {
      if (channel>=TDC_NUM_CHANNELS or channel<0) return -1;
      return static_cast<uint16_t>(GetBits(kOffset0-9*channel, 9));
    }
    /// Set the time offset for all channels
    inline void SetAllChannelsOffset(uint16_t offset) {
      for (int i=0; i<TDC_NUM_CHANNELS; i++) {
        SetChannelOffset(i, offset);
      }
    }
    /// Set offset for the coarse time counter
    inline void SetCoarseCountOffset(uint16_t cco) {
      SetBits(kCoarseCountOffset, cco, 12);
    }
    /// Extract offset for the coarse time counter
    inline uint16_t GetCoarseCountOffset() const {
      return static_cast<uint16_t>(GetBits(kCoarseCountOffset, 12));
    }
    /// Set the DLL taps adjustments with a resolution of ~10 ps
    inline void SetDLLAdjustment(int tap, uint8_t adj) {
      if (tap>=TDC_NUM_CHANNELS or tap<0) return;
      SetBits(kDLLTapAdjust0+3*tap, adj, 3);
    }
    /// Set the adjustment of DLL taps
    inline uint8_t GetDLLAdjustment(int tap) const {
      if (tap>=TDC_NUM_CHANNELS or tap<0) return -1;
      return static_cast<uint8_t>(GetBits(kDLLTapAdjust0+3*tap, 3));
    }
    /// Extract the adjustment of DLL taps
    inline void SetAllTapsDLLAdjustment(uint8_t adj) {
      for (int i=0; i<TDC_NUM_CHANNELS; i++) {
        SetDLLAdjustment(i, adj);
      }
    }
    /// Set the adjustment of the RC delay line
    inline void SetRCAdjustment(int tap, uint8_t adj) {
      if (tap>3 or tap<0) return;
      SetBits(kRCAdjust0+3*tap, adj, 3);
    }
    /// Extract the adjustment of the RC delay line
    inline uint8_t GetRCAdjustment(int tap) {
      if (tap>3 or tap<0) return -1;
      return static_cast<uint8_t>(GetBits(kRCAdjust0+3*tap, 3));
    }
    /// Set the pulse width resolution when paired measurements are performed
    inline void SetWidthResolution(const WidthResolution r) {
      SetBits(kWidthSelect, r, 4);
    }
    /// Extract the pulse width resolution when paired measurements are performed
    inline WidthResolution GetWidthResolution() const {
      return static_cast<WidthResolution>(GetBits(kWidthSelect, 4));
    }
    /// Set the offset in vernier decoding
    inline void SetVernierOffset(const uint8_t vo) {
      SetBits(kVernierOffset, vo, 5);
    }
    /// Extract the offset in vernier decoding
    inline uint8_t GetVernierOffset() const {
      return static_cast<uint8_t>(GetBits(kVernierOffset, 5));
    }
    /// Channel dead time between hits
    inline void SetDeadTime(const DeadTime dt) {
      SetBits(kDeadTime, dt, 2);
    }
    inline DeadTime GetDeadTime() const {
      return static_cast<DeadTime>(GetBits(kDeadTime, 2));
    }
    /// Automatic inversion of test pattern. Only used during production testing.
    inline void SetTestInvert(const bool ti=true) {
      SetBits(kTestInvert, ti, 1);
    }
    inline bool GetTestInvert() const {
      return static_cast<bool>(GetBits(kTestInvert, 1));
    }
    /// Test mode where hit data are taken from coretest. Only used during production testing.
    inline void SetTestMode(const bool tm=true) {
      SetBits(kTestMode, tm, 1);
    }
    inline bool GetTestMode() const {
      return static_cast<bool>(GetBits(kTestMode, 1));
    }
    /// Enable/disable the detection of trailing edges
    inline void SetTrailingMode(const bool trail=true) {
      SetBits(kTrailing, trail, 1);
    }
    /// Extract the status for the detection of trailing edges
    inline bool GetTrailingMode() const {
      return static_cast<bool>(GetBits(kTrailing, 1));
    }
    /// Enable the detection of leading edges
    inline void SetLeadingMode(const bool lead=true) {
      SetBits(kLeading, lead, 1);
    }
    /// Extract the status for the detection of leading edges
    inline bool GetLeadingMode() const {
      return static_cast<bool>(GetBits(kLeading, 1));
    }
    /// Set the enable status of trigger matching mode
    inline void SetTriggerMatchingMode(const bool trig=true) {
      SetBits(kEnableMatching, trig, 1);
    }
    /// Extract the enable status of trigger matching mode
    inline bool GetTriggerMatchingMode() const {
      return static_cast<bool>(GetBits(kEnableMatching, 1));
    }
    /// Enable the pairing of leading and trailing edges (overrides individual enable of leading/trailing edges)
    inline void SetEdgesPairing(const bool pair=true) {
      SetBits(kEnablePair, pair, 1);
    }
    inline bool GetEdgesPairing() const {
      return static_cast<bool>(GetBits(kEnablePair, 1));
    }
    /// Set the parity of setup data (should be an even parity)
    inline void SetSetupParity(const bool sp=true) {
      SetBits(kSetupParity, sp, 1);
    }
    /// Extract the parity of setup data (should be an even parity)
    inline bool GetSetupParity() const {
      return static_cast<bool>(GetBits(kSetupParity, 1));
    }
    
    /// Ensure that the critical constant values are properly set in the setup word
    void SetConstantValues();
    
    /// Effective trigger latency in number of clock cycles (when no counter roll-over is used)
    inline uint16_t GetTriggerLatency() const {
      return ((GetCoarseCountOffset()-GetTriggerCountOffset())%(0x1<<12));
    }
    inline void SetTDCId(const uint8_t id=0x0) {
      SetBits(kTDCId, id, 4);
    }
    inline uint16_t GetTDCId() const {
      return static_cast<uint16_t>(GetBits(kTDCId, 4));
    }
    
    void Dump(int verb=1, std::ostream& os=std::cout) const;
    
  private:    
    //////////////////////// Private set'ers and get'ers ////////////////////////
    
    /// Serial transmission speed in single cycle mode
    inline void SetReadoutSingleCycleSpeed(const ReadoutSingleCycleSpeed rscs=RSC_40Mbits_s) {
      SetBits(kReadoutSingleCycleSpeed, static_cast<int>(rscs), 3);
    }
    /// Programmable delay of serial input, in time unit ~ 1 ns
    inline void SetSerialDelay(const uint8_t sd=0x0) {
      SetBits(kSerialDelay, sd, 4);
    }
    inline void SetStrobeSelect(const SerialStrobeType ss=SS_NoStrobe) {
      SetBits(kStrobeSelect, static_cast<int>(ss), 2);
    }
    /**
     * \brief Selection of serial read-out speed
     * \param[in] rss
     * * 0: Selection of serial read-out speed (as defined by setup[19:17],
     * \a SetReadoutSingleCycleSpeed)
     * * 1: 80 Mbits/s (PLL lock required)
     */
    inline void SetReadoutSpeedSelect(const ReadoutSpeed rss=RO_Fixed) {
      SetBits(kReadoutSpeedSelect, static_cast<int>(rss), 1);
    }
    /// Programmable delay of token input, in time unit ~ 1 ns
    inline void SetTokenDelay(const uint8_t td=0x0) {
      SetBits(kTokenDelay, td, 4);
    }
    /// Enable of local trailers in read-out
    inline void SetEnableLocalTrailer(const bool elt=true) {
      SetBits(kEnableLocalTrailer, elt, 1);
    }
    /// Enable of local headers in read-out
    inline void SetEnableLocalHeader(const bool elh=true) {
      SetBits(kEnableLocalHeader, elh, 1);
    }
    /// Enable of global trailers in read-out (only valid for master TDC)
    inline void SetEnableGlobalTrailer(const bool egt=true) {
      SetBits(kEnableGlobalTrailer, egt, 1);
    }
    /// Enable of global headers in read-out (only valid for master TDC)
    inline void SetEnableGlobalHeader(const bool egh=true) {
      SetBits(kEnableGlobalHeader, egh, 1);
    }
    /// Keep token until end of event or no more data,
    /// otherwise pass token after each word read.
    /// Must be enabled when using trigger matching.
    inline void SetKeepToken(const bool kt=true) {
      SetBits(kKeepToken, kt, 1);
    }
    inline void SetMaster(const bool m=true) {
      SetBits(kMaster, m, 1);
    }
    inline void SetEnableBytewise(const bool seb=true) {
      SetBits(kEnableBytewise, seb, 1);
    }
    /// Select serial in and token in from bypass inputs
    inline void SetBypassInputs(const bool sbi=true) {
      SetBits(kSelectBypassInputs, sbi, 1);
    }
    /// Enable overflow detection of L1 buffers (should always be enabled!)
    inline void SetEnableOverflowDetect(const bool eod=true) {
      SetBits(kEnableOverflowDetect, eod, 1);
    }
    /// Enable read-out of relative time to trigger time tag. Only valid when
    /// using trigger matching mode.
    inline void SetEnableRelative(const bool er=true) {
      SetBits(kEnableRelative, er, 1);
    }
    /// Enable of automatic rejection (should always be enabled if trigger matching mode!)
    inline void SetEnableAutomaticReject(const bool ear=true) {
      SetBits(kEnableAutomaticReject, ear, 1);
    }
    /// Enable all counters to be set on bunch count reset
    inline void SetEnableSetCountersOnBunchReset(const bool escobr=true) {
      SetBits(kEnableSetCountersOnBunchReset, escobr, 1);
    }
    /// Enable master reset code on encoded_control
    inline void SetEnableMasterResetCode(const bool emrc=true) {
      SetBits(kEnableMasterResetCode, emrc, 1);
    }
    /// Enable master reset of whole TDC on event reset
    inline void SetEnableMasterResetOnEventReset(const bool emroer=true) {
      SetBits(kEnableMasterResetOnEventReset, emroer, 1);
    }
    /// Enable reset channel buffers when separator
    inline void SetEnableResetChannelBufferWhenSeparator(const bool ercbws=true) {
      SetBits(kEnableResetChannelBufferWhenSeparator, ercbws, 1);
    }
    /// Enable generation of separator on event reset
    inline void SetEnableSeparatorOnEventReset(const bool esoer=true) {
      SetBits(kEnableSeparatorOnEventReset, esoer, 1);
    }
    /// Enable generation of separator on bunch reset
    inline void SetEnableSeparatorOnBunchReset(const bool esobr=true) {
      SetBits(kEnableSeparatorOnBunchReset, esobr, 1);
    }
    /// Enable of direct event reset input pin (1), otherwise taken from encoded control
    inline void SetEnableDirectEventReset(const bool eder=true) {
      SetBits(kEnableDirectEventReset, eder, 1);
    }
    /// Enable of direct bunch reset input pin (1), otherwise taken from encoded control
    inline void SetEnableDirectBunchReset(const bool edbr=true) {
      SetBits(kEnableDirectBunchReset, edbr, 1);
    }
    /// Enable of direct trigger input pin
    inline void SetEnableDirectTrigger(const bool edt=true) {
      SetBits(kEnableDirectTrigger, edt, 1);
    }
    /// Low power mode of channel buffers
    inline void SetLowPowerMode(const bool lpm=true) {
      SetBits(kLowPowerMode, lpm, 1);
    }
    /// Control of DLL (DLL charge pump levels)
    inline void SetDLLControl(const uint8_t dc) {
      SetBits(kDLLControl, dc&0x15, 4);
    }
    /// Perform RC interpolation on-chip (only valid in very high resolution mode)
    inline void SetModeRCCompression(const bool mrc=true) {
      SetBits(kModeRCCompression, mrc, 1);
    }
    /// Enable of RR delay lines mode (in very high resolution mode) ; only for channels 0-4-8-12-16-20-24-28 active
    inline void SetModeRC(const bool mr=true) {
      SetBits(kModeRC, mr, 1);
    }
    /// Selection of DLL speed mode
    inline void SetDLLMode(const DLLSpeedMode dsm) {
      if (dsm==DLL_Illegal) {
        std::cerr << "Warning: Using an illegal DLL mode: 0x"
                  << std::hex << dsm << std::dec << std::endl;
      }
      SetBits(kDLLMode, dsm, 2);
    }
    /// Control of PLL
    inline void SetPLLControl(const uint8_t charge_pump_current=0x4,
                              const bool power_down_mode=false,
                              const bool enable_test_outputs=false,
                              const bool invert_connection_to_status=false) {
      uint8_t word = (charge_pump_current&0x1F)|((power_down_mode&0x1)<<5);
      word |= ((enable_test_outputs&0x1)<<6);
      word |= ((invert_connection_to_status&0x1)<<7);
      SetBits(kPLLControl, word, 8);
    }
    /**
     * \brief Delay of internal serial clock
     * \param[in] delay_clock Use of direct clock (0) or delayed clock (1)
     * \param[in] delay Delay in steps of (typically) 0.13 ns
     */
    inline void SetSerialClockDelay(const bool delay_clock, const uint8_t delay) {
      uint8_t word = ((delay&0x7)|((delay_clock&0x1)<<3));
      SetBits(kSerialClockDelay, word, 4);
    }
    /**
     * \brief Delay of internal I/O clock
     * \param[in] delay_clock Use of direct clock (0) or delayed clock (1)
     * \param[in] delay Delay in steps of (typically) 0.13 ns
     */
    inline void SetIOClockDelay(const bool delay_clock, const uint8_t delay) {
      uint8_t word = ((delay&0x7)|((delay_clock&0x1)<<3));
      SetBits(kIOClockDelay, word, 4);
    }
    /**
     * \brief Delay of internal core clock
     * \param[in] delay_clock Use of direct clock (0) or delayed clock (1)
     * \param[in] delay Delay in steps of (typically) 0.13 ns
     */
    inline void SetCoreClockDelay(const bool delay_clock, const uint8_t delay) {
      uint8_t word = ((delay&0x7)|((delay_clock&0x1)<<3));
      SetBits(kCoreClockDelay, word, 4);
    }
    /**
     * \brief Delay of internal DLL clock
     * \param[in] delay_clock Use of direct clock (0) or delayed clock (1)
     * \param[in] delay Delay in steps of (typically) 0.13 ns
     */
    inline void SetDLLClockDelay(const bool delay_clock, const uint8_t delay) {
      uint8_t word = ((delay&0x7)|((delay_clock&0x1)<<3));
      SetBits(kDLLClockDelay, word, 4);
    }
    /// Selection of source for serial clock
    inline void SetSerialClockSource(const SerialClockSource scs) {
      if (scs==Serial_pll_clock_160 or scs==Serial_pll_clock_40) {
        std::cerr << "Warning: Using an invalid Serial clock source: 0x"
                  << std::hex << scs << std::dec << std::endl;
      }
      SetBits(kSerialClockSource, scs, 2);
    }
    /// Selection of clock source for I/O signals
    inline void SetIOClockSource(const IOClockSource ics) {
      if (ics==IO_pll_clock_80 or ics==IO_pll_clock_160) {
        std::cerr << "Warning: Using an invalid IO clock source: 0x"
                  << std::hex << ics << std::dec << std::endl;
      }
      SetBits(kIOClockSource, ics, 2);
    }
    /// Selection of clock source for internal logic
    inline void SetCoreClockSource(const CoreClockSource ccs) {
      SetBits(kCoreClockSource, ccs, 2);
    }
    /// Selection of clock source for DLL
    inline void SetDLLClockSource(const DLLClockSource dcs) {
      SetBits(kDLLClockSource, dcs, 3);
    }
    /// Counter roll over value, defining maximal count value from where counters will be reset to 0
    inline void SetRollOver(const uint16_t ro=0xFFF) {
      SetBits(kRollOver, ro, 12);
    }
    /** Enable LV TTL input on:
     * * serial_in,
     * * serial_bypass_in,
     * * token_in,
     * * token_bypass_in,
     * otherwise uses LVDS input levels.
     * Disable LVDS drivers on:
     * * serial_out,
     * * strobe_out,
     * * token_out.
     * \brief Enable LV TTL inputs on serial registers, and disable their drivers
     */
    inline void SetEnableTTLSerial(const bool ts=true) {
      SetBits(kEnableTTLSerial, ts, 1);
    }
    /** Enable LV TTL input on:
     * * trigger,
     * * bunch_reset,
     * * event_reset,
     * * encoded_control,
     * otherwise uses LVDS input levels.
     * \brief Enable LV TTL inputs on control registers
     */
    inline void SetEnableTTLControl(const bool tc=true) {
      SetBits(kEnableTTLControl, tc, 1);
    }
    /// Enable LV TTL input on reset, otherwise uses LVDS input levels
    inline void SetEnableTTLReset(const bool tr=true) {
      SetBits(kEnableTTLReset, tr, 1);
    }
    /// Enable LV TTL inputs on: clk, aux_clock, otherwise uses LVDS input levels
    inline void SetEnableTTLClock(const bool tc=true) {
      SetBits(kEnableTTLClock, tc, 1);
    }
    /// Enable LV TTL input on hit[31:0], otherwise uses LVDS input levels
    inline void SetEnableTTLHit(const bool th=true) {
      SetBits(kEnableTTLHit, th, 1);
    }
    inline void SetTest(const bool test=true) {
      SetBits(kTestSelect, test, 1);
      SetBits(kTestSelect+1, 0x7, 3);
    }
    
    // List of LSBs for all sub-words in the full ~700-bits setup word
    static const bit kTestSelect = 0;
    static const bit kEnableErrorMark = 4;
    static const bit kEnableErrorBypass = 5;
    static const bit kEnableError = 6;
    static const bit kReadoutSingleCycleSpeed = 17;
    static const bit kSerialDelay = 20;
    static const bit kStrobeSelect = 24;
    static const bit kReadoutSpeedSelect = 26;
    static const bit kTokenDelay = 27;
    static const bit kEnableLocalTrailer = 31;
    static const bit kEnableLocalHeader = 32;
    static const bit kEnableGlobalTrailer = 33;
    static const bit kEnableGlobalHeader = 34;
    static const bit kKeepToken = 35;
    static const bit kMaster = 36;
    static const bit kEnableBytewise = 37;
    static const bit kEnableSerial = 38;
    static const bit kEnableJTAGReadout = 39;
    static const bit kTDCId = 40;
    static const bit kSelectBypassInputs = 44;
    static const bit kReadoutFIFOSize = 45;
    static const bit kRejectCountOffset = 48;
    static const bit kSearchWindow = 60;
    static const bit kMatchWindow = 72;
    static const bit kLeadingResolution = 84;
    static const bit kMaxEventSize = 116;
    static const bit kRejectFIFOFull = 120;
    static const bit kEnableReadoutOccupancy = 121;
    static const bit kEnableReadoutSeparator = 122;
    static const bit kEnableOverflowDetect = 123;
    static const bit kEnableRelative = 124;
    static const bit kEnableAutomaticReject = 125;
    static const bit kEventCountOffset = 126;
    static const bit kTriggerCountOffset = 138;
    static const bit kEnableSetCountersOnBunchReset = 150;
    static const bit kEnableMasterResetCode = 151;
    static const bit kEnableMasterResetOnEventReset = 152;
    static const bit kEnableResetChannelBufferWhenSeparator = 153;
    static const bit kEnableSeparatorOnEventReset = 154;
    static const bit kEnableSeparatorOnBunchReset = 155;
    static const bit kEnableDirectEventReset = 156;
    static const bit kEnableDirectBunchReset = 157;
    static const bit kEnableDirectTrigger = 158;
    static const bit kOffset0 = 438;
    static const bit kCoarseCountOffset = 447;
    static const bit kDLLTapAdjust0 = 459;
    static const bit kRCAdjust0 = 555;
    static const bit kLowPowerMode = 570;
    static const bit kWidthSelect = 571;
    static const bit kVernierOffset = 575;
    static const bit kDLLControl = 580;
    static const bit kDeadTime = 584;
    static const bit kTestInvert = 586;
    static const bit kTestMode = 587;
    static const bit kTrailing = 588;
    static const bit kLeading = 589;
    static const bit kModeRCCompression = 590;
    static const bit kModeRC = 591;
    static const bit kDLLMode = 592;
    static const bit kPLLControl = 594;
    static const bit kSerialClockDelay = 602;
    static const bit kIOClockDelay = 606;
    static const bit kCoreClockDelay = 610;
    static const bit kDLLClockDelay = 614;
    static const bit kSerialClockSource = 618;
    static const bit kIOClockSource = 620;
    static const bit kCoreClockSource = 622;
    static const bit kDLLClockSource = 624;
    static const bit kRollOver = 627;
    static const bit kEnableMatching = 639;
    static const bit kEnablePair = 640;
    static const bit kEnableTTLSerial = 641;
    static const bit kEnableTTLControl = 642;
    static const bit kEnableTTLReset = 643;
    static const bit kEnableTTLClock = 644;
    static const bit kEnableTTLHit = 645;
    static const bit kSetupParity = 646;
    
};

#endif
