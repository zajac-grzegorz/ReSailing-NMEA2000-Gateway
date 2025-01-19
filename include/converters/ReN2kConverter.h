#pragma once

#include <NMEA2000.h>

#include "ReBoatData.h"
#include "ReCommon.h"
#include "ReConfig.h"

#define Escape 0x10
#define StartOfText 0x02
#define EndOfText 0x03
#define MsgTypeN2k_NGT 0x93
#define MsgTypeN2k_RAW 0x95
#define MsgTypeN2k_BIN 0xD0

#define BUF_RESERVE 50

class ReN2kConverter
{
public:
   ReN2kConverter() {};
   void setFastMsgFlag(bool isFast) { m_isFastMsg = isFast; };

   virtual void convert(const tN2kMsg& n2kMsg, char* output, size_t& size) = 0;
   virtual size_t getMaxBufSize(const tN2kMsg& n2kMsg) = 0;

protected:
   void getTimeFromN2k(const tN2kMsg& n2kMsg);
   void handleGNSS(const tN2kMsg& N2kMsg);
   void handleSytemTime(const tN2kMsg& N2kMsg);
   void calculateWindData(ReBoatData& boatData, bool useCOG = false, bool useSOG = false);
   unsigned long n2ktoCanID(unsigned char prio, unsigned long pgn, unsigned long src, unsigned char dest);
   void addByteEscapedToBuf(unsigned char byteToAdd, size_t& idx, char* buf, int& byteSum);
   void printBuf(unsigned char len, const unsigned char* pData, bool AddLF);

   uint16_t m_daysSince1970 = 0;
   double m_secondsSinceMidnight = 0;
   bool m_isFastMsg = false;
};
