#ifndef _BABYSTEP_H_
#define _BABYSTEP_H_

extern bool autoCloseBabysetp;
extern float old_baby_step_value;

void menuBabyStep(void);
float getBabyStepZAxisTotalMM();
void setBabyStepZAxisIncMM(float mm);

#endif
