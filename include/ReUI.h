#pragma once

#include <ESPUI.h>
#include "ReServer.h"

class ReUI
{
public:
   ReUI(const std::vector<ReServer*>& reServers);
   void construct();

private:
   const std::vector<ReServer*>& mReServers;
};
