#include "ReCommon.h"
#include "ReConfig.h"
#include "ReUI.h"

ReUI::ReUI(const std::vector<ReServer*>& reServers) : mReServers(reServers)
{
}

void ReUI::construct()
{
   ESPUI.label("Status:", ControlColor::Turquoise, "<a href='/app/index.html'>instruments</a>");
   // ESPUI.label("Status:", ControlColor::Turquoise, "<img src='app/re_logo.png' width='50' height='50'>");

   auto tab1 = ESPUI.addControl(ControlType::Tab, "General", "General");
   auto tab2 = ESPUI.addControl(ControlType::Tab, "Wifi", "Wifi");
   auto tab3 = ESPUI.addControl(ControlType::Tab, "Servers", "Servers");
   auto tab4 = ESPUI.addControl(ControlType::Tab, "Misc", "Misc");
   auto tab5 = ESPUI.addControl(ControlType::Tab, "About", "About");

   // std::string bytesSent;  //= std::to_string(servers[0]->getStatictics().bytesSent);

   auto label = ESPUI.addControl(ControlType::Label, "Text Test:", "none", ControlColor::Alizarin, tab1);

   ESPUI.addControl(ControlType::Button, "Push Button", "Get Bytes Sent", ControlColor::Peterriver, label, 
      [label, this](Control* sender, int type) {
			if (type == B_UP) 
         {
            std::string bytesSent = std::to_string(mReServers[0]->getStatictics().bytesSent);
            ESPUI.updateLabel(label, String(bytesSent.c_str()));
			}
		});
}
