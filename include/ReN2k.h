#pragma once

#include <ActisenseReader.h>
#include <NMEA2000.h>

#include "ReConfig.h"


class ReN2k
{
    public:

        ReN2k(tNMEA2000& n2kObj);
        void init();
        void open();
        void update();
        void setActisenseReader(N2kStream* readStream, void (*msgHandler)(const tN2kMsg &n2kMsg)); 

        void attachMessageHandler(tNMEA2000::tMsgHandler* handler);
        void checkSourceAddressChange();

    private:

        tNMEA2000& n2k;
        int chipId;
        int nodeAddress;
        tActisenseReader actisenseReader;
        bool isReadStream;
        tN2kScheduler nextUpdateTime;
        const unsigned long updatePeriod = 60000 * 10; // 10 minutes
};