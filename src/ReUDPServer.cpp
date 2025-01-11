#include "ReUDPServer.h"

ReUDPServer::ReUDPServer(std::string name, std::string filtertype, std::string filtervalue, 
   int port, ReOutputFormat fmt, bool enabled) : ReServer(name, "udp", filtertype, filtervalue, port, fmt, enabled)
{
   m_server = new AsyncUDP();
}

void ReUDPServer::start()
{
   // m_server->listen((uint16_t) m_port);
   logger.debug(RE_TAG, "Start %s", m_name.c_str());
}

void ReUDPServer::sendData(char* outMsgBuf, size_t& size)
{
   if (size > 0)
   {
      // m_server->broadcast((unsigned char*) outMsgBuf, size);
      m_server->broadcastTo((unsigned char*) outMsgBuf, size, (uint16_t) m_port);

      m_stats.bytesSent += size;

      // logger.debug(RE_TAG, "%s", outMsgBuf);
   }
}

