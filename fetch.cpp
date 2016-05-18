#include "FPGAHandler.h"
#include "TDCEvent.h"

#include <iostream>

using namespace std;

FPGAHandler* h = 0;
int gEnd = 0;

void CtrlC(int aSig) {
  if (gEnd==0) {
    cout << endl << "[C-c] Trying a clean exit!" << endl;
    if (h) h->Stop();
  }
  else if (gEnd>=5) {
    cout << endl << "[C-c > 5 times] ... Forcing exit!" << endl;
    exit(0);
  }
  gEnd++;
}

static bool writing = false;

void
callback(PQBULKSTREAM stream)
{
  if (!stream) return;

  unsigned int filtered = 0;
  if (stream->Error) {
    std::cout << "--> CR: ERROR! Failed with error " << stream->Error << " (" << stream->BytesTransferred << " of " << stream->BytesRequested << " bytes)" << std::endl;
    return;
  }
  while (true) {
    if (writing) continue;

    writing = true;
    for (unsigned int j=0; j<0x10000/2; j++) {
      unsigned short word = (stream->Buffer[2*j]<<8)+stream->Buffer[2*j+1]; // group the two bytes into a 16-bit word
      if (word==0x00ff) { filtered++; continue; }
      std::cout << std::hex << word << std::endl; //FIXME delete me!!
    }
    writing = false;
    return;
  }
  std::cout << "haha -> " << stream->StreamID << " --- " << stream->RequestID << " --> filtered " << filtered << std::endl;
}

int main(int argc, char* argv[])
{
  signal(SIGINT, CtrlC);

  h = new FPGAHandler(1987, "/dev/usbmon");

  h->StartBulkTransfer(callback);
  sleep(1);
  h->StopBulkTransfer();

  /*try {
    //h->Connect();
    //h->SetTDCSetup(config);
    h->OpenFile();
  } catch (Exception& e) {
    e.Dump();
  }

  cout << " --> Output filename: " << h->GetFilename() << endl;
  cout << endl << "*** Ready for acquisition! ***" << endl << endl;
  
  int total_nevts = 0, nevts;
  while (true) {
    if (!h) return -1;
    nevts = h->ReadBuffer();
    if (nevts<0) break;
    total_nevts += nevts;
  }
  cout << "Number of events collected: " << total_nevts << endl;*/
  
  delete h;
  return 0;
}
