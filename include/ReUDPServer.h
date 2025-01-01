#pragma once

#include <AsyncUDP.h>

#include "ReServer.h"

class ReUDPServer : public ReServer
{
   public:
      ReUDPServer(std::string name, std::string filtertype, std::string filtervalue, 
         int port, ReOutputFormat fmt, bool enabled = false);

      void start();
      void sendData(char* outMsgBuf, size_t& size);

   private: 
      AsyncUDP* m_server;
};