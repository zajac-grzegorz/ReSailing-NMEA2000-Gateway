#pragma once

#include "ReConfig.h"

void resetWiFiSettingsOnNvs(bool flag);
void resetNvsData();
void loadDefaultConfigDataFromJson();
void printAllConfigKeys();

ReOutputFormat getOutputFormatFromString(std::string& fmt);

uint32_t getChipID();
std::string getChipIDStr();