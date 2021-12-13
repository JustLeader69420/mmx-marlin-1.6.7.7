#include "BabyStep.h"
#include "../TSC_Menu.h"
#include "../../../../../feature/babystep.h"
#include "../../../../../module/settings.h"

#define BABYSTEP_MAX_HIGH 2.0f
#define WHO_AXIS Z_AXIS

//1 title, ITEM_PER_PAGE items(icon+label) 
MENUITEMS babyStepItems = {
//title
  LABEL_BABYSTEP,
//icon                        label
 {{ICON_DOWN,                 LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_UP,                   LABEL_INC},
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

#define ITEM_BABYSTEP_UNIT_NUM 3
const ITEM itemBabyStepUnit[ITEM_BABYSTEP_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_babystep_unit[ITEM_BABYSTEP_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;
float old_baby_step_value = 0.0f;
bool autoCloseBabysetp = false;

static void initElements(uint8_t position)
{
  elementsUnit.totaled = ITEM_BABYSTEP_UNIT_NUM;
  elementsUnit.list = itemBabyStepUnit;
  elementsUnit.ele  = item_babystep_unit;

  for(uint8_t i=0; i<elementsUnit.totaled; i++)
  {
    if(memcmp(&elementsUnit.list[i], &babyStepItems.items[position], sizeof(ITEM)) == 0)
    {
      elementsUnit.cur = i;
      break;
    }
  }
}

float getBabyStepZAxisTotalMM(){
  return babystep.axis_total[BS_TOTAL_IND(WHO_AXIS)] * planner.steps_to_mm[WHO_AXIS];
}

static int WhatIsBabyStepMM(float mm){
  float oldmm = getBabyStepZAxisTotalMM();
  // 超越值
  if((oldmm>=BABYSTEP_MAX_HIGH && mm>0) || (oldmm<=-BABYSTEP_MAX_HIGH && mm<0))
  {
    return 0;
  }
  // 快到顶
  if((oldmm+mm) >= BABYSTEP_MAX_HIGH)
  {
    return 1;
  }
  // 快到底
  if((oldmm+mm) <= -BABYSTEP_MAX_HIGH)
  {
    return 2;
  }
  // 常规
  return 3;
}

void setBabyStepZAxisIncMM(float mm){
  float oldmm = getBabyStepZAxisTotalMM();
  switch (WhatIsBabyStepMM(mm))
  {
    case 1:
      babystep.add_mm(WHO_AXIS, BABYSTEP_MAX_HIGH - oldmm);    // 值快到顶了
      break;

    case 2:
      babystep.add_mm(WHO_AXIS, -BABYSTEP_MAX_HIGH - oldmm);   // 值快到底了
      break;

    case 3:
      babystep.add_mm(WHO_AXIS, mm);              // 值正常
      break;
    
    default:                                    // 其它
      break;
  }
}

void showBabyStep(void)
{
  GUI_DispFloat(CENTER_X - 5*BYTE_WIDTH/2, CENTER_Y, getBabyStepZAxisTotalMM(), 3, 2, RIGHT);
}

void menuCallBackBabyStep(void)
{
  static float baby_step_value = 0.0f;
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setBabyStepZAxisIncMM(-elementsUnit.ele[elementsUnit.cur]);
      break;
    case KEY_ICON_3:
      setBabyStepZAxisIncMM(elementsUnit.ele[elementsUnit.cur]);
      break;
    case KEY_ICON_4:
       #ifdef AUTO_BED_LEVELING_BILINEAR
        setLevelingOffset(getBabyStepZAxisTotalMM() - old_baby_step_value); // 将当前BabyStep的值赋给z_offset
        old_baby_step_value = getBabyStepZAxisTotalMM();
        popupReminder_B(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_SYCHRONZIED_VALUE));
        // infoMenu.cur--;
       #endif
      // settings.save();                              // 保存，注意保存的是z_offset的值，而不是BabyStep的值，BabyStep每次复位都会被清零，防止干扰
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      babyStepItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&babyStepItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      setBabyStepZAxisIncMM(-baby_step_value);
      break;
    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(baby_step_value != getBabyStepZAxisTotalMM())
  {
    baby_step_value = getBabyStepZAxisTotalMM();
    showBabyStep();
  }
  if(autoCloseBabysetp){
    autoCloseBabysetp = false;
    infoMenu.cur--;
  }
}

void menuBabyStep()
{
  initElements(KEY_ICON_5);
  menuDrawPage(&babyStepItems);
  showBabyStep();
  menuSetFrontCallBack(menuCallBackBabyStep);
}
