#include "PrintUdisk.h"
#include "../TSC_Menu.h"

static ExtUI::FileList filelistUdisk;


LISTITEMS printItemsUdisk = {
//  title
LABEL_BACKGROUND,
  {
// icon                       label
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_LABEL,  NULL,           NULL},
   {LIST_BUTTON, CHAR_PAGEUP,    NULL},
   {LIST_BUTTON, CHAR_PAGEDOWN,  NULL},
   {LIST_BUTTON, CHAR_BACK,      NULL},
  }
};


// File list number per page
#define NUM_PER_PAGE	5

SCROLL   titleScrollUdisk;
GUI_RECT titleRectUdisk={10, (TITLE_END_Y - 10 - BYTE_HEIGHT) / 2, LCD_WIDTH_PIXEL-10, (TITLE_END_Y -10 - BYTE_HEIGHT) / 2 + BYTE_HEIGHT};

SCROLL   gcodeScrollUdisk;
uint8_t curPageUdisk = 0;

char titleUdisk[1024] = "UDisk:";
bool EnterDirUdisk(const char *nextdir)
{
  // if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(titleUdisk, "/");
  strcat(titleUdisk, nextdir);
  return 1;
}
void ExitDirUdisk(void)
{
  int i = strlen(titleUdisk);
  for(; i > 0 && titleUdisk[i] != '/';i--)
  {	
  }
  titleUdisk[i]=0;
}


void gocdeListDrawUdisk(void)
{
  Scroll_CreatePara(&titleScrollUdisk, (uint8_t* )titleUdisk, &titleRectUdisk);
  printItemsUdisk.title.address = (uint8_t* )titleUdisk;
  GUI_SetBkColor(TITLE_COLOR);
  GUI_ClearPrect(&titleRectUdisk);
  GUI_SetBkColor(BK_COLOR);

  uint8_t i = 0;

  for(i=0;(i + curPageUdisk * NUM_PER_PAGE < filelistUdisk.count())
          &&(i < NUM_PER_PAGE)                                  ; i++)                  // folder
  {
    if (!filelistUdisk.seek(i + curPageUdisk * NUM_PER_PAGE)) continue;
    if (filelistUdisk.isDir()) 
      printItemsUdisk.items[i].icon = CHAR_FOLDER;
    else 
      printItemsUdisk.items[i].icon = CHAR_FILE;
    printItemsUdisk.items[i].label = (uint8_t* )filelistUdisk.filename();
    menuDrawListItem(&printItemsUdisk.items[i], i);
  }

  for(; (i<NUM_PER_PAGE); i++)			//background
  {		
    printItemsUdisk.items[i].icon = NULL;
    menuDrawListItem(&printItemsUdisk.items[i], i);
  }
}

const int16_t labelVolumeError[] = {LABEL_READ_TFTSD_ERROR, LABEL_READ_U_DISK_ERROR, LABEL_READ_ONBOARDSD_ERROR};

void menuCallBackPrintUdisk(void)
{
  static bool lock = false;
  uint8_t update = 0;
  KEY_VALUES key_num = menuKeyGetValue();

  GUI_SetBkColor(TITLE_COLOR);
  Scroll_DispString(&titleScrollUdisk, LEFT);    //
  GUI_SetBkColor(BK_COLOR);
  #ifndef GCODE_LIST_MODE
    Scroll_DispString(&gcodeScrollUdisk, CENTER); //
  #endif

  if (lock) return;
  lock = true;
  switch(key_num)
  {
    case KEY_ICON_5:			
      if(curPageUdisk > 0)
      {
        curPageUdisk--;
        update=1;
      }
      break;

    case KEY_ICON_6:	
      if(curPageUdisk+1 < (filelistUdisk.count()+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
      {
        curPageUdisk++;
        update=1;
      }
      break;

    case KEY_ICON_7:
      curPageUdisk = 0;
      if(filelistUdisk.isAtRootDir())
      {
       // clearInfoFile();
        infoMenu.cur--;
        break;
      }
      else
      {
        ExitDirUdisk();
       // scanPrintFiles();	
        filelistUdisk.upDir();
        update = 1;
      }
      break;

    case KEY_IDLE:
      break;

    default:                   
      if(key_num <= KEY_ICON_4)
      {	
        if (filelistUdisk.seek(curPageUdisk * NUM_PER_PAGE + key_num))
        {
          if (filelistUdisk.isDir()) {
            if(EnterDirUdisk(filelistUdisk.longFilename()) == false) break;
            update = 1;
            curPageUdisk = 0;
            filelistUdisk.changeDir(filelistUdisk.shortFilename());
          } else { //gcode
            ExtUI::printFile(filelistUdisk.shortFilename());
          }
        }
      }
      break;
  }
    
  if(update)
  {
    update = 0;
    gocdeListDrawUdisk();
  }
  
  if(!IS_SD_INSERTED())
  {
   // resetInfoFile();
    infoMenu.cur--;
  }
  lock = false;
}
// menuPrintFromSource
void menuPrintUdisk(void)
{
  GUI_Clear(BK_COLOR);
  GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_LOADING));
  if (ExtUI::isMediaInserted())
  {
    filelistUdisk.refresh();
    for(uint8_t i = 0; i < NUM_PER_PAGE; i++) printItemsUdisk.items[i].icon = NULL;
    menuDrawListPage(&printItemsUdisk);
    gocdeListDrawUdisk();
    menuSetFrontCallBack(menuCallBackPrintUdisk);
  }
  else
  {
    GUI_DispStringInRect(0, 0, LCD_WIDTH_PIXEL, LCD_HEIGHT_PIXEL, textSelect(LABEL_READ_U_DISK_ERROR));
    ExtUI::delay_ms(1000);
    infoMenu.cur--;
  }
}