#include <N2kMessages.h>

#include "ReConverterN2kJson.h"

/*
Source: https://github.com/AK-Homberger/NMEA2000-Gateway-My-Boat-Edition/blob/main/NMEA2000-Gateway-MBE/N2kDataToNMEA0183.cpp
*/

void ReConverterN2kJson::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   switch (n2kMsg.PGN)
   {
      case 126992UL:
         HandleSystemDateTime(n2kMsg);
         break;
      case 127245UL:
         HandleRudderPosition(n2kMsg);
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
      case 128275UL:
         HandleLog(n2kMsg);
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
      case 130310UL:
         HandleEnvParams(n2kMsg);
         break;
   }

   // data will be send to output every 2 seconds...
   if (m_nextUpdateTime.IsTime())
   {
      m_nextUpdateTime.FromNow(m_updatePeriod);

      size = measureJson(m_data.doc);
      serializeJson(m_data.doc, output, size);

      strcat(output, "\r\n");
      size = strlen(output);
   }
}

size_t ReConverterN2kJson::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   m_data.toJSON();
   return measureJson(m_data.doc) + 1 + 2 + BUF_RESERVE;  // add space for the '\0' + '\r\n'
}

void ReConverterN2kJson::HandleSystemDateTime(const tN2kMsg& N2kMsg)
{
   unsigned char sID;
   tN2kTimeSource timeSource;

   ParseN2kSystemTime(N2kMsg, sID, m_data.DaysSince1970, m_data.GPSTime, timeSource);
}

void ReConverterN2kJson::HandleRudderPosition(const tN2kMsg& N2kMsg)
{
   tN2kRudderDirectionOrder rudderDirection;
   double angleOrder;
   double rudderPosition;
   uint8_t instance;

   if (ParseN2kRudder(N2kMsg, rudderPosition, instance, rudderDirection, angleOrder))
   {
      if (rudderDirection == N2kRDO_MoveToStarboard)
      {
         m_data.RudderPosition = -rudderPosition;
      }
      else
      {
         m_data.RudderPosition = rudderPosition;
      }
   }
}

void ReConverterN2kJson::HandleHeading(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kHeadingReference ref;
   double _Deviation = 0;
   double _Variation;

   if (ParseN2kHeading(N2kMsg, SID, m_data.Heading, _Deviation, _Variation, ref))
   {
      if (ref == N2khr_magnetic)
      {
         if (!N2kIsNA(_Variation))
            m_data.Variation = _Variation; // Update Variation
         if (!N2kIsNA(m_data.Heading) && !N2kIsNA(m_data.Variation))
            m_data.Heading += m_data.Variation; // Heading will store the TRUE heading
      }

      // bug in NMEA_Simulator sometimes sending heading data > 360 degrees
      double hdgAng = RadToDeg(m_data.Heading);
      if (hdgAng >= 360)
      {
         hdgAng -= 360;
         m_data.Heading = DegToRad(hdgAng);
      }
   }
}

void ReConverterN2kJson::HandleVariation(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kMagneticVariation Source;

   ParseN2kMagneticVariation(N2kMsg, SID, Source, m_data.DaysSince1970, m_data.Variation);
}

void ReConverterN2kJson::HandleBoatSpeed(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kSpeedWaterReferenceType SWRT;

   ParseN2kBoatSpeed(N2kMsg, SID, m_data.STW, m_data.SOG, SWRT);
}

void ReConverterN2kJson::HandleDepth(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   double Range;

   ParseN2kWaterDepth(N2kMsg, SID, m_data.WaterDepth, m_data.WaterDepthOffset, Range);
}

void ReConverterN2kJson::HandlePosition(const tN2kMsg& N2kMsg)
{
   ParseN2kPGN129025(N2kMsg, m_data.Latitude, m_data.Longitude);
}

void ReConverterN2kJson::HandleCOGSOG(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   tN2kHeadingReference HeadingReference;
  
   if (ParseN2kCOGSOGRapid(N2kMsg, SID, HeadingReference, m_data.COG, m_data.SOG))
   {
      if (HeadingReference == N2khr_magnetic)
      {
         m_data.COG += m_data.Variation; // COG will store the TRUE COG
      }
   }
}

void ReConverterN2kJson::HandleGNSS(const tN2kMsg& N2kMsg)
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

   ParseN2kGNSS(N2kMsg, SID, m_data.DaysSince1970, m_data.GPSTime, m_data.Latitude, m_data.Longitude,
                m_data.Altitude, GNSStype, GNSSmethod, nSatellites, HDOP, PDOP, GeoidalSeparation,
                nReferenceStations, ReferenceStationType, ReferenceSationID, AgeOfCorrection);
}

void ReConverterN2kJson::HandleWind(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   double WindSpeed;
   double WindAngle;
   tN2kWindReference WindReference;
   static double FilteredAWS = 0;

   if (ParseN2kWindSpeed(N2kMsg, SID, WindSpeed, WindAngle, WindReference))
   {      
      if (WindReference == N2kWind_Apparent)
      {
         m_data.AWA = WindAngle;
         m_data.AWS = WindSpeed;
      }
      else if (WindReference == N2kWind_True_North)
      {
         m_data.TWD = WindAngle;
         m_data.TWS = WindSpeed;
      }
      else if (WindReference == N2kWind_True_water)
      {
         m_data.TWA = WindAngle;
         m_data.TWS = WindSpeed;
      }

      calculateWindData(m_data.AWA, m_data.AWS, m_data.STW, m_data.Heading, m_data.TWA, m_data.TWD, m_data.TWS);

      // Store max values for AWS and TWS
      // This implements a low pass filter to eliminate spike for AWS readings
      FilteredAWS = ((FilteredAWS * 3) + m_data.AWS) / 4; 
      
      if (FilteredAWS > m_data.MaxAws)
      {
         m_data.MaxAws = FilteredAWS;
      }

      if (m_data.TWS > m_data.MaxTws) 
      {
         m_data.MaxTws = m_data.TWS;
      }
   }
}

void ReConverterN2kJson::HandleEnvParams(const tN2kMsg &N2kMsg)
{
   unsigned char SID;
   double WaterTemperature;
   double OutsideAmbientAirTemperature;
   double AtmosphericPressure;

   if (ParseN2kPGN130310(N2kMsg, SID, WaterTemperature, OutsideAmbientAirTemperature, AtmosphericPressure))
   {
      m_data.WaterTemperature = WaterTemperature;
      m_data.AtmPressure = AtmosphericPressure;
   }
}

void ReConverterN2kJson::HandleLog(const tN2kMsg& N2kMsg) 
{
  ParseN2kDistanceLog(N2kMsg, m_data.DaysSince1970, m_data.GPSTime, m_data.Log, m_data.TripLog);
}
