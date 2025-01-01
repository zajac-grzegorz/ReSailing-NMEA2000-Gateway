#pragma once

#include <MycilaConfig.h>
#include <MycilaLogger.h>

#define RELINE(line) #line
#define RETAG(a, b) a ":" RELINE(b)
#define RE_TAG RETAG(__FILE__, __LINE__)

inline Mycila::Config config;
inline Mycila::Logger logger;