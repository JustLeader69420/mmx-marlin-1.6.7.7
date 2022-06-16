#include "../TSC_Menu.h"
#include "../../../../../module/settings.h"
#include "../../../../../feature/babystep.h"

#if ENABLED(LEVELING_OFFSET)

//1 titl, ITEM_PER_PAGE item
MENUITEMS LevelingOffsetItems = {
// title
LABEL_PROBE_OFFSET,
// icon                        label
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

#define ITEM_LEVELING_OFFSET_UNIT_NUM 3
const ITEM itemProbeOffsetUnit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};
const float item_LevelingOffset_unit[ITEM_LEVELING_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static ELEMENTS elementsUnit;
float LevelingOffset = LODEVA;    // 当前值,默认0.6
float oldLevelingOffset = 0.0f; // 上一个值

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
 #if 1
  LevelingOffset += offset;
  if(LevelingOffset>= (BABYSTEP_MAX_HIGH*2))  LevelingOffset =  (BABYSTEP_MAX_HIGH*2);
  if(LevelingOffset<=(-BABYSTEP_MAX_HIGH*2))  LevelingOffset = (-BABYSTEP_MAX_HIGH*2);
 #else
  LevelingOffset = offset;
 #endif
}
// 置零
void resetLevelingOffset(void){
  LevelingOffset = 0.0f;
}
// 将数据保存到存储器
bool saveOffset(){
  float newLevelingOffset = LevelingOffset - oldLevelingOffset; // 求变化量
  if (leveling_is_valid() && ((int)(newLevelingOffset*100) != 0)) {   // 判断调平数据有效 且 变化不小于0.01
    LOOP_L_N(py, GRID_MAX_POINTS_Y) {
      LOOP_L_N(px, GRID_MAX_POINTS_X) {
        z_values[px][py] += newLevelingOffset;
      }
    } // 循环将数组内的数据和并。
    
    oldLevelingOffset = LevelingOffset; // 告知已将偏移值合并到调平数据内
    return settings.save(); //判断是否保存成功
  }
  return false; // 没触发保存，保存失败
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
  bool leveling_offset_value_change = false;    // 值是否发生变化
  float ts_value = 0.0f;   // 临时存放数据
  KEY_VALUES key_num =  menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setLevelingOffset(- elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_3:
      setLevelingOffset(elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_4:
      if(saveOffset())
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
      else
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
      
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      LevelingOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&LevelingOffsetItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      resetLevelingOffset(); // 置零
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_7:
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(leveling_offset_value_change)  // 值发生变化，显示刷新
  {
    leveling_offset_value_change = false;
    LevelingOffsetReDraw();
  }
}

void menuLevelingOffset()
{
  // oldLevelingOffset = LevelingOffset;
  initElements(KEY_ICON_5);
  menuDrawPage(&LevelingOffsetItems);
  showLevelingOffset();
  menuSetFrontCallBack(menuCallBackLevelingOffset);
}


void menuCallBackSetLevelingOffset()
{
  bool leveling_offset_value_change = false;    // 值是否发生变化
  float ts_value = 0.0f;   // 临时存放数据
  KEY_VALUES key_num =  menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      setBabyStepZAxisIncMM(-elementsUnit.ele[elementsUnit.cur]);
      setLevelingOffset(- elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_3:
      setBabyStepZAxisIncMM(elementsUnit.ele[elementsUnit.cur]);
      setLevelingOffset(elementsUnit.ele[elementsUnit.cur]);
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_4:
      if(saveOffset())
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_SUCCESS), true);
      else
        popupReminder_SF(textSelect(LABEL_SAVE_POPUP),textSelect(LABEL_EEPROM_SAVE_FAILED), false);
      
      break;
    case KEY_ICON_5:
      elementsUnit.cur = (elementsUnit.cur + 1) % elementsUnit.totaled;
      LevelingOffsetItems.items[key_num] = elementsUnit.list[elementsUnit.cur];
      menuDrawItem(&LevelingOffsetItems.items[key_num], key_num);
      break;
    case KEY_ICON_6:
      resetLevelingOffset(); // 置零
      leveling_offset_value_change = true;
      break;
    case KEY_ICON_7:
      mustStoreCmd("M420 S0\n");
      mustStoreCmd("G28\n");
      infoMenu.cur--;
      break;
    default :
      break;
  }
  if(leveling_offset_value_change)  // 值发生变化，显示刷新
  {
    leveling_offset_value_change = false;
    LevelingOffsetReDraw();
  }
}
void menuSetLevelingOffset()
{
  initElements(KEY_ICON_5);
  menuDrawPage(&LevelingOffsetItems);
  showLevelingOffset();
  menuSetFrontCallBack(menuCallBackSetLevelingOffset);
}

// LEVELINGITEMS levelingSetItems = 
uint8_t *levelingSetButtonItems[4] = 
{
  CHAR_INCREASE,
  CHAR_LOWER,
  CHAR_SAVE,
  CHAR_SBACK,
};
uint8_t levelingSetCubeItems[ITEM_CUBE_NUM-4][8] = 
{
};
void my_ftoa(uint8_t *res, float num, int size){}

static bed_mesh_t old_z_values;
#define THE_FLOAT_MAGNIFICATION 1000
#define SET_LEVELING_VALUE 0.05f

void menuCallBackSetLevelingValue()
{
  uint16_t key_num =  menuKeyGetLevelingValue();
  char str[16];
  static uint8_t x = 255, y = 255;  //用于确定点击的坐标，255为没选中。
  uint8_t x2 = 255, y2 = 255;       //临时存放xy坐标
  uint8_t isClick = 0;
  int the_value = 0;

  switch(key_num)
  {
    case 0:
      // babystep.add_mm(Z_AXIS, 0.1f);
      if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y){
        old_z_values[x][y] += SET_LEVELING_VALUE;
        the_value = (old_z_values[x][y]+1.0f/THE_FLOAT_MAGNIFICATION/100)*THE_FLOAT_MAGNIFICATION;
        // the_value += (SET_LEVELING_VALUE*THE_FLOAT_MAGNIFICATION);

        if(old_z_values[x][y]<=0.00001 && old_z_values[x][y]>=-0.00001){
          levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
          levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
        }else if(old_z_values[x][y]<0){
          sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d%d", (-the_value)/1000, (-the_value/100)%10, (-the_value/10)%10);
        }else{
          sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d%d", the_value/1000, (the_value/100)%10, (the_value/10)%10);
        }
        // sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%f", old_z_values);
        menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
      }
      break;
    case 1:
      // babystep.add_mm(Z_AXIS, -0.1f);
      if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y){
        // old_z_values[x][y] -= 0.1f;
        // the_value = old_z_values[x][y]*100;
        // if(the_value<0){
        //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d", (-the_value)/100, (-the_value)%100);
        // }else if(the_value == 0){
        //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
        //   levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
        // }else{
        //   sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d", the_value/100, the_value%100);
        // }
        old_z_values[x][y] -= SET_LEVELING_VALUE;
        the_value = (old_z_values[x][y]-1.0f/THE_FLOAT_MAGNIFICATION/100)*THE_FLOAT_MAGNIFICATION;
        // the_value -= (SET_LEVELING_VALUE*THE_FLOAT_MAGNIFICATION);

        if(old_z_values[x][y]<=0.00001 && old_z_values[x][y]>=-0.00001){
          levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
          levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
        }else if(the_value<0){
          sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "-%d.%d%d", (-the_value)/1000, (-the_value/100)%10, (-the_value/10)%10);
        }else{
          sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d%d", the_value/1000, (the_value/100)%10, (the_value/10)%10);
        }
        menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
      }
      break;
    case 2:
      
      break;
    case 3:
      // storeCmd("M420 S0\n");
      // storeCmd("G28\n");
      infoMenu.cur--;
      break;

   #if 1
    #if GRID_MAX_POINTS_Y>0
      #if GRID_MAX_POINTS_X>0
        case KEY_0_0:
          if(0==y && 0==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 0; isClick = 1; }
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_0_1:
          if(0==y && 1==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 1; isClick = 1; }
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_0_2:
          if(0==y && 2==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 2; isClick = 1; }
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_0_3:
          if(0==y && 3==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 3; isClick = 1; }
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_0_4:
          if(0==y && 4==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 4; isClick = 1; }
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_0_5:
          if(0==y && 5==x)    { isClick = 2; }
          else{ y2 = 0; x2 = 5; isClick = 1; }
          break;
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>1
      #if GRID_MAX_POINTS_X>0
        case KEY_1_0:
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_1_1:
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_1_2:
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_1_3:
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_1_4:
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_1_5:
          break;
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>2
      #if GRID_MAX_POINTS_X>0
        case KEY_2_0:
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_2_1:
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_2_2:
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_2_3:
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_2_4:
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_2_5:
          break;
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>3
      #if GRID_MAX_POINTS_X>0
        case KEY_3_0:
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_3_1:
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_3_2:
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_3_3:
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_3_4:
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_3_5:
          break;
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>4
      #if GRID_MAX_POINTS_X>0
        case KEY_4_0:
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_4_1:
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_4_2:
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_4_3:
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_4_4:
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_4_5:
          break;
      #endif
    #endif
    #if GRID_MAX_POINTS_Y>5
      #if GRID_MAX_POINTS_X>0
        case KEY_5_0:
          break;
      #endif
      #if GRID_MAX_POINTS_X>1
        case KEY_5_1:
          break;
      #endif
      #if GRID_MAX_POINTS_X>2
        case KEY_5_2:
          break;
      #endif
      #if GRID_MAX_POINTS_X>3
        case KEY_5_3:
          break;
      #endif
      #if GRID_MAX_POINTS_X>4
        case KEY_5_4:
          break;
      #endif
      #if GRID_MAX_POINTS_X>5
        case KEY_5_5:
          break;
      #endif
    #endif
   #endif
  }
  if(1 == isClick){
    if(x<GRID_MAX_POINTS_X && y<GRID_MAX_POINTS_Y)
      menuDrawCubeItem(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
    x = x2; y = y2;
    menuDrawCubeItem_check(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
  }
  else if(2 == isClick){
    menuDrawCubeItem(levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], y*GRID_MAX_POINTS_X+4+x);
    x = 255; y = 255;
  }
}
void menuSetLevelingValue()
{
  int the_value = 0;
  uint8_t x,y;
  for(y=0; y<GRID_MAX_POINTS_Y; y++){
    for(x=0; x<GRID_MAX_POINTS_X; x++){
      the_value = 0;
      old_z_values[x][y] = 0.0f;
      if(leveling_is_valid()){
        the_value = z_values[x][y] * 100;
        old_z_values[x][y] = z_values[x][y];
      }
      if(the_value == 0){
        // sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "0");
        levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][0] = '0';
        levelingSetCubeItems[y*GRID_MAX_POINTS_X+x][1] = 0;
      }
      else{
        sprintf_P((char*)levelingSetCubeItems[y*GRID_MAX_POINTS_X+x], "%d.%d", the_value/100, the_value%100);
      }
    }
  }
  // storeCmd("M420 S1\n");
  // storeCmd("G28\n");
  GUI_Clear(BLACK);
  menuDrawCubePage(levelingSetButtonItems, levelingSetCubeItems);
  menuSetFrontCallBack(menuCallBackSetLevelingValue);
}

#endif
