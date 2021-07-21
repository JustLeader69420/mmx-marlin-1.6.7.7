#ifndef _PRINT_UDISK_H_
#define _PRINT_UDISK_H_

#include "ff.h"

extern bool UDiskPrint;
extern bool UDiskPausePrint;
extern bool UDiskStopPrint;
extern bool UDiskPrintFinish;
extern FIL udisk_fp;
extern uint64_t UDiskPrintSize;
extern uint64_t UDiskFileSize;

extern FILINFO workFileinfo;
// void menuPrintFromSource(void);
void menuPrintUdisk(void);

#endif
