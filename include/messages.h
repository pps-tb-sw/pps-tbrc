#ifndef messages_h
#define messages_h

#include <algorithm>
#include <string>
#include <vector>

/**
 * Convert a char/"char,char,..." pair into a <string,string,string,...>
 * vector
 * \param[in] a A word representing the first message type provided
 * \param[in] b A word representing a comma-separated list of message keys
 * (starting from the second in the user-defined list)
 * \return A well-defined, homogenized vector of strings to be propagated
 * through the message keys builder
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 26 Mar 2015
 */
inline std::vector<std::string> sar(const char* a, const char* b)
{ 
  std::string o = b;
  std::vector<std::string> out;
  out.push_back(a);
  size_t pos; std::string token;
  o.erase(std::remove(o.begin(), o.end(), ' '), o.end());
  while ((pos=o.find(","))!=std::string::npos) {
    token = o.substr(0, pos);
    out.push_back(token.c_str());
    o.erase(0, pos+1);
  }
  return out;
}

/**
 * Generate a list of message types (with a struct / string matching), given a
 * set of computer-readable names provided as an argument.
 * \brief Message keys builder
 * \note FIXME: is it sufficiently obfuscated ? I don't think so...
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 26 Mar 2015
 */
#define MESSAGES_ENUM(m1, ...)\
  enum MessageKey { m1=0, __VA_ARGS__  };\
  inline std::string MessageKeyToString(MessageKey value) {\
    std::vector<std::string> s=sar(#m1, #__VA_ARGS__);\
    return (((unsigned int)value<s.size())&&(value>=0)) ? s[value] : s[0]; } \
  inline const MessageKey MessageKeyToObject(const char* value) {\
    std::vector<std::string> s=sar(#m1, #__VA_ARGS__);\
    for (size_t i=0; i<s.size(); i++) { if (s[i]==std::string(value)) return (MessageKey)i; }\
    return (MessageKey)(-1); }

#endif

