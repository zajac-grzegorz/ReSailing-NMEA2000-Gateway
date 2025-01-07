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
      case 129284UL:
         HandleNavigationData(n2kMsg);
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

void ReConverterN2kJson::normalizeAngle(double& angleInRad)
{
   double angleInDegrees = RadToDeg(angleInRad);

   if (angleInDegrees >= 360)
   {
      angleInDegrees -= 360;
      angleInRad = DegToRad(angleInDegrees);
   }
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
   double _Variation = 0;
   double _Hdg = 0;

   if (ParseN2kHeading(N2kMsg, SID, _Hdg, _Deviation, _Variation, ref))
   {
      m_data.Variation = !N2kIsNA(_Variation) ? _Variation : m_data.Variation; // Update Variation

      if (ref == N2khr_magnetic)
      {
         m_data.HeadingMag = _Hdg;
         m_data.Heading = m_data.HeadingMag + m_data.Variation; // Heading will store the TRUE heading
      }
      else if (ref == N2khr_true)
      {
         m_data.Heading = _Hdg;
         m_data.HeadingMag = m_data.Heading - m_data.Variation;
      }
      else
      {
         // possible N2khr_error or N2khr_Unavailable
         return;
      }

      // bug in NMEA_Simulator sometimes sending heading data > 360 degrees
      normalizeAngle(m_data.Heading);
      normalizeAngle(m_data.HeadingMag);
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
   tN2kHeadingReference ref;
   double _COG = 0;
  
   if (ParseN2kCOGSOGRapid(N2kMsg, SID, ref, _COG, m_data.SOG))
   {
      if (ref == N2khr_magnetic)
      {
         m_data.COGMag = _COG;
         m_data.COG = m_data.COGMag + m_data.Variation; // COG will store the TRUE COG
      }
      else if (ref == N2khr_true)
      {
         m_data.COG = _COG;
         m_data.COGMag = m_data.COG - m_data.Variation;
      }
      else
      {
         // possible N2khr_error or N2khr_Unavailable
         return;
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
      // else if (WindReference == N2kWind_True_North)
      // {
      //    m_data.TWD = WindAngle;
      //    m_data.TWS = WindSpeed;
      // }
      // else if (WindReference == N2kWind_Magnetic)
      // {
      //    m_data.TWDMag = WindAngle;
      //    m_data.TWS = WindSpeed;
      // }
      // else if ((WindReference == N2kWind_True_water) || (WindReference == N2kWind_True_boat))
      // {
      //    m_data.TWA = WindAngle;
      //    m_data.TWS = WindSpeed;
      // }
      else
      {
         // logger.debug(RE_TAG, "Wind type: %d", WindReference);

         // We are only interested in aparent wind, all others are calculated below
         return;
      }

      // Calculations will be made according to parameters set in config json file:
      // - use magnetic or true values for HDG, COG, TWD
      // - use HDG or COG
      // - use STW or SOG
      // default ares: true, HDG, STW

      bool isDirectionMagnetic = (std::string(config.get(key_sys_dir_type)) == "magnetic");

      double& TWDorMTWD = isDirectionMagnetic ? m_data.TWDMag : m_data.TWD;
      double STWorSOG = m_data.STW;
      double HDGorCOG = isDirectionMagnetic ? m_data.HeadingMag : m_data.Heading;
      
      std::string wndCalcBoatSpeed = config.get(key_wnd_calc_spd);
      std::string wndCalcBoatHeading = config.get(key_wnd_calc_hdg);

      if (wndCalcBoatHeading == "cog")
      {
         HDGorCOG = isDirectionMagnetic ? m_data.COGMag : m_data.COG;
         // logger.debug(RE_TAG, "Using %s for wind calculations", isDirectionMagnetic ? "MCOG" : "COG");
      }
      else
      {
         // logger.debug(RE_TAG, "Using %s for wind calculations", isDirectionMagnetic ? "MHDG" : "HDG");
      }

      if (wndCalcBoatSpeed == "sog")
      {
         STWorSOG = m_data.SOG;
         // logger.debug(RE_TAG, "Using SOG for wind calculations");
      }
      else
      {
         // logger.debug(RE_TAG, "Using STW for wind calculations");
      }

      calculateWindData(m_data.AWA, m_data.AWS, STWorSOG, HDGorCOG, m_data.TWA, TWDorMTWD, m_data.TWS);

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

void ReConverterN2kJson::HandleNavigationData(const tN2kMsg& N2kMsg)
{
   unsigned char SID;
   double DistanceToWaypoint;
   tN2kHeadingReference BearingReference;
   bool PerpendicularCrossed;
   bool ArrivalCircleEntered;
   tN2kDistanceCalculationType CalculationType;
   double ETATime;
   int16_t ETADate;
   double BearingOriginToDestinationWaypoint;
   double BearingPositionToDestinationWaypoint;
   uint32_t OriginWaypointNumber;
   uint32_t DestinationWaypointNumber;
   double DestinationLatitude;
   double DestinationLongitude;
   double WaypointClosingVelocity;

   if (ParseN2kNavigationInfo(N2kMsg, SID, DistanceToWaypoint, BearingReference, PerpendicularCrossed, ArrivalCircleEntered, 
      CalculationType, ETATime, ETADate, BearingOriginToDestinationWaypoint, BearingPositionToDestinationWaypoint,
      OriginWaypointNumber, DestinationWaypointNumber, DestinationLatitude, DestinationLongitude, WaypointClosingVelocity))
   {
      if (BearingReference == N2khr_magnetic)
      {
         m_data.WPTBRGMag = BearingPositionToDestinationWaypoint;
         m_data.WPTBRG = m_data.WPTBRGMag + m_data.Variation; // WPTBRG will store the TRUE WPTBRG
      }
      else if (BearingReference == N2khr_true)
      {
         m_data.WPTBRG = BearingPositionToDestinationWaypoint;
         m_data.WPTBRGMag = m_data.WPTBRG - m_data.Variation;
      }
      else
      {
         // possible N2khr_error or N2khr_Unavailable
         return;
      }

      m_data.WPTDST = DistanceToWaypoint;
   }
}
