#include "ReConverterN2kRaw.h"

/*
Source: https://github.com/aldas/go-nmea-client/blob/main/actisense/binaryreader.go

Example Send: `cansend can0 18EAFFFE#00EE00`
Output from W2K RAW Actisense server: `95093eb7feffea1800ee0080`

Example: 1002 95 093eb7feffea1800ee0080 1003

Message format:
10 - Escape
02 - StartOfText

byte 0: MsgTypeN2k_RAW
byte 1: length of time counter + canid + data
byte 2,3: time/counter
byte 4,5,6,7: CanID (little endian)
byte 8 ... (N-1): data
byte N (last): CRC

10 - Escape
03 - EndOfText

Above example breakdown:
10 - Escape
02 - StartOfText

95 - MsgTypeN2k_RAW
09 - length of time counter + canid + data
3eb7 - time/counter
feffea18 - CanID (little endian - PGN = 59904 - ISO Request, prio = 6, src = 254)
00ee00 - 3 bytes data
80 - CRC

10 - Escape
03 - EndOfText

If any byte in the Size, Packet Data, or Checksum fields is equal to DLE, then a second DLE is inserted
immediately following the byte. This extra DLE is not included in the size or checksum calculation.
This procedure allows the DLE character to be used to delimit the boundaries of a packet.
*/

void ReConverterN2kRaw::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   unsigned long canId = n2ktoCanID(n2kMsg.Priority, n2kMsg.PGN, n2kMsg.Source, n2kMsg.Destination);

   // PGN validity - n2ktoCanID returns 0 for invalid PGN
   if (0 == canId)
   {
      return;
   }

   unsigned long msgTime = n2kMsg.MsgTime;
   
   if (n2kMsg.DataLen <= 8 && !m_isFastMsg)
   {
      // message will be one RAW frame
      addRawMessageToBuffer(msgTime, canId, n2kMsg.DataLen, n2kMsg.Data, output, size);

      // logger.debug(RE_TAG, "inside single");
      // printBuf(size, (unsigned char*) output, true);
   }
   else
   {
      // message will be fast packet frames
      unsigned char temp[8]; // {0,0,0,0,0,0,0,0};
      int cur = 0;
      int frames = (n2kMsg.DataLen > 6 ? (n2kMsg.DataLen - 6 - 1) / 7 + 1 + 1 : 1);
      int order = 1 << 5;
      
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
         addRawMessageToBuffer(msgTime, canId, 8, temp, output, size);

         // logger.debug(RE_TAG, "inside multi");
         // printBuf(size, (unsigned char*) output, true);
      }
   
      // clear fast packet flag
      m_isFastMsg = false;
   }
}

size_t ReConverterN2kRaw::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   size_t sentenceLength = 0;
   
   if (n2kMsg.DataLen <= 8 && !m_isFastMsg)
   {
      // message will be one RAW frame
      sentenceLength = 10 + n2kMsg.DataLen + 1 + 2;
   }
   else
   {
      // message will be fast packet frames
      int frames = (n2kMsg.DataLen > 6 ? (n2kMsg.DataLen - 6 - 1) / 7 + 1 + 1 : 1);

      // max output buffer size for 32 frames (32 * (10 + 8 + 3)) = 672
      sentenceLength = frames * 21;
   }

   // max output buffer size for 32 frames (32 * (10 + 8 + 3)) = 672
   return (sentenceLength + BUF_RESERVE);
}

void ReConverterN2kRaw::addRawMessageToBuffer(unsigned long msgTime, unsigned long canId, unsigned char msgLen, 
   const unsigned char* msgBuf, char* output, size_t& size)
{
   size_t msgIdx = size;
   int byteSum = 0;
   uint8_t checkSum = 0;

   output[msgIdx++] = Escape;
   output[msgIdx++] = StartOfText;
   addByteEscapedToBuf(MsgTypeN2k_RAW, msgIdx, output, byteSum);  // command identifier
   addByteEscapedToBuf(2 + 4 + msgLen, msgIdx, output, byteSum);  // length = time/counter(2) + canId(4) + msglen 
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);  // only 2 bytes of msgTime are useful here
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);    // canId 
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   
   for (int i = 0; i < msgLen; i++)
   {
      addByteEscapedToBuf(msgBuf[i], msgIdx, output, byteSum);
   }
   byteSum %= 256;

   checkSum = (uint8_t)((byteSum == 0) ? 0 : (256 - byteSum));
   output[msgIdx++] = checkSum;
   
   if (checkSum == Escape)
   {
      output[msgIdx++] = checkSum;
   }

   output[msgIdx++] = Escape;
   output[msgIdx++] = EndOfText;

   size = msgIdx;
}
