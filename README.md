# ReSailing-NMEA2000-Gateway
## Multi-format NMEA2000 WiFi Gateway for ESP32.

> [!IMPORTANT]
> **This is work in progress - more detailed instruction to come**

**Provides output data in most popular formats:**
* Actisense NGT
* Actisense Bin
* Actisense RAW
* Actisense ASCII
* Yacht Devices ASCII RAW
* SeaSmart
* NMEA0183
* custom defined JSON

**Major features:**
* Data is available on TCP and/or UDP asynchronous servers (up to 3 servers can be configured)
* All formats except JSON tested with **OpenCPN**.
* Filters based on white- and black- lists (PGN filtering)
* Configuration through JSON files
* EventSource endpoint where boat data in JSON is published
* Sample wind instruments through http://re.local/app/index.html
* Web GUI can be easily added, ESPUI framework is integrated
* Option to build as Actisense listener/sender - to work with [NMEA_Simulator](https://www.kave.fi/Apps/NMEA-Simulator/NMEA-Simulator.7z)

### Acknowledgement
That project was possible due to magnificent work done by Authors of the following libraries:
* https://github.com/ttlappalainen/NMEA2000
* https://github.com/ttlappalainen/NMEA0183
* https://github.com/mathieucarbou/ESPAsyncWebServer
* https://github.com/mathieucarbou/AsyncTCP
* https://github.com/mathieucarbou/MycilaWebSerial
* https://github.com/mathieucarbou/MycilaConfig
* https://github.com/mathieucarbou/MycilaLogger
* https://github.com/s00500/ESPUI
* https://github.com/konvajs/konva
* https://github.com/canboat/canboatjs
* https://github.com/canboat/canboat
* https://github.com/aldas/go-nmea-client
* https://github.com/OpenCPN/OpenCPN
