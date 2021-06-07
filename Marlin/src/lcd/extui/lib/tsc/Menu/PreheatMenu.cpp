#include "PreheatMenu.h"
#include "../TSC_Menu.h"


MENUITEMS preheatItems = {
// title
LABEL_PREHEAT,
// icon                       label
  {
    {ICON_PREHEAT,              LABEL_PREHEAT_PLA},
    {ICON_PREHEAT,              LABEL_PREHEAT_ABS},
    {ICON_BOTH,                 LABEL_PREHEAT_BOTH},
    {ICON_STOP,                 LABEL_STOP},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
//    {ICON_PREHEAT_CUSTOM1,      LABEL_PREHEAT_CUSTOM1},
//    {ICON_PREHEAT_CUSTOM2,      LABEL_PREHEAT_CUSTOM2},
    // #ifdef UNIFIED_MENU
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    // #else
    // {ICON_HEAT,                 LABEL_HEAT},
    // #endif
    {ICON_BACK,                 LABEL_BACK},
  }
};

const ITEM itemToolPreheat[] = {
// icon                       label
  {ICON_BOTH,         LABEL_PREHEAT_BOTH},
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};   

const uint16_t preheat_bed_temp[] = PREHEAT_BED;
const uint16_t preheat_hotend_temp[] = PREHEAT_HOTEND;

const GUI_RECT info_rect[4] = {
  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},

  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + INFO_OFFSET_X0,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y0 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + INFO_OFFSET_X0 + 4,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X - INFO_OFFSET_X1,  TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + INFO_OFFSET_Y1 + BYTE_HEIGHT},
};

static void drawTemperatureInfo(void)
{
  GUI_SetColor(BLACK);
  GUI_SetBkColor(WHITE);

  GUI_ClearPrect(&info_rect[0]);
  GUI_DispDec(info_rect[0].x0, info_rect[0].y0, preheat_hotend_temp[0], 3, LEFT);

  GUI_ClearPrect(&info_rect[1]);
  GUI_DispDec(info_rect[1].x0, info_rect[1].y0, preheat_bed_temp[0], 3, LEFT);

  GUI_ClearPrect(&info_rect[2]);
  GUI_DispDec(info_rect[2].x0, info_rect[2].y0, preheat_hotend_temp[1], 3, LEFT);

  GUI_ClearPrect(&info_rect[3]);
  GUI_DispDec(info_rect[3].x0, info_rect[3].y0, preheat_bed_temp[1], 3, LEFT);
  
  GUI_SetColor(FK_COLOR);
  GUI_SetBkColor(BK_COLOR);
}

void menuCallBackPreHeat() {
  static TOOLPREHEAT nowHeater = BOTH;
  KEY_VALUES  key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
    case KEY_ICON_1:
      switch(nowHeater){
        case BOTH:
          ExtUI::setTargetTemp_celsius(preheat_hotend_temp[key_num],  ExtUI::getActiveTool());
          ExtUI::setTargetTemp_celsius(preheat_bed_temp[key_num],  ExtUI::BED);
          break;
        case BED_PREHEAT:
          ExtUI::setTargetTemp_celsius(preheat_bed_temp[key_num],  ExtUI::BED);
          break;
        case NOZZLE0_PREHEAT:
          ExtUI::setTargetTemp_celsius(preheat_hotend_temp[key_num],  ExtUI::getActiveTool());
          break;
      }
      drawTemperatureInfo();
      break;
      
    case KEY_ICON_2:
      nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
      preheatItems.items[key_num] = itemToolPreheat[nowHeater];
      menuDrawItem(&preheatItems.items[key_num], key_num);
      break;

    case KEY_ICON_3:
          ExtUI::setTargetTemp_celsius(0,  ExtUI::getActiveTool());
          ExtUI::setTargetTemp_celsius(0,  ExtUI::BED);
          break;
    
    // #ifndef UNIFIED_MENU
    // case KEY_ICON_6:
    //   infoMenu.menu[++infoMenu.cur] = menuHeat;
    //   break;
    
    // #endif
    case KEY_ICON_7:
      infoMenu.cur--; break;
    default:break;
  }
  // drawTemperatureInfo();
}

void menuPreheat(void)
{
  menuDrawPage(&preheatItems);
  drawTemperatureInfo();
  menuSetFrontCallBack(menuCallBackPreHeat);
}
