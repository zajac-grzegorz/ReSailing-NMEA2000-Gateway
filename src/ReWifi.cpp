#include <ESPmDNS.h>
#include <WiFi.h>

#include "ReCommon.h"
#include "ReConfig.h"
#include "ReWifi.h"

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
   switch (event)
   {
      case ARDUINO_EVENT_WIFI_STA_START:
         logger.debug(RE_TAG, "(ev) STA started");
         break;
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
         logger.debug(RE_TAG, "(ev) STA connected to external WiFi");
         break;
      case ARDUINO_EVENT_WIFI_AP_START:
         logger.debug(RE_TAG, "(ev) AP started");
         break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
         logger.debug(RE_TAG, "(ev) STA IP: %s", WiFi.localIP().toString().c_str());
         break;
      case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
         logger.debug(RE_TAG, "(ev) Client connected to AP");
         break;
      case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
         logger.debug(RE_TAG, "(ev) Client disconnected from AP");
         break;
      case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
         logger.debug(RE_TAG, "(ev) Client IP assigned: %s", IPAddress(info.wifi_ap_staipassigned.ip.addr).toString().c_str());
         break;
      default:
         break;
   }
}

void initializeWifiConfiguration()
{
   // Init WiFi connection
   logger.debug(RE_TAG, "Starting WLAN");

   // Use both STA and AP modes
   WiFi.mode(WIFI_MODE_APSTA);
   WiFi.onEvent(onWiFiEvent);

   // In "sender" mode do not connect to the existing Wifi network
#ifndef RE_BUILD_AS_SENDER
   // Start STA - and try to connect with external WiFi network
   WiFi.begin(config.get(key_sta_ssid), config.get(key_sta_pass));
   WiFi.setHostname(config.get(key_app_hostname));

   uint8_t tries = 0;

   while ((WiFi.status() != WL_CONNECTED) && (tries < connectionTries))
   {
      delay(1000);
      tries++;
      logger.debug(RE_TAG, "Connecting to WiFi as STA, attempt %d", tries);

      if (tries == connectionTries)
      {
         logger.warn(RE_TAG, "Could not connect as STA");
         WiFi.disconnect();
         WiFi.setAutoReconnect(false); 
      }
   }
#endif

   // Static IP address for AP mode
   IPAddress apIp(config.get(key_ap_ip));
   IPAddress apSubnet(config.get(key_ap_subnet));

   // Start Access Point
   WiFi.softAP(config.get(key_ap_ssid), config.get(key_ap_pass), 9);
   WiFi.softAPConfig(apIp, apIp, apSubnet);

   // Run mDNS service
   if (!MDNS.begin(config.get(key_dns_name)))
   {
      logger.warn(RE_TAG, "Error starting mDNS");
   }
}