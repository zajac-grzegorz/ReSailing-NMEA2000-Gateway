#pragma once

#include "ReN2kConverter.h"

class ReConverterN2kAsciiRaw : public ReN2kConverter
{
public: 
   void convert(const tN2kMsg& n2kMsg, char* output, size_t& size) override;
   size_t getMaxBufSize(const tN2kMsg& n2kMsg) override;

private:
   void addAsciiRawMessageToBuffer(char* timeStr, unsigned long canId, unsigned char msgLen, 
      const unsigned char* msgBuf, char* output, size_t& size);
};