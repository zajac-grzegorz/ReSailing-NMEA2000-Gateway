#pragma once

#include <memory>

#include "ReConfig.h"
#include "ReConverterN2kAscii.h"
#include "ReConverterN2kAsciiRaw.h"
#include "ReConverterN2kBin.h"
#include "ReConverterN2kJson.h"
#include "ReConverterN2kNgt.h"
#include "ReConverterN2kNmea0183.h"
#include "ReConverterN2kRaw.h"
#include "ReConverterN2kSeaSmart.h"
#include "ReN2kConverter.h"

class ReN2kConverterCreator
{
public:
   static std::unique_ptr<ReN2kConverter> create(ReOutputFormat fmt)
   {
      if (fmt == ReOutputFormat::OUT_N2K_NGT) return std::make_unique<ReConverterN2kNgt>();
      if (fmt == ReOutputFormat::OUT_N2K_ASCII_RAW) return std::make_unique<ReConverterN2kAsciiRaw>();
      if (fmt == ReOutputFormat::OUT_N2K_YD) return std::make_unique<ReConverterN2kAsciiRaw>();
      if (fmt == ReOutputFormat::OUT_N2K_ASCII) return std::make_unique<ReConverterN2kAscii>();
      if (fmt == ReOutputFormat::OUT_N2K_BIN) return std::make_unique<ReConverterN2kBin>();
      if (fmt == ReOutputFormat::OUT_N2K_RAW) return std::make_unique<ReConverterN2kRaw>();
      if (fmt == ReOutputFormat::OUT_NMEA0183) return std::make_unique<ReConverterN2kNmea0183>();
      if (fmt == ReOutputFormat::OUT_JSON) return std::make_unique<ReConverterN2kJson>();
      if (fmt == ReOutputFormat::OUT_N2K_SEASMART) return std::make_unique<ReConverterN2kSeaSmart>();

      throw std::runtime_error("Unknown Format");
   }
};