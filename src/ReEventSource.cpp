#include "ReCommon.h"
#include "ReEventSource.h"

ReEventSource::ReEventSource(const char* url, unsigned long interval) : AsyncEventSource(url), mNextUpdate(0), mUpdatePeriod(interval)
{
   mCors.setOrigin("*");
   mCors.setMethods("*");
   mCors.setHeaders("*");
   mCors.setAllowCredentials(false);
   mCors.setMaxAge(86400);

   this->addMiddleware(&mCors);

   this->onConnect(std::bind(&ReEventSource::handleConnect, this, std::placeholders::_1));
   this->onDisconnect(std::bind(&ReEventSource::handleDisconnect, this, std::placeholders::_1));
}

void ReEventSource::update()
{
   // data will be send to connected clients every 1 second...
   if ((mNumClients > 0) && (mNextUpdate.IsTime()))
   {
      mNextUpdate.FromNow(mUpdatePeriod);

      // check if there is already a valid JSON document
      if (mBoatData.doc.isNull())
      {
         mBoatData.toJSON();
      }

      std::string textData;
      serializeJson(mBoatData.doc, textData);
      this->send(textData.c_str(), "update_data", 0, 1000);

      // logger.debug(RE_TAG, "update() message sent");
   }
}

void ReEventSource::handleConnect(AsyncEventSourceClient* client)
{
   logger.debug(RE_TAG, "Client connected to events");

   std::string textInterval = std::to_string(mUpdatePeriod);
   client->send(textInterval.c_str(), "update_interval", 0, 1000);
   
   mNumClients++;
}

void ReEventSource::handleDisconnect(AsyncEventSourceClient* client)
{
   logger.debug(RE_TAG, "Client disconnected from events");
   mNumClients--;
}
