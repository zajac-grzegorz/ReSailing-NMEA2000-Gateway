#include <esp_wifi.h>
#include <esp_mac.h>
#include <nvs_flash.h>
#include <sstream>
#include <string>

#include <ArduinoJson.h>

#include "ReCommon.h"
#include "ReConfig.h"
#include "ReUtils.h"

// Reset WiFi settings in the flash storage
void resetWiFiSettingsOnNvs(bool flag)
{
   if (flag)
   {
      int err = esp_wifi_restore();
      logger.warn(RE_TAG, "ESP Wifi on NVS restored: %d", err);
   }
}

// Reset whole NVS data
void resetNvsData()
{
   nvs_flash_erase();
   nvs_flash_init();

   logger.warn(RE_TAG, "NVS totally cleaned");
}

uint32_t getChipID()
{
   uint32_t chipId = 0;

   for (int i = 0; i < 17; i += 8)
   {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
   }

   return chipId;
}

std::string getChipIDStr()
{
   std::stringstream ss;
   ss << std::uppercase << std::hex << getChipID();
   return ss.str();
}

void loadDefaultConfigDataFromJson()
{
   // Start nvs preferences engine
   config.begin();

   JsonDocument doc;
   deserializeJson(doc, cfgInitDataStart);

   JsonObject root = doc.as<JsonObject>();

   for (JsonPair kv : root)
   {
      config.configure(kv.key().c_str(), kv.value().as<const char*>());
   }

   // If requested, clear the NVS memory
   if (config.getBool(key_sys_restore))
   {
      // Reset whole NVS data
      resetNvsData();

      // Clear reset request
      config.setBool(key_sys_restore, false);
   }

   printAllConfigKeys();
}

void printAllConfigKeys()
{
   for (auto& str : config.keys())
   {
      logger.info(RE_TAG, "%s = %s", str.c_str(), config.get(str.c_str()));
   }
}

ReOutputFormat getOutputFormatFromString(std::string& fmt)
{
   if (fmt == "n2k_ngt")
      return ReOutputFormat::OUT_N2K_NGT;
   else if (fmt == "n2k_raw")
      return ReOutputFormat::OUT_N2K_RAW;
   else if (fmt == "n2k_bin")
      return ReOutputFormat::OUT_N2K_BIN;
   else if (fmt == "n2k_ascii")
      return ReOutputFormat::OUT_N2K_ASCII;
   else if (fmt == "n2k_ascii_raw")
      return ReOutputFormat::OUT_N2K_ASCII_RAW;
   else if (fmt == "n2k_yd")
      return ReOutputFormat::OUT_N2K_YD;
   else if (fmt == "nmea0183")
      return ReOutputFormat::OUT_NMEA0183;
   else if (fmt == "json")
      return ReOutputFormat::OUT_JSON;
   else if (fmt == "seasmart")
      return ReOutputFormat::OUT_N2K_SEASMART;
   else
      return ReOutputFormat::OUT_UNKNOWN;
}

