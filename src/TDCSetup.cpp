#include "TDCSetup.h"

void
TDCSetup::SetConstantValues()
{
  SetTest(false);
  SetEnableErrorMark(true);
  SetEnableErrorBypass(false);
  SetEnableError(0x7ff); // 11 bits of '1'

  SetStrobeSelect(SS_NoStrobe);

  SetEnableGlobalTrailer(false); // FIXME not yet discussed...
  SetEnableGlobalHeader(false); // FIXME not yet discussed...

  SetReadoutSingleCycleSpeed(RSC_40Mbits_s); // FIXME don't care...
  SetSerialDelay(0x0); // FIXME maybe affected by the realistic tests
  SetReadoutSpeedSelect(RO_Fixed);
  SetTokenDelay(0x0); // FIXME maybe affected by the realistic tests
  
  SetEnableLocalTrailer(true); // FIXME not yet discussed...
  SetEnableLocalHeader(true); // FIXME not yet discussed...
  SetMaster(false);
  SetEnableBytewise(false);
  
  SetBypassInputs(false); // bit 44
  SetReadoutFIFOSize(256); // maximum
  SetMaxEventSize(-1); // no limit
  SetRejectFIFOFull(true);
  SetEnableReadoutOccupancy(false);
  SetEnableReadoutSeparator(false);
  SetEnableOverflowDetect(true);

  SetKeepToken(true);
  
  SetEnableRelative(true);
  SetEnableAutomaticReject(false);
  
  /*SetEventCountOffset(0); // FIXME needs confirmation
  SetTriggerCountOffset(0);*/
  
  SetEnableSetCountersOnBunchReset(false); // FIXME not yet discussed...
  SetEnableMasterResetCode(false);
  SetEnableMasterResetOnEventReset(false);
  SetEnableResetChannelBufferWhenSeparator(false); // FIXME not yet discussed...
  SetEnableSeparatorOnEventReset(false);
  SetEnableSeparatorOnBunchReset(false);
  SetEnableDirectEventReset(true);
  SetEnableDirectBunchReset(true);
  SetEnableDirectTrigger(true);
  
  SetLowPowerMode(true);
  SetDLLControl(0x1);
  
  //SetDeadTime(DT_5ns); // FIXME do we force the dead time value?
  //SetTestInvert(false);
  //SetTestMode(false);
  
  SetModeRCCompression(true);
  SetModeRC(true);
  SetDLLMode(DLL_320MHz);
  SetPLLControl(0x4, false, false, false);
  
  SetSerialClockDelay(false, 0x0);
  SetIOClockDelay(false, 0x0);
  SetCoreClockDelay(false, 0x0);
  SetDLLClockDelay(false, 0x0);
  SetSerialClockSource(Serial_pll_clock_80);

  SetIOClockSource(IO_clock_40);
  SetCoreClockSource(Core_clock_40);
  SetDLLClockSource(DLL_clock_40);
  
  SetRollOver(0xfff); // 4095
  SetEnableTTLSerial(true);
  SetEnableTTLControl(true);
  SetEnableTTLReset(true);
  SetEnableTTLClock(false);
  SetEnableTTLHit(false);

  SetEdgesPairing(false);
  SetTriggerMatchingMode(true);
  SetLeadingMode(true);
  SetTrailingMode(true);

  SetRCAdjustment(0, 0x7);
  SetRCAdjustment(1, 0x7);
  SetRCAdjustment(2, 0x4);
  SetRCAdjustment(3, 0x2);

  SetCoarseCountOffset(0);
  SetTriggerCountOffset(0);
  SetMatchWindow(198);
  SetSearchWindow(198);
  SetRejectCountOffset(0);
  SetTDCId(0);
  
  SetEnableJTAGReadout(false);

  SetSetupParity(true);
}

void
TDCSetup::Dump(int verb, std::ostream& os) const
{
  os << "====================="
     << " TDC Setup register dump "
     << "====================" << std::endl;
     if (verb>1) DumpRegister(os);
  os << " Enabled errors:             ";
  for (unsigned int i=0; i<11; i++) {
    if (static_cast<bool>((GetEnableError()>>i)&0x1)) os << i << " ";
  }
  os << std::endl;
  os << " Edge resolution:            " << GetEdgeResolution() << std::endl
     << " Maximal event size:         " << GetMaxEventSize() << std::endl
     << " Reject events if FIFO full? " << GetRejectFIFOFull() << std::endl
     << " Channels offset/DLL adjustments:" << std::endl
     << "   +---------------------------------------------------------+" << std::endl;
  for (unsigned int i=0; i<TDC_NUM_CHANNELS/2; i++ ) {
    os << "   |  Ch.  " << std::setw(2) << i
       << ":   0x" << std::setfill('0')
       << std::setw(3) << std::hex << static_cast<int>(GetChannelOffset(i))
       << " / 0x"
       << std::setw(3) << static_cast<int>(GetDLLAdjustment(i)) << std::dec << std::setfill(' ')
       << "   |  Ch.  " << std::setw(2) << i+TDC_NUM_CHANNELS/2
       << ":   0x" << std::setfill('0')
       << std::setw(3) << std::hex << static_cast<int>(GetChannelOffset(i+TDC_NUM_CHANNELS/2))
       << " / 0x"
       << std::setw(3) << static_cast<int>(GetDLLAdjustment(i+TDC_NUM_CHANNELS/2)) << std::dec << std::setfill(' ')
       << " |" << std::endl;
  }
  os << "   +---------------------------------------------------------+" << std::endl
     << " Width resolution:           " << GetWidthResolution() << std::endl
     << " Dead time:                  " << GetDeadTime() << std::endl
     << " Leading/trailing mode:      " << GetLeadingMode() << " / " << GetTrailingMode() << std::endl
     << " Trigger matching mode:      " << GetTriggerMatchingMode() << std::endl
     << " Edges pairing:              " << GetEdgesPairing() << std::endl;
  os << "================================="
     << "=================================" << std::endl;
}
