#include "ReConverterN2kAscii.h"

/*
Source: https://actisense.com/knowledge-base/nmea-2000/w2k-1-nmea-2000-to-wifi-gateway/nmea-2000-ascii-output-format/

Format:   Ahhmmss.ddd <SS><DD><P> <PPPPP> b0b1b2b3b4b5b6b7…..bn <CR><LF>

Example: 'A173321.107 23FF7 1F513 012F3070002F30709F \r\n'
                11   1  5  1  5          max 223       2

A173321.107
A = Message is an N2K (or J1939) data PGN
Time = 17:33:21.107

23FF7
23 = Message came from source address = 0x23
FF = Message went to destination address = 0xff (Global)
7 = Message was priority 7

1F513 – Decoded PGN number

012F3070002F30709F = data payload, 9 bytes
” ” – optional whitespace for example purposes – receiver must ignore.

<CR><LF> – end delimiters
*/

void ReConverterN2kAscii::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   char bt[5];

   getTimeFromN2k(n2kMsg);
   
   // Create time from GNSS time;
   time_t rawTime = (m_daysSince1970 * 3600 * 24) + m_secondsSinceMidnight;
   struct tm ts = *localtime(&rawTime);
   
   char time_str[13] = { 0, };
   strftime(time_str, sizeof(time_str), "A%H%M%S.000", &ts); // Create time string

   snprintf(output, 30, "%s %.2x%.2x%.1x %.5x ", time_str, n2kMsg.Source, n2kMsg.Destination, 
      n2kMsg.Priority, (unsigned int) n2kMsg.PGN); 

   for (size_t i = 0; i < n2kMsg.DataLen; i++)
   {
      snprintf(bt, 4, "%.2x", n2kMsg.Data[i]); // Add data fields
      strcat(output, bt);
   }

   strcat(output, "\r\n");
   size = strlen(output);

   // logger.debug(RE_TAG, "%s", output);
}

size_t ReConverterN2kAscii::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   size_t sentenceLength = 27 + n2kMsg.DataLen * 2;
   return (sentenceLength + BUF_RESERVE);                // max 27 bytes + 223 * 2 = 473 bytes 
}
