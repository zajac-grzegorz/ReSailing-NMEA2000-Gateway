#include "ReCommon.h"
#include "ReConfig.h"
#include "ReN2k.h"
#include "ReUtils.h"

ReN2k::ReN2k(tNMEA2000 &n2kObj) : n2k(n2kObj), nextUpdateTime(0)
{
   isReadStream = false;
}

void ReN2k::init()
{
   // Get node address stored in the preferences
   nodeAddress = config.getInt(key_n2k_node_addr);
   logger.debug(RE_TAG, "nodeAddress stored = %d", nodeAddress);
   
   chipId = getChipID();

   // Reserve enough buffer for sending all messages.
   n2k.SetN2kCANMsgBufSize(8);
   n2k.SetN2kCANReceiveFrameBufSize(250);
   n2k.SetN2kCANSendFrameBufSize(250);

#ifdef RE_BUILD_AS_SENDER
   // Set product information
   n2k.SetProductInformation("000100200300",           // Manufacturer's Model serial code
                             200,                      // Manufacturer's product code
                             config.get(key_app_name), // Manufacturer's Model ID
                             reSoftwareVersionStr,     // Manufacturer's Software version code
                             reSoftwareVersionStr      // Manufacturer's Model version
   );

   // Set device information
   n2k.SetDeviceInformation(chipId, // Unique number. Use e.g. Serial number.
                            130,    // Device function=NMEA 2000 to Analog Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                            25,     // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                            2046    // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
   );
#else
   // Set product information
   n2k.SetProductInformation("300200100000",           // Manufacturer's Model serial code
                             100,                      // Manufacturer's product code
                             config.get(key_app_name), // Manufacturer's Model ID
                             reSoftwareVersionStr,     // Manufacturer's Software version code
                             reSoftwareVersionStr      // Manufacturer's Model version
   );
   
   // Set device information
   n2k.SetDeviceInformation(chipId,  // Unique number. Use e.g. Serial number.
                            130, // Device function=NMEA 2000 to Analog Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                            25,  // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                            2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
   );
#endif

   n2k.SetMode(tNMEA2000::N2km_ListenAndNode, nodeAddress);
   n2k.ExtendTransmitMessages(transmitMessages);
   n2k.ExtendReceiveMessages(receiveMessages);

#ifdef RE_BUILD_AS_SENDER
   // Stream* ForwardStream=&Serial;
   // Serial.begin(115200);
   // n2k.SetForwardStream(ForwardStream);
#endif
}

void ReN2k::open()
{
   n2k.Open();
}

void ReN2k::update()
{
   n2k.ParseMessages();

   // check for address source change every 10 minutes
   if (nextUpdateTime.IsTime())
   {
      nextUpdateTime.FromNow(updatePeriod);
      checkSourceAddressChange();
   }

   if (isReadStream)
   {
      actisenseReader.ParseMessages();
   }
}

void ReN2k::setActisenseReader(N2kStream* readStream, void (*msgHandler)(const tN2kMsg &n2kMsg))
{
   actisenseReader.SetReadStream(readStream);
   actisenseReader.SetDefaultSource(75);
   actisenseReader.SetMsgHandler(msgHandler); 

   isReadStream = true;
}

void ReN2k::attachMessageHandler(tNMEA2000::tMsgHandler* handler)
{
   n2k.AttachMsgHandler(handler);
}

void ReN2k::checkSourceAddressChange()
{
   // if (sourceAddress != nodeAddress)
   if (n2k.ReadResetAddressChanged())
   {
      // Save potentially changed Source Address to NVS memory
      nodeAddress = n2k.GetN2kSource();
      config.set(key_n2k_node_addr, std::to_string(nodeAddress));
      
      logger.debug(RE_TAG, "Address change: new node address = %d", nodeAddress);
   }
}
