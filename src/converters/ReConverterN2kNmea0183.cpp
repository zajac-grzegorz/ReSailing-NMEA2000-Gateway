/*
ReConverterN2kNmea0183.cpp

Copyright (c) 2015-2018 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <N2kMessages.h>
#include <NMEA0183Messages.h>

#include "ReConverterN2kNmea0183.h"

/*
https://actisense.com/news/how-to-read-nmea-0183-data/

With most of the NMEA 0183 messages following a general structure, there are certain rules they must follow;

The sentence is limited to a maximum of 82 ASCII Characters, which compiles of 79 characters plus 
the starting delimiter and the ending <CR><LF>.

A sentence will have a minimum of one field within it.
There isn’t a specified limit on the number of fields you can have within a sentence, but this is of course 
limited by the maximum number of ASCII Characters supported in a single message.
Every sentence will start with a delimiting character of ‘$’ or ‘!’, and will end with a terminating <CR><LF>.
There are certain specific sentences which may differ from the standard structure and have different restrictions. 
These are all documented within the NMEA’s 0183 standard.
*/

void ReConverterN2kNmea0183::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   Update();

   // get RMC message into output buffer
   size_t maxSize = MAX_NMEA0183_MSG_LEN + 10;

   // Protection to add RMC message only once
   if (NMEA0183Msg.IsMessageCode("RMC") && MsgReadyToSend)
   {
      if (!NMEA0183Msg.GetMessage(output, maxSize))
      {
         return;
      }

      // Add the NMEA separator \\r\\n and update size value
      strcat(output, "\r\n");
      size = strlen(output);

      MsgReadyToSend = false;
   }

   switch (n2kMsg.PGN)
   {
      case 126992UL:
         HandleSystemDateTime(n2kMsg);
         break;
      case 127250UL:
         HandleHeading(n2kMsg);
         break;
      case 127258UL:
         HandleVariation(n2kMsg);
         break;
      case 128259UL:
         HandleBoatSpeed(n2kMsg);
         break;
      case 128267UL:
         HandleDepth(n2kMsg);
         break;
      case 129025UL:
         HandlePosition(n2kMsg);
         break;
      case 129026UL:
         HandleCOGSOG(n2kMsg);
         break;
      case 129029UL:
         HandleGNSS(n2kMsg);
         break;
      case 130306UL:
         HandleWind(n2kMsg);
         break;
   }

   // Protection to not add RMC message for the second time
   if (!NMEA0183Msg.IsMessageCode("RMC") && MsgReadyToSend)
   {
      if (!NMEA0183Msg.GetMessage(output + size, maxSize))
      {
         return;
      }

      // Add the NMEA separator \\r\\n and update size value
      strcat(output, "\r\n");
      size = strlen(output);

      MsgReadyToSend = false;
   }
}

size_t ReConverterN2kNmea0183::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   // in one conversion we may have 2 NMEA0183 messages in a buffer at once - one RMC message + another one
   // therefore size for 2 messages is needed in a buffer + some reserve (81 * 2 + reserve (50) = 212 bytes)
   return MAX_NMEA0183_MSG_LEN * 2 + BUF_RESERVE;
}

ReConverterN2kNmea0183::ReConverterN2kNmea0183()
{
   Latitude = N2kDoubleNA;
   Longitude = N2kDoubleNA;
   Altitude = N2kDoubleNA;
   Variation = N2kDoubleNA;
   Heading = N2kDoubleNA;
   COG = N2kDoubleNA;
   SOG = N2kDoubleNA;
   SecondsSinceMidnight = N2kDoubleNA;
   DaysSince1970 = N2kUInt16NA;
   LastPosSend = 0;
   NextRMCSend = millis() + RMCPeriod;
   LastHeadingTime = 0;
   LastCOGSOGTime = 0;
   LastPositionTime = 0;
   LastWindTime = 0;
}

void ReConverterN2kNmea0183::Update()
{
   SendRMC();

   if (LastHeadingTime + 2000 < millis())
      Heading = N2kDoubleNA;
   
   if (LastCOGSOGTime + 2000 < millis())
   {
      COG = N2kDoubleNA;
      SOG = N2kDoubleNA;
   }
   
   if (LastPositionTime + 4000 < millis())
   {
      Latitude = N2kDoubleNA;
      Longitude = N2kDoubleNA;
   }
   
   if (LastWindTime + 2000 < millis())
   {
      WindSpeed = N2kDoubleNA;
      WindAngle = N2kDoubleNA;
   }
}

void ReConverterN2kNmea0183::HandleSystemDateTime(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   uint16_t SystemDate;
   double SystemTime;
   tN2kTimeSource TimeSource;
   
   // Start with empty message container
   NMEA0183Msg.Clear();

   if (ParseN2kSystemTime(N2kMsg, SID, SystemDate, SystemTime, TimeSource))
   {
      time_t t = tNMEA0183Msg::daysToTime_t(SystemDate);
      tmElements_t tm;
      tNMEA0183Msg::breakTime(t, tm);
      int GPSDay = tNMEA0183Msg::GetDay(tm);
      int GPSMonth = tNMEA0183Msg::GetMonth(tm);
      int GPSYear = tNMEA0183Msg::GetYear(tm);
      int LZD = 0;
      int LZMD = 0;

      if (NMEA0183SetZDA(NMEA0183Msg, SystemTime, GPSDay, GPSMonth, GPSYear, LZD, LZMD, "GP"))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::HandleHeading(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kHeadingReference ref;
   double _Deviation = 0;
   double _Variation;
   
   // Start with empty message container
   NMEA0183Msg.Clear();

   if (ParseN2kHeading(N2kMsg, SID, Heading, _Deviation, _Variation, ref))
   {
      if (ref == N2khr_magnetic)
      {
         if (!N2kIsNA(_Variation))
            Variation = _Variation; // Update Variation
         if (!N2kIsNA(Heading) && !N2kIsNA(Variation))
            Heading += Variation; // Heading will store the TRUE heading
      }
      LastHeadingTime = millis();

      // HDG message holds value from magnetic sensor, so need to provide magnetic heading
      if (NMEA0183SetHDG(NMEA0183Msg, Heading - Variation, _Deviation, Variation))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::HandleVariation(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kMagneticVariation Source;

   ParseN2kMagneticVariation(N2kMsg, SID, Source, DaysSince1970, Variation);
}

void ReConverterN2kNmea0183::HandleBoatSpeed(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   double WaterReferenced;
   double GroundReferenced;
   tN2kSpeedWaterReferenceType SWRT;

   if (ParseN2kBoatSpeed(N2kMsg, SID, WaterReferenced, GroundReferenced, SWRT))
   { 
      // Start with empty message container
      NMEA0183Msg.Clear();
      
      double MagneticHeading = (!N2kIsNA(Heading) && !N2kIsNA(Variation) ? Heading + Variation : NMEA0183DoubleNA);
      
      if (NMEA0183SetVHW(NMEA0183Msg, Heading, MagneticHeading, WaterReferenced))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::HandleDepth(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   double DepthBelowTransducer;
   double Offset;
   double Range;

   if (ParseN2kWaterDepth(N2kMsg, SID, DepthBelowTransducer, Offset, Range))
   { 
      // Start with empty message container
      NMEA0183Msg.Clear();
      
      if (NMEA0183SetDPT(NMEA0183Msg, DepthBelowTransducer, Offset))
      {
         SendMessage(NMEA0183Msg);
      }

      // if (NMEA0183SetDBx(NMEA0183Msg, DepthBelowTransducer, Offset))
      // {
      //    SendMessage(NMEA0183Msg);
      // }
   }
}

void ReConverterN2kNmea0183::HandlePosition(const tN2kMsg& N2kMsg)
{
   if (ParseN2kPGN129025(N2kMsg, Latitude, Longitude))
   {
      LastPositionTime = millis();
   }
}

void ReConverterN2kNmea0183::HandleCOGSOG(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kHeadingReference HeadingReference;
   
   // Start with empty message container
   NMEA0183Msg.Clear();

   if (ParseN2kCOGSOGRapid(N2kMsg, SID, HeadingReference, COG, SOG))
   {
      LastCOGSOGTime = millis();
      double MCOG = (!N2kIsNA(COG) && !N2kIsNA(Variation) ? COG - Variation : NMEA0183DoubleNA);

      if (HeadingReference == N2khr_magnetic)
      {
         MCOG = COG;
         if (!N2kIsNA(Variation))
         {
            COG += Variation; // COG will store the TRUE COG
         }
      }
      
      if (NMEA0183SetVTG(NMEA0183Msg, COG, MCOG, SOG))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::HandleGNSS(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kGNSStype GNSStype;
   tN2kGNSSmethod GNSSmethod;
   unsigned char nSatellites;
   double HDOP;
   double PDOP;
   double GeoidalSeparation;
   unsigned char nReferenceStations;
   tN2kGNSStype ReferenceStationType;
   uint16_t ReferenceSationID;
   double AgeOfCorrection;
   
   // Start with empty message container
   NMEA0183Msg.Clear();

   if (ParseN2kGNSS(N2kMsg, SID, DaysSince1970, SecondsSinceMidnight, Latitude, Longitude, Altitude, GNSStype, GNSSmethod,
      nSatellites, HDOP, PDOP, GeoidalSeparation,
      nReferenceStations, ReferenceStationType, ReferenceSationID, AgeOfCorrection))
   {
      LastPositionTime = millis();

      if (NMEA0183SetGGA(NMEA0183Msg, SecondsSinceMidnight, Latitude, Longitude, static_cast<uint32_t>(GNSSmethod), 
         nSatellites, HDOP, Altitude, GeoidalSeparation, AgeOfCorrection, 
         ReferenceSationID == N2kInt16NA ? NMEA0183UInt32NA : ReferenceSationID))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::HandleWind(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kWindReference WindReference;
   tNMEA0183WindReference NMEA0183Reference = NMEA0183Wind_True;

   if (ParseN2kWindSpeed(N2kMsg, SID, WindSpeed, WindAngle, WindReference))
   {
      // Start with empty message container
      NMEA0183Msg.Clear();
      
      LastWindTime = millis();

      if (WindReference == N2kWind_Apparent)
      {
         NMEA0183Reference = NMEA0183Wind_Apparent;
      }
      else if (WindReference == N2kWind_True_water)
      {
         NMEA0183Reference = NMEA0183Wind_True;
      }
      else
      {
         // consider adding wind conversion for NMEA0183 handler
         return;
      }

      if (NMEA0183SetMWV(NMEA0183Msg, RadToDeg(WindAngle), NMEA0183Reference, WindSpeed))
      {
         SendMessage(NMEA0183Msg);
      }
   }
}

void ReConverterN2kNmea0183::SendRMC()
{
   if (NextRMCSend <= millis() && !N2kIsNA(Latitude))
   {
      // Start with empty message container
      NMEA0183Msg.Clear();

      if (NMEA0183SetRMC(NMEA0183Msg, SecondsSinceMidnight, Latitude, Longitude, COG, SOG, DaysSince1970, Variation))
      {
         SendMessage(NMEA0183Msg);
      }

      SetNextRMCSend();
   }
}

void ReConverterN2kNmea0183::SendMessage(const tNMEA0183Msg& NMEA0183Msg)
{
   MsgReadyToSend = true;
}
