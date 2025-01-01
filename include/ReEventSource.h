#pragma once

#include <AsyncEventSource.h>
#include <N2kTimer.h>

#include "ReBoatData.h"

class ReEventSource : public AsyncEventSource
{
public:
    ReEventSource(const char* url, unsigned long interval=1000);
    void setUpdateInterval(unsigned long interval) { mUpdatePeriod = interval; };
    void update();

private:
    
    void handleConnect(AsyncEventSourceClient* client);
    void handleDisconnect(AsyncEventSourceClient* client);

    ReBoatData mBoatData;
    tN2kScheduler mNextUpdate;
    unsigned long mUpdatePeriod;
    int mNumClients = 0;

    CorsMiddleware mCors;
};
