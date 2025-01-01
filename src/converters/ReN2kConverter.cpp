#include <N2kMessages.h>

#include "ReN2kConverter.h"

void ReN2kConverter::getTimeFromN2k(const tN2kMsg& n2kMsg)
{
   if (n2kMsg.PGN == 126992L)
      handleSytemTime(n2kMsg);         // Just to get time from GNSS
   if (n2kMsg.PGN == 129029L)
      handleGNSS(n2kMsg);              // or this way
}

void ReN2kConverter::handleSytemTime(const tN2kMsg& N2kMsg)
{
   unsigned char sID;
   tN2kTimeSource timeSource;

   ParseN2kSystemTime(N2kMsg, sID, m_daysSince1970, m_secondsSinceMidnight, timeSource);
}

void ReN2kConverter::calculateWindData(const double& AWAinRad, const double& AWSinMs, const double& STWinMs, 
   const double& HDGinRad, double& TWAinRad, double& TWDinRad, double& TWSinMs)
{
   // Calculate true wind speed (TWS) and true wind angle (TWA)
   // Resolve the Apparent Wind Vector into x and y components:
   double AWA_x = AWSinMs * cos(AWAinRad);
   double AWA_y = AWSinMs * sin(AWAinRad);

   // Calculate the True Wind Vector components by subtracting the Boat Speed Vector
   double TWA_x = AWA_x - STWinMs;
   double TWA_y = AWA_y;

   // Combine the True Wind Vector components to calculate the True Wind Speed (TWS)
   TWSinMs = sqrt((TWA_x * TWA_x) + (TWA_y * TWA_y));

   // Calculate the True Wind Angle (TWA) using the inverse tangent function
   TWAinRad = atan2(TWA_y, TWA_x);

   // Normalize so the value is always 0-359 degrees
   double TWAinDeg = RadToDeg(TWAinRad);
   if (TWAinDeg < 0)
   {
      TWAinDeg += 360;
      TWAinRad = DegToRad(TWAinDeg);
   }

   // Calculate true wind direction TWD
   double TWDinDeg = RadToDeg(TWAinRad) + RadToDeg(HDGinRad);
   if (TWDinDeg > 360) TWDinDeg = TWDinDeg - 360;
   if (TWDinDeg < 0) TWDinDeg = TWDinDeg + 360;

   TWDinRad = DegToRad(TWDinDeg);
}

void ReN2kConverter::handleGNSS(const tN2kMsg& N2kMsg)
{
   unsigned char sID;
   double latitude;
   double longitude;
   double altitude;
   tN2kGNSStype GNSStype;
   tN2kGNSSmethod GNSSmethod;
   unsigned char nSatellites;
   double HDOP;
   double PDOP;
   double geoidalSeparation;
   unsigned char nReferenceStations;
   tN2kGNSStype referenceStationType;
   uint16_t referenceStationID;
   double ageOfCorrection;

   if (ParseN2kGNSS(N2kMsg, sID, m_daysSince1970, m_secondsSinceMidnight, latitude, longitude, altitude,
      GNSStype, GNSSmethod, nSatellites, HDOP, PDOP, geoidalSeparation,
      nReferenceStations, referenceStationType, referenceStationID, ageOfCorrection))
   {
   }
}

// Convert NMEA2000 values into a CAN Id
unsigned long ReN2kConverter::n2ktoCanID(unsigned char prio, unsigned long pgn, unsigned long src, unsigned char dest) 
{
   unsigned char canIdPF = (unsigned char)(pgn >> 8);

   if (canIdPF < 240)
   { // PDU1 format
      if ((pgn & 0xff) != 0)
         return 0; // for PDU1 format PGN lowest byte has to be 0 for the destination.

      return (((unsigned long)(prio & 0x7)) << 26 | pgn << 8 | ((unsigned long) dest) << 8 | (unsigned long) src);
   }
   else
   { // PDU2 format
      return (((unsigned long)(prio & 0x7)) << 26 | pgn << 8 | (unsigned long) src);
   }
}

void ReN2kConverter::addByteEscapedToBuf(unsigned char byteToAdd, size_t& idx, char* buf, int& byteSum)
{
   buf[idx++] = byteToAdd;
   byteSum += byteToAdd;

   if (byteToAdd == Escape)
   {
      buf[idx++] = Escape;
   }
}

void ReN2kConverter::printBuf(unsigned char len, const unsigned char* pData, bool AddLF) 
{
   for (int i = 0; i < len; i++)
   {
      if (i > 0)
      {
         Serial.print(F(","));
      };

      // Print bytes as hex.
      Serial.print(pData[i], 16);
   }

   if (AddLF)
      Serial.println(F(""));
}
