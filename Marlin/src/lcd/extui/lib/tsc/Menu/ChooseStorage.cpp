#include "../TSC_Menu.h"
#include "Print.h"
#include "PrintUdisk.h"

//1title, ITEM_PER_PAGE item
const MENUITEMS chooseStorageItems = {
// title
LABEL_CUSTOM_2,
// icon                 label
 {{ICON_SD_SOURCE,      LABEL_TFTSD},
  {ICON_UDISK,          LABEL_U_DISK},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACKGROUND,     LABEL_BACKGROUND},
  {ICON_BACK,           LABEL_BACK},}
};

void menuCallBackChooseStorage() {
    KEY_VALUES key_num = menuKeyGetValue();
    switch(key_num)
    {
        case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuPrint;         break;
        case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuPrintUdisk;    break;      

        case KEY_ICON_7: infoMenu.cur--;                                    break;
        default: break;
    }
}

void menuChooseStorage()
{
    menuDrawPage(&chooseStorageItems);
    menuSetFrontCallBack(menuCallBackChooseStorage);
}