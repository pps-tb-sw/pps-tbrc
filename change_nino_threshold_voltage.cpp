#include "VMEReader.h"
#include "VME_FPGAUnitV1495.h"

using namespace std;

VMEReader* vme;
int gEnd = 0;

int main(int argc, char *argv[]) {
  uint32_t address =  0x32100000;
  bool with_socket = true;

  vme = new VMEReader("/dev/a2818_0", VME::CAEN_V2718, with_socket);
  try { vme->AddFPGAUnit(address); } catch (Exception& e) { e.Dump(); }
  VME::FPGAUnitV1495* fpga = vme->GetFPGAUnit();
  const bool use_fpga = (fpga!=0);

  uint32_t Vth = 10;
  while(Vth != 0xFFFF){
     std::cout << "How much should the threshold value? ";
     std::cin >> Vth;
     std::cout << std::endl;
     if (use_fpga) {
       fpga->SetThresholdVoltage(Vth, 0);
       std::cout << "You asked for: "<< Vth << " and the readback value of the Threshold Voltage is: " << fpga->GetThresholdVoltage(0) << std::endl;
     }
  }
  return 0;
}
