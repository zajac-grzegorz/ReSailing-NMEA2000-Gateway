#ifndef _ReBoatData_H_
#define _ReBoatData_H_

#include <ArduinoJson.h>

class ReBoatData
{
public:
   void toJSON();
   double roundDouble(double a);

   static double Heading;
   static double Latitude;
   static double Longitude;
   static double SOG;
   static double COG;
   static double STW;
   static double AWS;
   static double TWS;
   static double MaxAws;
   static double MaxTws;
   static double AWA;
   static double TWA;
   static double TWD;
   static uint32_t TripLog;
   static uint32_t Log;
   static double RudderPosition;
   static double WaterTemperature;
   static double WaterDepth;
   static double WaterDepthOffset;
   static double AtmPressure;
   static double Variation;
   static double Altitude;
   static double GPSTime; // Secs since midnight,
   static uint16_t DaysSince1970; // Days since 1970-01-01

   static JsonDocument doc;
};

#endif // _ReBoatData_H_