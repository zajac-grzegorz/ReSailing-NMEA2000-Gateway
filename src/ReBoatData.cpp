#include <N2kMessages.h>

#include "ReBoatData.h"

double ReBoatData::Heading = 0;
double ReBoatData::Latitude = 0;
double ReBoatData::Longitude = 0;
double ReBoatData::SOG = 0;
double ReBoatData::COG = 0;
double ReBoatData::STW = 0;
double ReBoatData::AWS = 0;
double ReBoatData::TWS = 0;
double ReBoatData::MaxAws = 0;
double ReBoatData::MaxTws = 0;
double ReBoatData::AWA = 0;
double ReBoatData::TWA = 0;
double ReBoatData::TWD = 0;
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
   
   doc["Heading"] = round(RadToDeg(Heading));
   doc["Latitude"] = Latitude;
   doc["Longitude"] = Longitude;
   doc["SOG"] = roundDouble(msToKnots(SOG));
   doc["COG"] = round(RadToDeg(COG));
   doc["STW"] = roundDouble(msToKnots(STW));
   doc["AWS"] = roundDouble(msToKnots(AWS));
   doc["TWS"] = roundDouble(msToKnots(TWS));
   doc["MaxAws"] = roundDouble(msToKnots(MaxAws));
   doc["MaxTws"] = roundDouble(msToKnots(MaxTws));
   doc["AWA"] = round(RadToDeg(AWA));
   doc["TWA"] = round(RadToDeg(TWA));
   doc["TWD"] = round(RadToDeg(TWD));
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