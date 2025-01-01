#include "ReServer.h"
#include "ReNMEA2000_Re.h"

ReServer::ReServer()
{
   m_type = "tcp";
   m_port = 7050;
   m_filter_type = "black";
   m_filter_value = "";
   m_fmt = ReOutputFormat::OUT_NMEA0183;
   m_enabled = false;
   m_client_connected = (m_type == "tcp" ? false : true);      // for 'udp' type there is a broadcast so always true
   
   m_converter = ReN2kConverterCreator::create(m_fmt);
}

ReServer::ReServer(std::string name, std::string type, std::string filtertype, std::string filtervalue, 
   int port, ReOutputFormat fmt, bool enabled)
{
   m_name = name;
   m_type = type;
   m_filter_type = filtertype;
   m_filter_value = filtervalue;
   m_port = port;
   m_fmt = fmt;
   m_enabled = enabled;
   m_client_connected = (m_type == "tcp" ? false : true);      // for 'udp' type there is a broadcast so always true

   m_converter = ReN2kConverterCreator::create(m_fmt);
}

void ReServer::HandleMsg(const tN2kMsg& N2kMsg)
{
   // logger.debug(RE_TAG, "Message from %s, %s on port %d", m_name.c_str(), m_type.c_str(), m_port);

   // do not run handler where there are no clients for "tcp" type of server
   // for "udp" that is always set to true
   if (m_client_connected && filterMsgPGN(N2kMsg.PGN))
   {
      // If the message is not valid do not process with conversion and data sending
      if (!N2kMsg.IsValid())
      {
         logger.warn(RE_TAG, "N2K message has PGN=0 or data=0");
         return;
      }

      // for RAW formats we need much more space - exact number to be calculated in each converter based on the N2K message size
      // addtionally handling of assembling fast packet messages is necessary
      if (((ReOutputFormat::OUT_N2K_ASCII_RAW == m_fmt) || (ReOutputFormat::OUT_N2K_RAW == m_fmt)) && (NULL != GetNMEA2000()))
      {
         ReNMEA2000_Re* ptr = reinterpret_cast<ReNMEA2000_Re* >(GetNMEA2000());
         
         bool isFast = ptr->isFastPacketMsg(N2kMsg);
         m_converter->setFastMsgFlag(isFast);
      }

      // calculate max buffer size depending on the converter being used - calculation occurs inside the converter itself
      size_t maxBufSize = m_converter->getMaxBufSize(N2kMsg);

      std::unique_ptr<char[]> outputBuffer = std::make_unique<char[]>(maxBufSize);
      size_t size = 0;

      m_converter->convert(N2kMsg, outputBuffer.get(), size);
      sendData(outputBuffer.get(), size);
   }
}

bool ReServer::filterMsgPGN(const unsigned long& pgn)
{
   if (m_filter_type == "white")
   {
      return (std::string::npos != m_filter_value.find(std::to_string(pgn)));
   }
   else if (m_filter_type == "black")
   {
      return !(std::string::npos != m_filter_value.find(std::to_string(pgn)));
   }
   else
   {
      return true;
   }
}
