#pragma once

#include "ReN2kConverter.h"

class ReConverterN2kSeaSmart : public ReN2kConverter
{
public: 
   void convert(const tN2kMsg& n2kMsg, char* output, size_t& size) override;
   size_t getMaxBufSize(const tN2kMsg& n2kMsg) override;
};