#include "ReConverterN2kBin.h"

/*
Source: https://actisense.com/knowledge-base/nmea-2000/w2k-1-nmea-2000-to-wifi-gateway/nmea-2000-actisense-output-format/
Source: https://github.com/aldas/go-nmea-client/tree/main/actisense
Source: https://github.com/canboat/canboatjs/blob/master/lib/n2k-actisense.js

Format: <ID><LL><D><S><PDUS><PDUF><DPP><C><TTTT><MHS>b0b1b2b3b4b5b6b7…bn

<ID> ID - BST Message ID
<LL> Length - Two bytes for payload length, the maximum length of an N2K message is 1785.
<D> Destination Address - Location for storing the address of the device receiving the message.
<S> Source Address - Location for storing the address of the device sending the message.
<PDUS> PDU Specific - Lowest byte of PGN, depending on PDUF this will contain an address (PDU1) or a Group Extension (PDU2).
<PDUF> PDU Format - PDU Format determines contents of PDUS.
<DPP> Data page and priority - Data Page and message Priority bits.
<C> Control - PGN control ID bits and 3-bit Fast-Packet sequence ID.
<TTTT> Timestamp - Four bytes for timestamp in milliseconds.
<MHS> Message header size - Size of message header, includes all bytes up to b0.
(b0…bn) Message data - Message’s payload of data.

Example: 1002 d0 1500 ff 23 02fd09 00740c1a 00 34ed013f26faffff d8 1003


10 - Escape
02 - StartOfText

d0 - MsgTypeN2k_BIN

1500 - total length = 0x15 = 21 decimal
ff - destination
23 - source
02fd09 - PDUS + PDUF + DPP + C
00740c1a - timestamp
00 - MHS
34ed013f26faffff - data payload
d8 - checksum? - not clear / missing in the Actisense documentation

10 - Escape
03 - EndOfText

If any byte in the Size, Packet Data, or Checksum fields is equal to DLE, then a second DLE is inserted
immediately following the byte. This extra DLE is not included in the size or checksum calculation.
This procedure allows the DLE character to be used to delimit the boundaries of a packet.
*/

void ReConverterN2kBin::convert(const tN2kMsg& n2kMsg, char* output, size_t& size)
{
   unsigned long msgTime = n2kMsg.MsgTime;
   unsigned long canId = n2ktoCanID(n2kMsg.Priority, n2kMsg.PGN, n2kMsg.Source, n2kMsg.Destination);

   size_t msgIdx = 0;
   int byteSum = 0;
   uint8_t checkSum = 0;
   uint8_t mhs = 13;
   uint16_t length = n2kMsg.DataLen + mhs;

   output[msgIdx++] = Escape;
   output[msgIdx++] = StartOfText;
   addByteEscapedToBuf(MsgTypeN2k_BIN, msgIdx, output, byteSum);
   // length
   addByteEscapedToBuf(length & 0xff, msgIdx, output, byteSum); // length does not include escaped chars
   length >>= 8;
   addByteEscapedToBuf(length & 0xff, msgIdx, output, byteSum); // length does not include escaped chars
   // destination
   addByteEscapedToBuf(n2kMsg.Destination, msgIdx, output, byteSum);
   // can ID 4 bytes
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   canId >>= 8;
   addByteEscapedToBuf(canId & 0xff, msgIdx, output, byteSum);
   // Timestamp
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   msgTime >>= 8;
   addByteEscapedToBuf(msgTime & 0xff, msgIdx, output, byteSum);
   // MHS
   addByteEscapedToBuf(mhs, msgIdx, output, byteSum);
   // Data payload
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

   // logger.debug(RE_TAG, "Actisense N2K message");
   // printBuf(size, (unsigned char*) output, true);
}

size_t ReConverterN2kBin::getMaxBufSize(const tN2kMsg& n2kMsg)
{
   // size_t sentenceLength = 2 + 1 + 12 + n2kMsg.DataLen + 1 + 1 + 1; 
   size_t sentenceLength = 18 + n2kMsg.DataLen * 1.5;

   // 18 + msg data len 223 (but some bytes may be 0x10, so additional Escape character has to 
   // be inserted - addByteEscapedToBuf()) = 241 bytes
   // therefore add a reserve of about 50% + normal reserve (50 bytes)
   return (sentenceLength + BUF_RESERVE);                               // max 360 + reserve (50) = 410 bytes
}
