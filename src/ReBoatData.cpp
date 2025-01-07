#include <N2kMessages.h>

#include "ReBoatData.h"

double ReBoatData::Heading = 0;
double ReBoatData::HeadingMag = 0;
double ReBoatData::Latitude = 0;
double ReBoatData::Longitude = 0;
double ReBoatData::SOG = 0;
double ReBoatData::COG = 0;
double ReBoatData::COGMag = 0;
double ReBoatData::STW = 0;
double ReBoatData::AWS = 0;
double ReBoatData::TWS = 0;
double ReBoatData::MaxAws = 0;
double ReBoatData::MaxTws = 0;
double ReBoatData::AWA = 0;
double ReBoatData::TWA = 0;
double ReBoatData::TWD = 0;
double ReBoatData::TWDMag = 0;
double ReBoatData::WPTBRG = 0;
double ReBoatData::WPTBRGMag = 0;
double ReBoatData::WPTDST = 0;
uint32_t ReBoatData::TripLog = 0;
uint32_t ReBoatData::Log = 0;
double ReBoatData::RudderPosition = 0;
double ReBoatData::WaterTemperature = 273.15;
double ReBoatData::WaterDepth = 0;
double ReBoatData::WaterDepthOffset = 0;
double ReBoatData::AtmPressure = 0;
double ReBoatData::Variation = 0;
double ReBoatData::Altitude = 0;
double ReBoatData::GPSTime = 0; // Secs since midnight,
uint16_t ReBoatData::DaysSince1970 = 0; // Days since 1970-01-01
JsonDocument ReBoatData::doc;

void ReBoatData::toJSON()
{
   doc.clear();
   
   doc["HDG"] = round(RadToDeg(Heading));
   doc["MHDG"] = round(RadToDeg(HeadingMag));
   doc["Lat"] = Latitude;
   doc["Lon"] = Longitude;
   doc["SOG"] = roundDouble(msToKnots(SOG));
   doc["COG"] = round(RadToDeg(COG));
   doc["MCOG"] = round(RadToDeg(COGMag));
   doc["STW"] = roundDouble(msToKnots(STW));
   doc["AWS"] = roundDouble(msToKnots(AWS));
   doc["TWS"] = roundDouble(msToKnots(TWS));
   doc["MaxAWS"] = roundDouble(msToKnots(MaxAws));
   doc["MaxTWS"] = roundDouble(msToKnots(MaxTws));
   doc["AWA"] = round(RadToDeg(AWA));
   doc["TWA"] = round(RadToDeg(TWA));
   doc["TWD"] = round(RadToDeg(TWD));
   doc["MTWD"] = round(RadToDeg(TWDMag));
   doc["WPTBRG"] = round(RadToDeg(WPTBRG));
   doc["MWPTBRG"] = round(RadToDeg(WPTBRGMag));
   doc["WPTDST"] = roundDouble(WPTDST);
   doc["TripLog"] = roundDouble(TripLog);
   doc["Log"] = roundDouble(Log);
   doc["RudderPosition"] = round(RadToDeg(RudderPosition));
   doc["WaterTemperature"] = roundDouble(KelvinToC(WaterTemperature));
   doc["WaterDepth"] = WaterDepth;
   doc["WaterDepthOffset"] = WaterDepthOffset;
   doc["AtmPressure"] = PascalTohPA(AtmPressure);
   doc["Variation"] = roundDouble(RadToDeg(Variation));
   doc["Altitude"] = roundDouble(Altitude);
   doc["GPSTime"] = GPSTime;
   doc["DaysSince1970"] = DaysSince1970;
}

double ReBoatData::roundDouble(double a)
{
   double value = (int)(a * 100 + .5);
   return (double)value / 100;
}
