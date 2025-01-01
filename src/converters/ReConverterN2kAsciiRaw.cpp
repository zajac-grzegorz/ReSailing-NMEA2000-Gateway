#include "ReConverterN2kAsciiRaw.h"

/*
Source: https://www.yachtd.com/downloads/ydnu02.pdf
APPENDIX E. Format of Messages in RAW Mode

Messages sent from Device to PC have the following form:

hh:mm:ss.ddd D msgid b0 b1 b2 b3 b4 b5 b6 b7<CR><LF>

where:
• hh:mm:sss.ddd — time of message transmission or reception, ddd are milliseconds;
• D — direction of the message (‘R’ — from NMEA 2000 to PC, ‘T’ — from PC to NMEA 2000);
• msgid — 29-bit message identifier in hexadecimal format (contains NMEA 2000 PGN and other
fields);
• b0..b7 — message data bytes (from 1 to 8) in hexadecimal format;
• <CR><LF> — end of line symbols (carriage return and line feed, decimal 13 and 10).

Example:
17:33:21.107 R 19F51323 01 2F 30 70 00 2F 30 70

Example fast packet:
17:33:21.189 R 1DEFFF00 A0 0B E5 98 F1 08 02 02 
17:33:21.190 R 1DEFFF00 A1 00 DF 83 00 00

Example: 
17:33:21.219 R 15FD0734 FF 02 2B 75 A9 1A FF FF '\r\n' '\0'
     12      3     8             max 8            2      1

Time of message is UTC time if the Device has received the time from the NMEA 2000 network, otherwise
it is the time from Device start. 
*/

void ReConverterN2kAsciiRaw::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   unsigned long canId = n2ktoCanID(n2kMsg.Priority, n2kMsg.PGN, n2kMsg.Source, n2kMsg.Destination);

   // PGN validity - n2ktoCanID returns 0 for invalid PGN
   if (0 == canId)
   {
      return;
   }

   // prepare time string for the ASCII RAW format
   getTimeFromN2k(n2kMsg);

   // Create time from GNSS time;
   time_t rawTime = (m_daysSince1970 * 3600 * 24) + m_secondsSinceMidnight;
   struct tm ts = *localtime(&rawTime);
   
   char time_str[13] = { 0, };
   strftime(time_str, sizeof(time_str), "%T.000", &ts); // Create time string

   if (n2kMsg.DataLen <= 8 && !m_isFastMsg)
   {
      // logger.debug(RE_TAG, "inside single");
      // printBuf(n2kMsg.DataLen, n2kMsg.Data, true);
    
      // message will be one RAW frame
      addAsciiRawMessageToBuffer(time_str, canId, n2kMsg.DataLen, n2kMsg.Data, output, size);
   }
   else
   {
      // message will be fast packet frames
      unsigned char temp[8]; // {0,0,0,0,0,0,0,0};
      int cur = 0;
      int frames = (n2kMsg.DataLen > 6 ? (n2kMsg.DataLen - 6 - 1) / 7 + 1 + 1 : 1);
      int order = 1 << 5;
      
      // logger.debug(RE_TAG, "inside multi");
      // printBuf(n2kMsg.DataLen, n2kMsg.Data, true);

      for (int i = 0; i < frames; i++)
      {
         temp[0] = i | order; // frame counter
         
         if (i == 0)
         {
            temp[1] = n2kMsg.DataLen; // total bytes in fast packet
            // send the first 6 bytes
            for (int j = 2; j < 8; j++)
            {
               temp[j] = n2kMsg.Data[cur];
               cur++;
            }
         }
         else
         {
            int j = 1;
            // send the next 7 data bytes
            for (; j < 8 && cur < n2kMsg.DataLen; j++)
            {
               temp[j] = n2kMsg.Data[cur];
               cur++;
            }
            for (; j < 8; j++)
            {
               temp[j] = 0xff;
            }
         }

         // temporary print directly to Serial
         // printBuf(8, temp, true);

         addAsciiRawMessageToBuffer(time_str, canId, 8, temp, output, size);
      }

      // clear fast packet flag
      m_isFastMsg = false;
   }
}

size_t ReConverterN2kAsciiRaw::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   // 3 characters per one data byte = 2 chars in hex + space
   // for 8 data bytes it is 24 characters in size
   size_t sentenceLength = 0;
   
   if (n2kMsg.DataLen <= 8 && !m_isFastMsg)
   {
      // message will be one RAW frame
      sentenceLength = 27 + n2kMsg.DataLen * 3;
   }
   else
   {
      // message will be fast packet frames
      int frames = (n2kMsg.DataLen > 6 ? (n2kMsg.DataLen - 6 - 1) / 7 + 1 + 1 : 1);

      // max output buffer size for 32 frames (32 * (24 + 8*3 + 2)) + 1 = 1601 - '1' at the end is for '\0'
      sentenceLength = frames * (26 + 24) + 1;
   }

   // max output buffer size for 32 frames (32 * (24 + 8*3 + 2)) + 1 = 1601
   return (sentenceLength + BUF_RESERVE);
}

void ReConverterN2kAsciiRaw::addAsciiRawMessageToBuffer(char* timeStr, unsigned long canId, unsigned char msgLen, 
   const unsigned char* msgBuf, char* output, size_t& size)
{
   char bt[4] = "";
   
   snprintf(output + size, 24, "%s R %.8x", timeStr, (unsigned int) canId); // Set time and canID
   size += strlen(output);

   for (size_t i = 0; i < msgLen; i++)
   {
      snprintf(bt, 4, " %.2x", msgBuf[i]); // Add data fields
      strcat(output, bt);
   }

   strcat(output, "\r\n");
   size = strlen(output);
}