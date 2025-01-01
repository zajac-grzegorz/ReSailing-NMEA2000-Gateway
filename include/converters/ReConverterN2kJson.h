#pragma once

#include <N2kTimer.h>
#include "ReBoatData.h"
#include "ReN2kConverter.h"

class ReConverterN2kJson : public ReN2kConverter
{
public:
   ReConverterN2kJson() : m_nextUpdateTime(0) {}
   void convert(const tN2kMsg& n2kMsg, char* output, size_t& size) override;
   size_t getMaxBufSize(const tN2kMsg& n2kMsg) override;

private:
   ReBoatData m_data;
   tN2kScheduler m_nextUpdateTime;
   const unsigned long m_updatePeriod = 2000;

   void HandleSystemDateTime(const tN2kMsg &N2kMsg); // 126992
   void HandleRudderPosition(const tN2kMsg &N2kMsg); // 127245
   void HandleHeading(const tN2kMsg &N2kMsg);        // 127250
   void HandleVariation(const tN2kMsg &N2kMsg);      // 127258
   void HandleBoatSpeed(const tN2kMsg &N2kMsg);      // 128259
   void HandleDepth(const tN2kMsg &N2kMsg);          // 128267
   void HandleLog(const tN2kMsg& N2kMsg);            // 128275 
   void HandlePosition(const tN2kMsg &N2kMsg);       // 129025
   void HandleCOGSOG(const tN2kMsg &N2kMsg);         // 129026
   void HandleGNSS(const tN2kMsg &N2kMsg);           // 129029
   void HandleWind(const tN2kMsg &N2kMsg);           // 130306
   void HandleEnvParams(const tN2kMsg &N2kMsg);      // 130311
};