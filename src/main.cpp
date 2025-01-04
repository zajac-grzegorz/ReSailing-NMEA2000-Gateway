#include <esp_wifi.h>
#include <nvs_flash.h>

#include <Arduino.h>
#include <AsyncTCP.h>
#include <AsyncUDP.h>
#include <ESPAsyncWebServer.h>
#include <ESPUI.h>
#include <MycilaWebSerial.h>

#include "ReNMEA2000_Re.h"
#include <N2kMessages.h>

#include "ReBoatData.h"
#include "ReCommon.h"
#include "ReConfig.h"
#include "ReN2k.h"
#include "ReServer.h"
#include "ReTCPServer.h"
#include "ReUDPServer.h"
#include "ReUI.h"
#include "ReUtils.h"
#include "ReEventSource.h"
#include "ReWifi.h"

// Constuct the most important object of this application :)
tNMEA2000 &NMEA2000=*(new ReNMEA2000_Re());

// Replace this with the instance from ESPUI
AsyncWebServer* server = nullptr;

// Custom web socket
ReEventSource reEvents("/re");

// Max 3 servers for stable performance
std::vector<ReServer*> reOutputServers;

// UI wrapper object
ReUI reUI(reOutputServers);

// Setup N2k engine
ReN2k n2k(NMEA2000); 

// Stream for Actisense input data i.e. from NMEA_Simulator application
N2kStream* readStream = nullptr;
// Stream for Actisense output data i.e. to Actisense NMEA Reader application
N2kStream* forwardStream = nullptr;

// To allow log viewing over the web
WebSerial webSerial;

void configureWebSerial(bool enabled, AsyncWebServer* server)
{
   if (enabled)
   {
      if (nullptr != server)
      {
         logger.forwardTo(&webSerial);
         webSerial.setBuffer(256);
         webSerial.begin(server);

         logger.debug(RE_TAG, "Using WebSerial logger");
      }
      else
      {
         logger.error(RE_TAG, "AsyncWebServer not initialized in ESPUI");
      }
   }
}

void createOutputServers()
{
   std::vector<const char*> enabledKeys = { key_ser1_enable, key_ser2_enable, key_ser3_enable };
   std::vector<const char*> nameKeys = { key_ser1_name, key_ser2_name, key_ser3_name };
   std::vector<const char*> portKeys = { key_ser1_port, key_ser2_port, key_ser3_port };
   std::vector<const char*> formatKeys = { key_ser1_format, key_ser2_format, key_ser3_format };
   std::vector<const char*> typeKeys = { key_ser1_type, key_ser2_type, key_ser3_type };
   std::vector<const char*> filterTypeKeys = { key_ser1_fil_type, key_ser2_fil_type, key_ser3_fil_type };
   std::vector<const char*> filterValueKeys = { key_ser1_fil_val, key_ser2_fil_val, key_ser3_fil_val };
   int cnt = 0;

   for (auto key : enabledKeys)
   {
      bool isEnabled = config.getBool(key);

      if (isEnabled)
      {
         std::string name = config.getString(nameKeys.at(cnt));
         std::string type = config.getString(typeKeys.at(cnt));
         std::string filtertype = config.getString(filterTypeKeys.at(cnt));
         std::string filtervalue = config.getString(filterValueKeys.at(cnt));
         int port = config.getInt(portKeys.at(cnt));
         std::string outF = config.getString(formatKeys.at(cnt));
         ReOutputFormat outFmt = getOutputFormatFromString(outF);

         ReServer* srv = nullptr;
         
         if (type == "tcp")
         {
            srv = new ReTCPServer(name, filtertype, filtervalue, port, outFmt, isEnabled);
         }
         else
         {
            srv = new ReUDPServer(name, filtertype, filtervalue, port, outFmt, isEnabled);
         }

         reOutputServers.push_back(srv);
         srv->start();

         // Attach each server to NMEA2000 message handlers list
         n2k.attachMessageHandler(srv);
      }
      
      cnt++;
   }
}

void HandleStreamN2kMsg(const tN2kMsg& N2kMsg) 
{
// In dev test, data from USB Serial (i.e. from NMEA_Simulator) is converted to selected format on the TCP or UDP server 
#ifdef RE_N2K_SIMULATOR_INPUT_FOR_DEV_TEST

   for (auto srv : reOutputServers)
   {
      srv->HandleMsg(N2kMsg);
   }

#else 
   // By default data from USB Serial is converted to NMEA2000 message and send to the N2K bus
   NMEA2000.SendMsg(N2kMsg,-1);
#endif
}

void setup() 
{
   // Need to delay to allow proper logs after flashing and immediate reboot
   // delay(3000);
   
   // Init file system partition
   LittleFS.begin();

   // Init USB serial ports (depending on the flag in platformio.ini)
#ifdef RE_N2K_TERMINAL_PORT
   RE_N2K_TERMINAL_PORT.begin(115200);
   logger.forwardTo(&RE_N2K_TERMINAL_PORT);

   logger.debug(RE_TAG, "Using %s as N2K terminal serial", (RE_N2K_TERMINAL_PORT == Serial) ? "Serial" : "Serial1");
#endif

#ifdef RE_N2K_INPUT_PORT
   RE_N2K_INPUT_PORT.begin(115200);
   readStream = &RE_N2K_INPUT_PORT;

   logger.debug(RE_TAG, "Using %s as N2K input serial", (RE_N2K_INPUT_PORT == Serial) ? "Serial" : "Serial1");
#endif

#ifdef RE_N2K_OUTPUT_PORT
   RE_N2K_OUTPUT_PORT.begin(115200);
   forwardStream = &RE_N2K_OUTPUT_PORT;

   logger.debug(RE_TAG, "Using %s as N2K output serial", (RE_N2K_OUTPUT_PORT == Serial) ? "Serial" : "Serial1");
#endif

   // Load initial configuration data from JSON file - call at very beginning of setup()
   loadDefaultConfigDataFromJson();

   // Init WiFi connection
   initializeWifiConfiguration();

   // Build user interface with ESPUI library
   reUI.construct();

   // Start ESPUI engine to display UI on the web page
   ESPUI.begin(config.get(key_app_name));
   
   // This may only be called once the ESPAsyncWebServer is started - this happens in ESPUI.begin()
   server = ESPUI.WebServer();
   server->serveStatic("/", LittleFS, "/");
   server->onNotFound([](AsyncWebServerRequest* request) { request->redirect("/"); });

   // Set Server Sent Events handler
   reEvents.setUpdateInterval(config.getLong(key_sys_evt_int));
   server->addHandler(&reEvents);

   // If WebSerial enabled in JSON, configure it
   configureWebSerial(config.getBool("sys_webserial"), server);

   // Setup NMEA2000 library within ReN2k class
   n2k.init(forwardStream);

   // DO NOT change the place of calling below function!
   createOutputServers();

   // Start the NMEA2000 library
   n2k.open();

   // Read stream is configured so use it for Actisense input from i.e. NMEA_Simulator application
   n2k.setActisenseReader(readStream, HandleStreamN2kMsg);

   // Some delay to allow NMEA2000 initialize
   delay(200);

   logger.debug(RE_TAG, "NMEA2000 library up and running");
}

void loop() 
{
   // Never call "delay()" in the loop to not block async behavior of components
   n2k.update();
   reEvents.update();

   // Dummy to empty input buffer to avoid board to stuck with e.g. NMEA Reader
   // if (Serial.available())
   // {
   //    Serial.read();
   // }
}
