#pragma once

#include <AsyncTCP.h>

#include "ReServer.h"

class ReTCPServer : public ReServer
{
   public:
      ReTCPServer(std::string name, std::string filtertype, std::string filtervalue, 
         int port, ReOutputFormat fmt, bool enabled = false);

      void start();
      void sendData(char* outMsgBuf, size_t& size);

      std::vector<AsyncClient*> getClients() { return m_clients; };
   
   private: 
      void handleNewClient(void* arg, AsyncClient* client);
      void handleError(void* arg, AsyncClient *client, int8_t error);
      void handleData(void* arg, AsyncClient* client, void* data, size_t len);
      void handleDisconnect(void* arg, AsyncClient* client);
      void handleTimeOut(void* arg, AsyncClient* client, uint32_t time);

      AsyncServer* m_server = nullptr;
      std::vector<AsyncClient*> m_clients;
};