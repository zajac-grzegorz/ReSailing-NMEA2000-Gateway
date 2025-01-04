#pragma once

#include <NMEA2000.h>

#include "ReCommon.h"
#include "ReConfig.h"
#include "ReN2kConverterCreator.h"
#include "ReServerStatistics.h"

#define MaxActisenseMsgBuf 810

class ReServer : public tNMEA2000::tMsgHandler
{
   public:

      ReServer();
      ReServer(std::string name, std::string type, std::string filtertype, std::string filtervalue, 
         int port, ReOutputFormat outFmt, bool enabled = false);

      void HandleMsg(const tN2kMsg& N2kMsg);

      virtual void start() = 0;
      virtual void sendData(char* outMsgBuf, size_t& size) = 0;

      void setName(std::string name) { m_name = name; }
      void setType(std::string type) { m_type = type; }
      void setFilterType(std::string filtertype) { m_filter_type = filtertype; }
      void setFilterValue(std::string filtervalue) { m_filter_value = filtervalue; }
      void setPort(int port) { m_port = port; }
      void setFmt(ReOutputFormat fmt) { m_fmt = fmt; }
      void setEnabled(bool enabled) { m_enabled = enabled; }

      std::string getName() { return m_name; }
      std::string getType() { return m_type; }
      std::string getFilterType() { return m_filter_type; }
      std::string getFilterValue() { return m_filter_value; }
      bool isEnabled() { return m_enabled; }
      int getPort() { return m_port; }
      ReOutputFormat getFmt() { return m_fmt; }
      ReServerStatistics getStatictics() { return m_stats; }

   protected:

      std::string m_name;
      std::string m_type;
      std::string m_filter_type;
      std::string m_filter_value;
      int m_port;
      ReOutputFormat m_fmt;
      bool m_enabled;
      bool m_client_connected;
      ReServerStatistics m_stats;
      std::unique_ptr<ReN2kConverter> m_converter;

   private:
      bool filterMsgPGN(const unsigned long& pgn);
};
