#ifndef VME_TDCMeasurement_h
#define VME_TDCMeasurement_h

#include <vector>
#include <map>

#include "VME_TDCEvent.h"

namespace VME
{
  /**
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date Jun 2015
   */
  class TDCMeasurement
  {
    public:
      inline TDCMeasurement() {;}
      inline TDCMeasurement(const std::vector<TDCEvent>& v) { SetEventsCollection(v); }
      inline ~TDCMeasurement() { fMap.clear(); }

      inline void Dump() {
        std::ostringstream os;
        /*for (std::map<Type,TDCEvent>::const_iterator e=fMap.begin(); e!=fMap.end(); e++) {
          os << "=> Type=" << e->first << std::endl;
        }*/
        os << "TDC/Channel Id: " << GetTDCId() << " / " << GetChannelId() << "\n\t"
           << "Event/bunch Id: " << GetEventId() << " / " << GetBunchId() << "\n\t";
        for (unsigned int i=0; i<NumEvents(); i++) {
          os << "----- Event " << i << " -----" << "\n\t"
             << "Leading time:   " << GetLeadingTime() << "\n\t"
             << "Trailing time:  " << GetTrailingTime() << "\n\t";
        }
        os << NumEvents() << " hits recorded" << "\n\t"
           << NumErrors() << " error words";
        PrintInfo(os.str());
      }

      inline void SetEventsCollection(const std::vector<TDCEvent>& v) {
        unsigned int num_measurements = 0;
        fMap.clear(); fEvents.clear();
        TDCEvent leading; bool has_leading = false;
        for (std::vector<TDCEvent>::const_iterator e=v.begin(); e!=v.end(); e++) {
          const TDCEvent::EventType type = e->GetType();
          if (type==TDCEvent::GlobalHeader)       { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::GlobalHeader, *e)); }
          else if (type==TDCEvent::GlobalTrailer) { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::GlobalTrailer, *e)); }
          else if (type==TDCEvent::TDCHeader)     { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::TDCHeader, *e)); }
          else if (type==TDCEvent::TDCTrailer)    { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::TDCTrailer, *e)); }
          else if (type==TDCEvent::ETTT)          { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::ETTT, *e)); }
          else if (type==TDCEvent::TDCError)      { fMap.insert(std::pair<TDCEvent::EventType,TDCEvent>(TDCEvent::TDCError, *e)); }
          else if (type==TDCEvent::TDCMeasurement) {
            if (!e->IsTrailing()) {
              if (has_leading) { // did not find a trailing edge for the previous measurement
                fEvents.push_back(std::pair<TDCEvent,TDCEvent>(leading, TDCEvent(0))); //FIXME no trailing edge for this hit
                num_measurements++;
                has_leading = false;
                continue;
              }
              leading = *e;
              has_leading = true;
            }
            else { // trailing edge
              if (!has_leading) {
                //throw Exception(__PRETTY_FUNCTION__, "Failed to retrieve leading/trailing edges", JustWarning);//return;
                leading = TDCEvent(); //FIXME
              }
              fEvents.push_back(std::pair<TDCEvent,TDCEvent>(leading, *e));
              num_measurements++;
            }
          }
        }
      }

      inline uint32_t GetLeadingTime(unsigned short event_id=0) {
        if (event_id>=fEvents.size()) { return 0; }
        return fEvents[event_id].first.GetTime();
      }
      inline bool HasTrailingEdge(unsigned short event_id=0) {
        if (event_id>=fEvents.size()) { return false; }
        if (fEvents[event_id].second.GetWord()==0) return false;
        return true;
      }
      inline uint32_t GetTrailingTime(unsigned short event_id=0) {
        if (event_id>=fEvents.size()) { return 0; }
        if (!HasTrailingEdge(event_id)) return 0;
        return fEvents[event_id].second.GetTime();
      }
      inline uint16_t GetToT(unsigned short event_id=0) {
        if (event_id>=fEvents.size()) { return 0; }
        uint32_t tt = GetTrailingTime(event_id), lt = GetLeadingTime(event_id);
        if ((tt-lt)>(1<<21)) tt += ((1<<21));
        return tt-lt;
      }
      inline uint16_t GetChannelId(unsigned short event_id=0) {
        if (event_id>=fEvents.size()) { return 0; }
        return fEvents[event_id].second.GetChannelId();
      }
      inline uint16_t GetTDCId() {
        if (!fMap.count(TDCEvent::TDCHeader)) { return 0; }
        return fMap[TDCEvent::TDCHeader].GetTDCId();
      }
      inline uint16_t GetEventId() {
        if (!fMap.count(TDCEvent::TDCHeader)) { return 0; }
        return fMap[TDCEvent::TDCHeader].GetEventId();
      }
      inline uint16_t GetBunchId() {
        if (!fMap.count(TDCEvent::TDCHeader)) { return 0; }
        return fMap[TDCEvent::TDCHeader].GetBunchId();
      }
      inline uint32_t GetETTT() {
        if (!fMap.count(TDCEvent::ETTT) or !fMap.count(TDCEvent::GlobalTrailer)) { return 0; }
        return (fMap[TDCEvent::ETTT].GetETTT()&0x07ffffff)*32+fMap[TDCEvent::GlobalTrailer].GetGeo();
      }
      inline size_t NumEvents() const { return fEvents.size(); }
      inline size_t NumErrors() const { return fMap.count(TDCEvent::TDCError); }

    private:
      std::map<TDCEvent::EventType,TDCEvent> fMap;
      std::vector< std::pair<TDCEvent,TDCEvent> > fEvents;
  };
}

#endif
