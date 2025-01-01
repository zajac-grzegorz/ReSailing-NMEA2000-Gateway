#include "ReConverterN2kNgt.h"

/*
Source: https://www.yachtd.com/downloads/ydnu02.pdf
APPENDIX F. Format of Messages in N2K Mode

Example: 1002 93130212f101ff80af3a0a090800fde3ff7f3005fd41 1003

10 - Escape
02 - StartOfText
93 - MsgTypeN2k_NGT
13 - total length (prio(1), pgn(3), dst(1), src(1), timestamp(4), msg data len(1))
02 - prio
12f101 - PGN (little indian - 0x1f112)
ff - dest
80 - src
af3a0a09 - timestamp
08 - payload length
00fde3ff7f3005fd - payload bytes
41 - checksum
10 - Escape
03 - EndOfText

If any byte in the Size, Packet Data, or Checksum fields is equal to DLE, then a second DLE is inserted
immediately following the byte. This extra DLE is not included in the size or checksum calculation.
This procedure allows the DLE character to be used to delimit the boundaries of a packet.
*/

void ReConverterN2kNgt::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   unsigned long pgn = n2kMsg.PGN;
   unsigned long msgTime = n2kMsg.MsgTime;
   size_t msgIdx = 0;
   int byteSum = 0;
   uint8_t checkSum = 0;

   output[msgIdx++] = Escape;
   output[msgIdx++] = StartOfText;
   addByteEscapedToBuf(MsgTypeN2k_NGT, msgIdx, output, byteSum);
   addByteEscapedToBuf(n2kMsg.DataLen + 11, msgIdx, output, byteSum); // length does not include escaped chars
   addByteEscapedToBuf(n2kMsg.Priority, msgIdx, output, byteSum);
   addByteEscapedToBuf(pgn & 0xff, msgIdx, output, byteSum);
   pgn >>= 8;
   addByteEscapedToBuf(pgn & 0xff, msgIdx, output, byteSum);
   pgn >>= 8;
   addByteEscapedToBuf(pgn & 0xff, msgIdx, output, byteSum);
   addByteEscapedToBuf(n2kMsg.Destination, msgIdx, output, byteSum);
   addByteEscapedToBuf(n2kMsg.Source, msgIdx, output, byteSum);
   // Time?
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   addByteEscapedToBuf(n2kMsg.DataLen, msgIdx, output, byteSum);

   for (int i = 0; i < n2kMsg.DataLen; i++)
   {
      addByteEscapedToBuf(n2kMsg.Data[i], msgIdx, output, byteSum);
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

size_t ReConverterN2kNgt::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   // size_t sentenceLength = 1 + 1 + 1 + 1 + 1 + 3 + 1 + 1 + 4 + 1 + n2kMsg.DataLen + 1 + 1 + 1; 
   size_t sentenceLength = 18 + n2kMsg.DataLen * 1.5;

   // 18 + msg data len 223 (but some bytes may be 0x10, so additional Escape character has to 
   // be inserted - addByteEscapedToBuf()) = 241 bytes
   // therefore add a reserve of about 50% + normal reserve (50 bytes)
   return (sentenceLength + BUF_RESERVE);                               // max 360 + reserve (50) = 410 bytes
}
