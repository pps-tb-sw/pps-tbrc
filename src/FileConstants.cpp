#include "FileConstants.h"

std::string
GenerateString(const size_t len)
{
  std::string out;
  srand(time(NULL));
  const char az[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  out = "";
  for (size_t i=0; i<len; i++) { out += az[rand()%(sizeof(az)-1)]; }
  return out;
}
