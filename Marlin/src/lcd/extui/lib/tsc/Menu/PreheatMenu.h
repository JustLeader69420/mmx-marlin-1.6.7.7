#ifndef _PREHEATMENU_H_
#define _PREHEATMENU_H_

//                       PLA       ABS   "CUSTOM2" "SHUTDOWN" "CUSTOM3"
#define PREHEAT_BED      {50,      80,       55,       0,       55}
#define PREHEAT_HOTEND   {190,     230,      200,      0,      200}

#define INFO_OFFSET_X0  54
#define INFO_OFFSET_X1  15
#define INFO_OFFSET_Y0  20
#define INFO_OFFSET_Y1  61

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
}TOOLPREHEAT;


void menuPreheat(void);

#endif

