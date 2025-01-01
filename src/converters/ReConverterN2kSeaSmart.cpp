#include "ReConverterN2kSeaSmart.h"

#include <Seasmart.h>

#define MAX_NMEA2000_MESSAGE_SEASMART_SIZE 500

/*
Source: https://www.seasmart.net/pdf/SeaSmart_HTTP_Protocol_RevG_043012.pdf

The SeaSmart.Net Protocol is an 7-bit ASCII based format to retain compatibility with all types of Web
Browsers. Each received PGN instance is converted in to a sequence of comma separated fields and
terminated with the standard NMEA 0183 “*” character and two character (1 byte) checksum.
Each field is a fixed length with a variable number of fields depending on the type of data.
The Protocol header starts with a “$” symbol (0x24 HEX) followed by the letters “PCDIN” then the
specified six digit NMEA 2000 PGN number. This PGN number corresponds to the type of data to follow
as well as the number of parameters.

Example: $PCDIN,01F211,0B9CF01B,03,008061480D0000FF*5C '\r\n' '\0' 
            6  1   6  1    8   1 2 1     max 223   1 2    2     1
*/

void ReConverterN2kSeaSmart::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   // If the buffer is long enough, this function returns the number of bytes
   // written including the terminating \0 (but this function does not add the
   // NMEA separator \\r\\n).
   size = N2kToSeasmart(n2kMsg, millis(), output, MAX_NMEA2000_MESSAGE_SEASMART_SIZE);
   
   // If the buffer is not long enough, this function returns 0 and does not do anything.
   if (0 == size) 
   {
      return;
   }

   // Add the NMEA separator \\r\\n and update size value
   output[size++] = 0x0d;
   output[size++] = 0x0a;

   // logger.debug(RE_TAG, "(%d) %s", size, output);
}

size_t ReConverterN2kSeaSmart::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   // size_t pcdinSentenceLength = 6 + 1 + 6 + 1 + 8 + 1 + 2 + 1 + n2kMsg.DataLen * 2 + 1 + 2 + 2 + 1;   // 32 + msg data len (223 * 2) = 478 bytes max
   size_t pcdinSentenceLength = 32 + n2kMsg.DataLen * 2;
   return (pcdinSentenceLength + BUF_RESERVE);                                                           // max 478 + reserve (50) = 528 bytes
}
