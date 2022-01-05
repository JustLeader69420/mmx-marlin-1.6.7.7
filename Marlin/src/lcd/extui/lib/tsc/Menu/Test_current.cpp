#include "../TSC_Menu.h"

#if ENABLED(NEW_BOARD)
MENUITEMS TestItems = {
// title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

#define GETTIME     20

static uint32_t motors_num = 0, bed_num = 0, hotend_num = 0;
static uint8_t ifFirst = 1;
static const GUI_RECT TestCurrentRect_0 = { (int16_t)24, (int16_t) 75, (int16_t)455, (int16_t)100 };
static const GUI_RECT TestCurrentRect_1 = { (int16_t)24, (int16_t)101, (int16_t)455, (int16_t)126 };
static const GUI_RECT TestCurrentRect_2 = { (int16_t)24, (int16_t)127, (int16_t)455, (int16_t)152 };
static const GUI_RECT TestCurrentRect_3 = { (int16_t)24, (int16_t)153, (int16_t)455, (int16_t)178 };

/********************************************************/
void redrawTestCurrent(void){
  char tempstr[100];
  uint16_t motors_A = 0, bed_A = 0, hotend_A = 0;

  GUI_SetColor(GANTRYLBL_COLOR);

  memset(tempstr, 0, 100);
  GUI_ClearPrect(&TestCurrentRect_0);
  // motors_v = (motors_num/GETTIME*3300)/(1024*21*4);
  sprintf_P(tempstr, "Motors ocp: %d", motors_num/GETTIME);
  GUI_DispStringInPrect(&TestCurrentRect_0,(uint8_t *)tempstr);

  memset(tempstr, 0, 100);
  GUI_ClearPrect(&TestCurrentRect_1);
  sprintf_P(tempstr, "Bed ocp: %d", bed_num/GETTIME);
  GUI_DispStringInPrect(&TestCurrentRect_1,(uint8_t *)tempstr);

  memset(tempstr, 0, 100);
  GUI_ClearPrect(&TestCurrentRect_2);
  // 计算加热头的电流(放大1000倍)
  hotend_A = ((hotend_num/GETTIME*330)*1000)/(1024*21*4); // 330-->3.3v放大100倍,1024-->ADC分辨率10bit,21-->放大倍数,4-->采样电阻0.04欧放大100倍
  sprintf_P(tempstr, "Hotend_ocp: %d, Hotend: %dmA", (hotend_num/GETTIME), hotend_A);
  GUI_DispStringInPrect(&TestCurrentRect_2,(uint8_t *)tempstr);
}
void Get_Current(void){
  motors_num = 0; bed_num = 0; hotend_num = 0;
  for(uint8_t i=0; i<GETTIME; i++){
    motors_num += Get_Adc(MOTORS_OCP);
    bed_num    += Get_Adc(BED_OCP);
    hotend_num += Get_Adc(HOTEND_OCP);
    delay(5);
  }
}
void menuCallBackTestC(void)
{
  static uint32_t nowTime_ms = 0;
  static uint32_t old_motors_num = motors_num, old_bed_num = bed_num, old_hotend_num = hotend_num;

  KEY_VALUES  key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:break;
  }
  
  if (millis() - nowTime_ms > 1000) { // Refresh per 1 sec
    nowTime_ms = millis();
    Get_Current();
    if( (old_motors_num != motors_num ||
         old_bed_num    != bed_num    ||
         old_hotend_num != hotend_num ))
    {
      old_motors_num = motors_num; old_bed_num = bed_num; old_hotend_num = hotend_num;
      redrawTestCurrent();
    }
  }
}

void menuTestC(void)
{
  #if ENABLED(NEW_BOARD)
    if(ifFirst == 1){
      ifFirst = 3;
      HAL_ANALOG_SELECT(MOTORS_OCP);
      HAL_ANALOG_SELECT(BED_OCP);
      HAL_ANALOG_SELECT(HOTEND_OCP);
      delay(250);
    }
  #endif
  menuDrawPage(&TestItems);
  Get_Current();
  redrawTestCurrent();
  menuSetFrontCallBack(menuCallBackTestC);
}

#endif

