#ifndef _LEVELINGOFFSET_H_
#define _LEVELINGOFFSET_H_

#include "stdint.h"

#define LODEVA 0.6f   //LevelingOffset default values 调平补偿的默认值

extern float LevelingOffset;
extern float oldLevelingOffset; 

void menuLevelingOffset(void);
void setLevelingOffset(float offset);
bool saveOffset();

#endif
