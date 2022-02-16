#include "../TSC_Menu.h"
//1title, ITEM_PER_PAGE items
MENUITEMS developerItems = {
// title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_START,                LABEL_BACKGROUND},
  {ICON_LEVELING,             LABEL_LEVELING},
  {ICON_RUNOUT,               LABEL_RUNOUT_ON},
  {ICON_UNLOAD,               LABEL_UNLOAD},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};
#define TEST_GCODE_NUM 9
static char Gcode_str[TEST_GCODE_NUM][24] = {
  "M104 S170",
  "M140 S50",
  "G28",
  "M302 P1",
  "M106 S255",
  "G1 X10 Y10 Z10",
  "G1 E10",
  "G1 E-10",
  "M302 P0",};
#define READ_FIL_NUM 5

void menuCallBackDeveloper(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  static uint16_t touch_num = IDLE_TOUCH;
  static uint8_t send_gcode_num = 0;
  static bool Fil_pin_flag = false;
  switch(key_num)
  {
    case KEY_ICON_0:
      touch_num = KEY_ICON_0;
      break;
    
    case KEY_ICON_1:
      storeCmd("G28\n");
      storeCmd("G29\n");
      break;

    case KEY_ICON_2:
      for(int read_num=0; read_num<READ_FIL_NUM; read_num++)
      {
        if(READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE)    // 断料检测开关触发（有料）
        {
          Fil_pin_flag = true;
        }else
        {
          Fil_pin_flag = false;
        }
      }
      if(Fil_pin_flag)
      {
        popupReminder((uint8_t  *)"Yes", (uint8_t  *)"Has filament runout");
      }else
      {
        popupReminder((uint8_t  *)"No", (uint8_t  *)"No filament runout");
      }
      break;

    case KEY_ICON_3:
      mustStoreCmd("M302 P1\n");
      mustStoreCmd("G1 E-5\n");
      mustStoreCmd("M302 P0\n");
      break;

    #if ENABLED(NEW_BOARD)
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuTestM;
        break;
      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuTestC;
        break;
      case KEY_ICON_6: mustStoreCmd("M240\n"); break;
    #endif

    case KEY_ICON_7:
      infoMenu.cur--; 
      break;
  }
  // 发送测试的G代码，注意测试G代码比较多，需要排队入队，防止丢失
  if (touch_num == KEY_ICON_0)
  {
    if(storeCmd(Gcode_str[send_gcode_num]))
    {
      send_gcode_num++;
    }
    if(send_gcode_num>=TEST_GCODE_NUM)
    {
      send_gcode_num = 0;
      touch_num = IDLE_TOUCH;
    }
  }
}
void menuDeveloper()
{
  menuDrawPage(&developerItems);
  menuSetFrontCallBack(&menuCallBackDeveloper);
}

