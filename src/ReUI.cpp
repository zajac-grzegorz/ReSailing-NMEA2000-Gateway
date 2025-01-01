#include "ReCommon.h"
#include "ReConfig.h"
#include "ReUI.h"

void constructUI()
{
   ESPUI.label("Status:", ControlColor::Turquoise, "<img src='app/re_logo.png' width='50' height='50'>");

   auto tab1 = ESPUI.addControl(ControlType::Tab, "General", "General");
   auto tab2 = ESPUI.addControl(ControlType::Tab, "Wifi", "Wifi");
   auto tab3 = ESPUI.addControl(ControlType::Tab, "Servers", "Servers");
   auto tab4 = ESPUI.addControl(ControlType::Tab, "Misc", "Misc");
   auto tab5 = ESPUI.addControl(ControlType::Tab, "About", "About");

   ESPUI.addControl(ControlType::Text, "Text Test:", "a Text Field", ControlColor::Alizarin, tab1);
   
   // ESPUI.begin(config.get(key_app_name));
}
