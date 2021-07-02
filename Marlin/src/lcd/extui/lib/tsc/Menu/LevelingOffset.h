#ifndef _LEVELINGOFFSET_H_
#define _LEVELINGOFFSET_H_

#include "stdint.h"

extern float LevelingOffset;
extern float oldLevelingOffset; 

void menuLevelingOffset(void);
void setLevelingOffset(float offset);
void saveOffset();

#endif
