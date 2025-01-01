#pragma once

#include "ReN2kConverter.h"

class ReConverterN2kRaw : public ReN2kConverter
{
public: 
   void convert(const tN2kMsg& n2kMsg, char* output, size_t& size) override;
   size_t getMaxBufSize(const tN2kMsg& n2kMsg) override;
   void addRawMessageToBuffer(unsigned long msgTime, unsigned long canId, unsigned char msgLen, 
      const unsigned char* msgBuf, char* output, size_t& size);
};