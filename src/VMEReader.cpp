#include "VMEReader.h"

VMEReader::VMEReader(const char *device, VME::BridgeType type, bool on_socket) :
  Client(1987), fBridge(0), fSG(0), fCAENET(0), fHV(0),
  fOnSocket(on_socket), fIsPulserStarted(false)
{
  try {
    if (fOnSocket) Client::Connect(DETECTOR);
    fBridge = new VME::BridgeVx718(device, type);
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
}

VMEReader::~VMEReader()
{
  if (fOnSocket) {
    Client::Send(Exception(__PRETTY_FUNCTION__, "Stopping the acquisition process", Info, 30001));
    Client::Disconnect();
  }
  if (fSG) delete fSG;
  if (fIsPulserStarted) fBridge->StopPulser();
  if (fBridge) delete fBridge;
  if (fHV) delete fHV;
  if (fCAENET) delete fCAENET;
}

void
VMEReader::ReadXML(const char* filename)
{
  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename);
  if (doc.Error()) {
    std::ostringstream os;
    os << "Error while trying to parse the configuration file \"" << filename << "\"" << "\n\t"
       << "Code: " << doc.ErrorID() << "\n\t"
       << "Dump of error:" << "\n"
       << doc.GetErrorStr1();
    throw Exception(__PRETTY_FUNCTION__, os.str(), Fatal);
  }
  if (tinyxml2::XMLElement* atrig=doc.FirstChildElement("triggering")) {
    if (const char* mode=atrig->Attribute("mode")) {
      std::ostringstream os; os << "Triggering mode: ";
      if (!strcmp(mode,"continuous_storage")) { fGlobalAcqMode = ContinuousStorage; os << "Continuous storage (manual)"; }
      if (!strcmp(mode,"trigger_start"))      { fGlobalAcqMode = TriggerStart; os << "Continuous storage (trigger on start)"; }
      if (!strcmp(mode,"trigger_matching"))   { fGlobalAcqMode = TriggerMatching; os << "Trigger matching"; }
      if (fOnSocket) Client::Send(Exception(__PRETTY_FUNCTION__, os.str(), Info));
    }
  }
  for (tinyxml2::XMLElement* afpga=doc.FirstChildElement("fpga"); afpga!=NULL; afpga=afpga->NextSiblingElement("fpga")) {
    if (const char* address=afpga->Attribute("address")) {
      unsigned long addr = static_cast<unsigned long>(strtol(address, NULL, 0));
      if (!addr) throw Exception(__PRETTY_FUNCTION__, "Failed to parse FPGA's base address", Fatal);
      try {
        try { AddFPGAUnit(addr); } catch (Exception& e) { if (fOnSocket) Client::Send(e); }
        VME::FPGAUnitV1495* fpga = GetFPGAUnit(addr);
        if (const char* type=afpga->Attribute("type")) {
          if (strcmp(type, "tdc_fanout")==0) fpga->SetTDCControlFanout(true);
        }
        VME::FPGAUnitV1495Control control = fpga->GetControl();
        if (tinyxml2::XMLElement* clock=afpga->FirstChildElement("clock")) {
          if (tinyxml2::XMLElement* source=clock->FirstChildElement("source")) {
            if (!strcmp(source->GetText(),"internal")) control.SetClockSource(VME::FPGAUnitV1495Control::InternalClock);
            if (!strcmp(source->GetText(),"external")) control.SetClockSource(VME::FPGAUnitV1495Control::ExternalClock);
          }
          if (tinyxml2::XMLElement* period=clock->FirstChildElement("period")) {
            fpga->SetInternalClockPeriod(atoi(period->GetText()));
          }
        }
        if (tinyxml2::XMLElement* trig=afpga->FirstChildElement("trigger")) {
          if (tinyxml2::XMLElement* source=trig->FirstChildElement("source")) {
            if (!strcmp(source->GetText(),"internal")) control.SetTriggerSource(VME::FPGAUnitV1495Control::InternalTrigger);
            if (!strcmp(source->GetText(),"external")) control.SetTriggerSource(VME::FPGAUnitV1495Control::ExternalTrigger);
          }
          if (tinyxml2::XMLElement* period=trig->FirstChildElement("period")) {
            fpga->SetInternalTriggerPeriod(atoi(period->GetText()));
          }
        }
        if (tinyxml2::XMLElement* sig=afpga->FirstChildElement("signal")) {
          if (tinyxml2::XMLElement* source=sig->FirstChildElement("source")) {
            if (!strcmp(source->GetText(),"internal")) for (unsigned int i=0; i<2; i++) control.SetSignalSource(i, VME::FPGAUnitV1495Control::InternalSignal);
            if (!strcmp(source->GetText(),"external")) for (unsigned int i=0; i<2; i++) control.SetSignalSource(i, VME::FPGAUnitV1495Control::ExternalSignal);
          }
          if (tinyxml2::XMLElement* poi=sig->FirstChildElement("poi")) {
            fpga->SetOutputPulserPOI(atoi(poi->GetText()));
          }
        }
	if (tinyxml2::XMLElement* vth=afpga->FirstChildElement("threshold")) {
          if (tinyxml2::XMLElement* tdc0=vth->FirstChildElement("tdc0")) {
            fpga->SetThresholdVoltage(atoi(tdc0->GetText()), 0);
          }
          if (tinyxml2::XMLElement* tdc1=vth->FirstChildElement("tdc1")) {
            fpga->SetThresholdVoltage(atoi(tdc1->GetText()), 1);
          }
          if (tinyxml2::XMLElement* tdc2=vth->FirstChildElement("tdc2")) {
            fpga->SetThresholdVoltage(atoi(tdc2->GetText()), 2);
          }
          if (tinyxml2::XMLElement* tdc3=vth->FirstChildElement("tdc3")) {
            fpga->SetThresholdVoltage(atoi(tdc3->GetText()), 3);
          }
        }
        switch (fGlobalAcqMode) {
          case ContinuousStorage:
          case TriggerStart:
            control.SetTriggeringMode(VME::FPGAUnitV1495Control::ContinuousStorage); break;
          case TriggerMatching:
            control.SetTriggeringMode(VME::FPGAUnitV1495Control::TriggerMatching); break;
        }
        fpga->SetControl(control);
        fpga->GetControl().Dump();
        fpga->DumpFWInformation();
      } catch (Exception& e) { e.Dump(); if (fOnSocket) Client::Send(e); throw e; }
    }
    else throw Exception(__PRETTY_FUNCTION__, "Failed to extract FPGA's base address", Fatal);
  }
  unsigned int tdc_id = 0;
  for (tinyxml2::XMLElement* atdc=doc.FirstChildElement("tdc"); atdc!=NULL; atdc=atdc->NextSiblingElement("tdc"), tdc_id++) {
    if (const char* address=atdc->Attribute("address")) {
      unsigned long addr = static_cast<unsigned long>(strtol(address, NULL, 0));
      if (!addr) throw Exception(__PRETTY_FUNCTION__, "Failed to parse TDC's base address", Fatal);
      try {
        try { AddTDC(addr); } catch (Exception& e) { if (fOnSocket) Client::Send(e); }
        VME::TDCV1x90* tdc = GetTDC(addr);
        uint16_t poi_group1 = 0xffff, poi_group2 = 0xffff;
        std::string detector_name = "unknown";
        switch (fGlobalAcqMode) {
          case ContinuousStorage:
          case TriggerStart:
            tdc->SetAcquisitionMode(VME::CONT_STORAGE); break;
          case TriggerMatching:
            tdc->SetAcquisitionMode(VME::TRIG_MATCH); break;
        }
        //std::cout << triggering_mode << " --> " << tdc->GetAcquisitionMode() << std::endl;
        if (tinyxml2::XMLElement* verb=atdc->FirstChildElement("verbosity")) {
          tdc->SetVerboseLevel(atoi(verb->GetText()));
        }
        if (tinyxml2::XMLElement* det=atdc->FirstChildElement("detector")) {
          detector_name = det->GetText();
        }
	if (tinyxml2::XMLElement* det=atdc->FirstChildElement("det_mode")) {
	  if (!strcmp(det->GetText(),"trailead")) tdc->SetDetectionMode(VME::TRAILEAD);
	  if (!strcmp(det->GetText(),"leading")) tdc->SetDetectionMode(VME::OLEADING);
	  if (!strcmp(det->GetText(),"trailing")) tdc->SetDetectionMode(VME::OTRAILING);
	  if (!strcmp(det->GetText(),"pair")) tdc->SetDetectionMode(VME::PAIR);
        }
	if (tinyxml2::XMLElement* dll=atdc->FirstChildElement("dll")) {
	  if (!strcmp(dll->GetText(),"Direct_Low_Resolution")) tdc->SetDLLClock(VME::TDCV1x90::DLL_Direct_LowRes);
	  if (!strcmp(dll->GetText(),"PLL_Low_Resolution")) tdc->SetDLLClock(VME::TDCV1x90::DLL_PLL_LowRes);
	  if (!strcmp(dll->GetText(),"PLL_Medium_Resolution")) tdc->SetDLLClock(VME::TDCV1x90::DLL_PLL_MedRes);
	  if (!strcmp(dll->GetText(),"PLL_High_Resolution")) tdc->SetDLLClock(VME::TDCV1x90::DLL_PLL_HighRes);
        }
        if (tinyxml2::XMLElement* poi=atdc->FirstChildElement("poi")) {
          if (tinyxml2::XMLElement* g0=poi->FirstChildElement("group0")) { poi_group1 = atoi(g0->GetText()); }
          if (tinyxml2::XMLElement* g1=poi->FirstChildElement("group1")) { poi_group2 = atoi(g1->GetText()); }
        }
        tdc->SetPoI(poi_group1, poi_group2);
	if (atdc->FirstChildElement("ettt")) { tdc->SetETTT(); }
	if (tinyxml2::XMLElement* wind=atdc->FirstChildElement("trigger_window")) {
          if (tinyxml2::XMLElement* width=wind->FirstChildElement("width")) { tdc->SetWindowWidth(atoi(width->GetText())); }
          if (tinyxml2::XMLElement* offset=wind->FirstChildElement("offset")) { tdc->SetWindowOffset(atoi(offset->GetText())); }
        }
        OnlineDBHandler().SetTDCConditions(tdc_id, addr, tdc->GetAcquisitionMode(), tdc->GetDetectionMode(), detector_name);
      } catch (Exception& e) { throw e; }
    }
  }
  for (tinyxml2::XMLElement* acfd=doc.FirstChildElement("cfd"); acfd!=NULL; acfd=acfd->NextSiblingElement("cfd")) {
    if (const char* address=acfd->Attribute("address")) {
      unsigned long addr = static_cast<unsigned long>(strtol(address, NULL, 0));
      if (!addr) throw Exception(__PRETTY_FUNCTION__, "Failed to parse CFD's base address", Fatal);
      try {
        try { AddCFD(addr); } catch (Exception& e) { if (fOnSocket) Client::Send(e); }
        VME::CFDV812* cfd = GetCFD(addr);
        if (tinyxml2::XMLElement* poi=acfd->FirstChildElement("poi")) { cfd->SetPOI(atoi(poi->GetText())); }
        if (tinyxml2::XMLElement* ow=acfd->FirstChildElement("output_width")) {
          if (tinyxml2::XMLElement* g0=ow->FirstChildElement("group0")) { cfd->SetOutputWidth(0, atoi(g0->GetText())); }
          if (tinyxml2::XMLElement* g1=ow->FirstChildElement("group1")) { cfd->SetOutputWidth(1, atoi(g1->GetText())); }
        }
        if (tinyxml2::XMLElement* dt=acfd->FirstChildElement("dead_time")) {
          if (tinyxml2::XMLElement* g0=dt->FirstChildElement("group0")) { cfd->SetDeadTime(0, atoi(g0->GetText())); }
          if (tinyxml2::XMLElement* g1=dt->FirstChildElement("group1")) { cfd->SetDeadTime(1, atoi(g1->GetText())); }
        }
        if (tinyxml2::XMLElement* thr=acfd->FirstChildElement("threshold")) {
          for (tinyxml2::XMLElement* ch=thr->FirstChildElement("channel"); ch!=NULL; ch=ch->NextSiblingElement("channel")) {
            cfd->SetThreshold(atoi(ch->Attribute("id")), atoi(ch->GetText()));
            std::cout << "Threshold for channel " << atoi(ch->Attribute("id")) << " set to " << ch->GetText() << std::endl;
          }
        }
      } catch (Exception& e) { throw e; }
    }
  }
  for (tinyxml2::XMLElement* asca=doc.FirstChildElement("scaler"); asca!=NULL; asca=asca->NextSiblingElement("scaler")) {
    if (const char* address=asca->Attribute("address")) {
      unsigned long addr = static_cast<unsigned long>(strtol(address, NULL, 0));
      if (!addr) throw Exception(__PRETTY_FUNCTION__, "Failed to parse scaler's base address", Fatal);
      try {
        try { AddScaler(addr); } catch (Exception& e) { if (fOnSocket) Client::Send(e); }
        VME::ScalerV8x0* sca = GetScaler(addr);
	VME::ScalerV8x0Control control = sca->GetControl();
        if (const char* hdr=asca->Attribute("header")) {
	  if (!strcmp(hdr, "true") or !strcmp(hdr, "True") or !strcmp(hdr, "1")) control.SetHeader(true);
	  if (!strcmp(hdr, "false") or !strcmp(hdr, "False") or !strcmp(hdr, "0")) control.SetHeader(false);
	}
	if (tinyxml2::XMLElement* poi=asca->FirstChildElement("poi")) { sca->SetPOI(atoi(poi->GetText())); }
	if (tinyxml2::XMLElement* df=asca->FirstChildElement("data_format")) {
	  if (!strcmp(df->GetText(), "26bit")) control.SetDataFormat(VME::DF26bit);
	  if (!strcmp(df->GetText(), "32bit")) control.SetDataFormat(VME::DF32bit);
	}
	sca->SetControl(control);
      } catch (Exception& e) { throw e; }
    }
  }
  std::cout << "Global acquisition mode: " << fGlobalAcqMode << std::endl;
  unsigned int run = GetRunNumber();
  std::ifstream source(filename, std::ios::binary);
  std::stringstream out_name; out_name << std::getenv("PPS_PATH") << "/config/config_run" << run << ".xml";
  std::ofstream dest(out_name.str().c_str(), std::ios::binary);
  dest << source.rdbuf();
  if (fOnSocket) Client::Send(Exception(__PRETTY_FUNCTION__, "Ready to release veto!", Info));

  //doc.Print();
}

unsigned int
VMEReader::GetRunNumber() const
{
  if (!fOnSocket) return 0;
  SocketMessage msg;
  try {
    msg = Client::SendAndReceive(SocketMessage(GET_RUN_NUMBER), RUN_NUMBER);
    return static_cast<unsigned int>(msg.GetIntValue());
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  return 0;
}

void
VMEReader::AddTDC(uint32_t address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    fTDCCollection.insert(std::pair<uint32_t,VME::TDCV1x90*>(
      address,
      new VME::TDCV1x90(fBridge->GetHandle(), address)
    ));
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  std::ostringstream os;
  os << "TDC with base address 0x" << std::hex << address << " successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::AddScaler(uint32_t address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    fScalerCollection.insert(std::pair<uint32_t,VME::ScalerV8x0*>(
      address,
      new VME::ScalerV8x0(fBridge->GetHandle(), address)
    ));
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  std::ostringstream os;
  os << "Scaler with base address 0x" << std::hex << address << " successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::AddCFD(uint32_t address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    fCFDCollection.insert(std::pair<uint32_t,VME::CFDV812*>(
      address,
      new VME::CFDV812(fBridge->GetHandle(), address)
    ));
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  std::ostringstream os; os << "CFD with base address 0x" << std::hex << address << " successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::AddIOModule(uint32_t address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    fSG = new VME::IOModuleV262(fBridge->GetHandle(), address);
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  std::ostringstream os; os << "I/O module with base address 0x" << std::hex << address << " successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::AddFPGAUnit(uint32_t address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    fFPGACollection.insert(std::pair<uint32_t,VME::FPGAUnitV1495*>(
      address,
      new VME::FPGAUnitV1495(fBridge->GetHandle(), address)
    ));
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
  sleep(4); // wait for FW to be ready...
  std::ostringstream os; os << "FPGA module with base address 0x" << std::hex << address << " successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::AddHVModule(uint32_t vme_address, uint16_t nim_address)
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  if (!fCAENET) fCAENET = new VME::CAENETControllerV288(fBridge->GetHandle(), vme_address);
  try {
    fHV = new NIM::HVModuleN470(nim_address, *fCAENET);
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
    std::ostringstream os;
    os << "Failed to add NIM HV module at address   0x" << std::hex << nim_address << "\n\t"
       << "through VME CAENET controller at address 0x" << std::hex << vme_address;
    throw Exception(__PRETTY_FUNCTION__, os.str(), JustWarning);
  }
  std::ostringstream os;
  os << "NIM HV module with address 0x" << std::hex << nim_address << "\n\t"
     << " (through VME CAENET controller at base address 0x" << std::hex << vme_address << ") successfully built";
  throw Exception(__PRETTY_FUNCTION__, os.str(), Info, TDC_ACQ_START);
}

void
VMEReader::Abort()
{
  if (!fBridge) throw Exception(__PRETTY_FUNCTION__, "No bridge detected! Aborting...", Fatal);
  try {
    for (VME::TDCCollection::iterator t=fTDCCollection.begin(); t!=fTDCCollection.end(); t++) {
      t->second->abort();
    }
  } catch (Exception& e) {
    e.Dump();
    if (fOnSocket) Client::Send(e);
  }
}

void
VMEReader::NewRun() const
{
  if (!fOnSocket) return;
  Client::Send(SocketMessage(NEW_RUN));  
  std::ostringstream os; os << "New run detected: " << GetRunNumber();
  Client::Send(Exception(__PRETTY_FUNCTION__, os.str(), JustWarning));  
}

void
VMEReader::SetOutputFile(uint32_t tdc_address, std::string filename)
{
  OutputFiles::iterator it = fOutputFiles.find(tdc_address);
  if (it!=fOutputFiles.end()) { it->second = filename; }
  else fOutputFiles.insert(std::pair<uint32_t, std::string>(tdc_address, filename));
}

void
VMEReader::SendOutputFile(uint32_t tdc_address) const
{
  if (!fOnSocket) return;
  OutputFiles::const_iterator it = fOutputFiles.find(tdc_address);
  if (it!=fOutputFiles.end()) {
    std::ostringstream os;
    os << tdc_address << ":" << it->second;
    Client::Send(SocketMessage(SET_NEW_FILENAME, os.str()));
  }
}

void
VMEReader::BroadcastNewBurst(unsigned int burst_id) const
{
  if (!fOnSocket) return;
  std::ostringstream os; os << "New output file detected: burst id " << burst_id;
  Client::Send(Exception(__PRETTY_FUNCTION__, os.str(), JustWarning));
}

void
VMEReader::BroadcastTriggerRate(unsigned int burst_id, unsigned long num_triggers) const
{
  std::ostringstream os; os << burst_id << ":" << num_triggers;
  Client::Send(SocketMessage(NUM_TRIGGERS, os.str()));
}

void
VMEReader::BroadcastHVStatus(unsigned short channel_id, const NIM::HVModuleN470ChannelValues& val) const
{
  std::ostringstream os; os << channel_id << ":" << val.ChannelStatus() << "," << val.Imon() << "," << val.Vmon();
  Client::Send(SocketMessage(HV_STATUS, os.str()));
}

void
VMEReader::LogHVValues(unsigned short channel_id, const NIM::HVModuleN470ChannelValues& val) const
{
  try {
    OnlineDBHandler().SetHVConditions(channel_id, val.V0(), val.I0());
  } catch (Exception& e) {
    e.Dump();
  }
}
