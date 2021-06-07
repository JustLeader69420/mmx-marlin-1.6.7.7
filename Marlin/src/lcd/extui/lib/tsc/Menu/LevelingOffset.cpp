#include "../TSC_Menu.h"
#include "../../../../../module/settings.h"

//1 titl, ITEM_PER_PAGE item
MENUITEMS LevelingOffsetItems = {
// title
LABEL_PROBE_OFFSET,
// icon                        label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_NORMAL_SPEED,         LABEL_VALUE_ZERO},
  {ICON_BACK,                 LABEL_BACK},}
};

typedef struct
{
  const ITEM *list;
  const float *ele;
  uint8_t cur;
  uint8_t totaled;
}ELEMENTS;

#define ITEM_LEVELING_OFFSET_UNIT_NUM 3
const ITEM itemProbeOffsetUnit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_LevelingOffset_unit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;
float LevelingOffset = 0.2;

static void initElements(uint8_t position)
{
  elementsUnit.totaled = ITEM_LEVELING_OFFSET_UNIT_NUM;
  elementsUnit.list = itemProbeOffsetUnit;
  elementsUnit.ele  = item_LevelingOffset_unit;

  for(uint8_t i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &LevelingOffsetItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

/* 获取调平补偿 */
float getLevelingOffset(void)
{  
  return LevelingOffset;
}
/* 设置调平补偿 */
void setLevelingOffset(float offset)
{  
  LevelingOffset = offset;
}

void showLevelingOffset(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getLevelingOffset(), 3, 2, RIGHT);
}
void LevelingOffsetReDraw(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getLevelingOffset(), 3, 2, RIGHT);
}


void menuCallBackLevelingOffset(void)
{
  static float leveling_offset_value = 0.0f;    // 用于显示屏幕的值
  KEY_VALUES key_num =  menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setLevelingOffset(leveling_offset_value - elementsUnit.ele[elementsUnit.cur]);
      break;
    case KEY_ICON_3:
      setLevelingOffset(leveling_offset_value + elementsUnit.ele[elementsUnit.cur]);
      break;
    case KEY_ICON_4:
      settings.save();
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      LevelingOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&LevelingOffsetItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      setLevelingOffset(0); // 置零
      break;
    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(leveling_offset_value != getLevelingOffset())
  {
    leveling_offset_value = getLevelingOffset();
    LevelingOffsetReDraw();
  }
}

void menuLevelingOffset()
{
  initElements(KEY_ICON_5);
  menuDrawPage(&LevelingOffsetItems);
  showLevelingOffset();
  menuSetFrontCallBack(menuCallBackLevelingOffset);
}

