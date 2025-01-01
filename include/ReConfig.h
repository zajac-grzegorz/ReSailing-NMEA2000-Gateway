#pragma once

#include <IPAddress.h>
#include "ReStorageKeys.h"

// Embedded text data pointer
extern const uint8_t cfgInitDataStart[] asm("_binary_config_reInitData_json_start");

inline const char* reSoftwareVersionStr = "1.0.0 (2024-12-30)";

const uint8_t connectionTries = 10;       // how many tries to connect to STA wifi

// Set the information for other bus devices, which messages we support
const unsigned long transmitMessages[] PROGMEM = {0};
const unsigned long receiveMessages[] PROGMEM = 
{
   /*126992L,*/ // System time
   127250L,     // Heading
   127258L,     // Magnetic variation
   128259UL,    // Boat speed
   128267UL,    // Depth
   129025UL,    // Position
   129026L,     // COG and SOG
   129029L,     // GNSS
   130306L,     // Wind
   130310L,     // Environment (Water, Pressure)
   128275UL,    // Log
   127245UL,    // Rudder
   0
};

enum class ReOutputFormat
{
   OUT_UNKNOWN = 0,
   OUT_NMEA0183,              // NMEA0183 format
   OUT_JSON,                  // Custom JSON format
   OUT_N2K_NGT,               // N2KFormat_Actisense_NGT in OpenCPN
   OUT_N2K_RAW,               // N2KFormat_Actisense_RAW in OpenCPN
   OUT_N2K_BIN,               // N2KFormat_Actisense_N2K in OpenCPN
   OUT_N2K_ASCII,             // N2KFormat_Actisense_N2K_ASCII in OpenCPN
   OUT_N2K_ASCII_RAW,         // N2KFormat_Actisense_RAW_ASCII in OpenCPN
   OUT_N2K_YD,                // Yacht Devices - Byte compatible with Actisense ASCII RAW
   OUT_N2K_SEASMART           // SeaSmart format
};

// inline std::map<std::string, ReOutputFormat> fmtMap = {
//    { "nmea0183", ReOutputFormat::OUT_NMEA0183 },
//    { "json", ReOutputFormat::OUT_JSON },
//    { "n2k_ngt", ReOutputFormat::OUT_N2K_NGT },
//    { "n2k_raw", ReOutputFormat::OUT_N2K_RAW },
//    { "n2k_bin", ReOutputFormat::OUT_N2K_BIN },
//    { "n2k_ascii", ReOutputFormat::OUT_N2K_ASCII },
//    { "n2k_ascii_raw", ReOutputFormat::OUT_N2K_ASCII_RAW },
//    { "n2k_yd", ReOutputFormat::OUT_N2K_ASCII_RAW },
//    { "seasmart", ReOutputFormat::OUT_N2K_SEASMART }
// };