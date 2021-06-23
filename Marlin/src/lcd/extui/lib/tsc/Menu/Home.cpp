#include "../TSC_Menu.h"

bool stop_home = false; // 停止复位的标志，注该标志影响到阻塞状态，慎用！！！记得及时关闭

//1title, ITEM_PER_PAGE item
const MENUITEMS homeItems = {
// title
LABEL_HOME,
// icon                       label
 {{ICON_HOME,                 LABEL_HOME},
  {ICON_X_HOME,               LABEL_X_HOME},
  {ICON_Y_HOME,               LABEL_Y_HOME},
  {ICON_Z_HOME,               LABEL_Z_HOME},
  {ICON_EM_STOP,              LABEL_EMERGENCYSTOP},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuCallBackHome(void)
{
  KEY_VALUES key_num = menuKeyGetValue();

  switch(key_num)
  {
    case KEY_ICON_0: stop_home = false; storeCmd(PSTR("G28"));   break;
    case KEY_ICON_1: stop_home = false; storeCmd(PSTR("G28 X")); break;
    case KEY_ICON_2: stop_home = false; storeCmd(PSTR("G28 Y")); break;
    case KEY_ICON_3: stop_home = false; storeCmd(PSTR("G28 Z")); break;
    case KEY_ICON_4: stop_home = true;  storeCmd(PSTR("M410")); storeCmd(PSTR("M18")); break; //M410快速停止电机，在这用于关闭定时；M18/M84，解锁电机
    case KEY_ICON_7: stop_home = false; infoMenu.cur--; break;
    default:break;            
  }
}

void menuHome()
{
  menuDrawPage(&homeItems);
  menuSetFrontCallBack(menuCallBackHome);
}
