#include "ReTCPServer.h"

ReTCPServer::ReTCPServer(std::string name, std::string filtertype, std::string filtervalue, 
   int port, ReOutputFormat fmt, bool enabled) : ReServer(name, "tcp", filtertype, filtervalue, port, fmt, enabled)
{
   m_server = new AsyncServer(port);
}

void ReTCPServer::start()
{
   m_server->onClient(std::bind(&ReTCPServer::handleNewClient, this, std::placeholders::_1, std::placeholders::_2), m_server);
   m_server->begin();

   logger.debug(RE_TAG, "Start %s", m_name.c_str());
}

void ReTCPServer::sendData(char* outMsgBuf, size_t& size)
{
   for (auto client : m_clients)
   {
      // reply to client
      if (client->space() >= size + 10 && client->canSend())
      {
         client->write(outMsgBuf, size);

         // Data can be sent to multiple clients so count bytesSent total value
         m_stats.bytesSent += size;
      }
   }
}

/* server events */
void ReTCPServer::handleNewClient(void* arg, AsyncClient* client)
{
   logger.debug(RE_TAG, "%s add client (%p) ip: %s", m_name.c_str(), client, client->remoteIP().toString().c_str());

   // add to list
   m_clients.push_back(client);
   logger.debug(RE_TAG, "%s num clients %d", m_name.c_str(), m_clients.size());

   m_client_connected = (m_clients.size()) > 0 ? true : false;

   // register events
   client->onData(std::bind(&ReTCPServer::handleData, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), NULL);
   client->onError(std::bind(&ReTCPServer::handleError, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3), NULL);
   client->onTimeout(std::bind(&ReTCPServer::handleTimeOut, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3), NULL);
   client->onDisconnect(std::bind(&ReTCPServer::handleDisconnect, this, std::placeholders::_1,
      std::placeholders::_2), NULL);

   std::string info = m_name + " " + m_type + " on port " + std::to_string(m_port) + "\r\n";
   // reply to client
   if (client->space() > info.length() && client->canSend())
   {
      client->write(info.c_str(), info.length());
   }
}

/* clients events */
void ReTCPServer::handleError(void* arg, AsyncClient* client, int8_t error)
{
   logger.debug(RE_TAG, "%s, connection error %s from client %s", m_name.c_str(), client->errorToString(error),
      client->remoteIP().toString().c_str());
}

void ReTCPServer::handleData(void* arg, AsyncClient* client, void* data, size_t len)
{
   logger.debug(RE_TAG, "%s, data received from client %s", m_name.c_str(), client->remoteIP().toString().c_str());
}
void ReTCPServer::handleDisconnect(void* arg, AsyncClient* client)
{
   logger.debug(RE_TAG, "%s client (%p) disconnected", m_name.c_str(), client);
   m_clients.erase(std::find(m_clients.begin(), m_clients.end(), client));

   m_client_connected = (m_clients.size()) > 0 ? true : false;
}

void ReTCPServer::handleTimeOut(void* arg, AsyncClient* client, uint32_t time)
{
   logger.debug(RE_TAG, "%s, client ACK timeout ip: %s", m_name.c_str(), client->remoteIP().toString().c_str());
}
