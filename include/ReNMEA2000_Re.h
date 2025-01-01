// Custom class because access to the IsFastPacket() method from NMEA2000 is needed
// and this method is protected

#pragma once

#include <NMEA2000_esp32.h>

class ReNMEA2000_Re : public tNMEA2000_esp32
{
   public:
      bool isFastPacketMsg(const tN2kMsg& n2kMsg)
      {
         return IsFastPacket(n2kMsg);
      }
};