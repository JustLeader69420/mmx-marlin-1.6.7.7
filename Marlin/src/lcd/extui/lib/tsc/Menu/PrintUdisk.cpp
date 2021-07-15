#include "PrintUdisk.h"
#include "../TSC_Menu.h"
// #include "stm32_usb.h"
#include "ff.h"

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
char gUdiskPath[1024];

char titleUdisk[128] = "UDisk:";
bool EnterDirUdisk(const char *nextdir)
{
  // if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(gUdiskPath, "/");
  strcat(gUdiskPath, nextdir);
  return 1;
}
void ExitDirUdisk(void)
{
  int i = strlen(gUdiskPath);
  for(; i > 0 && gUdiskPath[i] != '/';i--)//从右往左找/
  {	
  }
  gUdiskPath[i]=0;
}
enum {
  FTYPE_GCODE = 0,
  FTYPE_DIR,
};
// typedef struct
// {
//   int isDir;
//   char fname[255]; //store short fname
// }ufl_t;
FILINFO ufilelist[NUM_PER_PAGE];
int udisk_cwd_fcount; //when change dir, need update
bool udisk_at_root = true;

static inline bool is_dir_or_gcode(FILINFO *fno)
{
  return !( fno->fattrib &  AM_SYS ) &&
         (fno->fattrib & AM_DIR ) ||
          strstr(fno->altname, ".GCO") ;
}
//
// Get the number of (compliant) items in the folder
//
int udisk_file_count_dir_and_gcode(char *path) {
  DIR dir;
  int c = 0;
  FRESULT res;
  FILINFO fileinfo;
  res = f_opendir(&dir, path);
  while (1) {
    res = f_readdir(&dir, &fileinfo);
    if (res != FR_OK || fileinfo.fname[0] == 0) break;
    if ( fileinfo.fattrib &  AM_SYS ) continue; //ignore system file
    if (  (fileinfo.fattrib & AM_DIR ) ||
          strstr(fileinfo.altname, ".GCO") )
    {
      c++;
      rtt.printf("-%s/%s \n", path, fileinfo.fname);
    }
  }
  f_closedir(&dir);
    // c += is_dir_or_gcode(p);
    // flag.filenameIsDir                                  // All Directories are ok
    // || (p.name[8] == 'G' && p.name[9] != '~')           // Non-backup *.G* files are accepted
  return c;
}

DIR workDir;
FILINFO workFileinfo;

//true if read valid in current pos
bool udisk_seek(const uint16_t pos) {
    if ( (pos + 1) > udisk_cwd_fcount )
      return false;
    
    f_opendir(&workDir, gUdiskPath);
    for (int c=0; c < udisk_cwd_fcount;) {
      f_readdir(&workDir, &workFileinfo);
      // rtt.printf("list file:%s\n", workFileinfo.fname);
      if (is_dir_or_gcode(&workFileinfo)) {
        c++;
        if (c == pos) {
          rtt.printf("chose file:%s\n", workFileinfo.fname);
          return true;
        }
      }
    }
}

void copy_file_to_sdcard(char* path)
{
  uint8_t buffer[512];
  FIL fp;
  FRESULT res;
  UINT rd_cnt;
  f_open(&fp, path,  FA_READ | FA_OPEN_ALWAYS);
  card.openFileWrite(path);
  while (1) {
    res = f_read(&fp, buffer, sizeof(buffer), &rd_cnt);
    card.write(buffer, rd_cnt);
    if (rd_cnt == 0) break;
  }
  f_close(&fp);
  card.closefile();
}

void gocdeListDrawUdisk(void)
{
  memset(titleUdisk, 0, sizeof(titleUdisk));
  strncpy(titleUdisk, gUdiskPath, sizeof(titleUdisk));
  Scroll_CreatePara(&titleScrollUdisk, (uint8_t* )titleUdisk, &titleRectUdisk);
  printItemsUdisk.title.address = (uint8_t* )titleUdisk;
  GUI_SetBkColor(TITLE_COLOR);
  GUI_ClearPrect(&titleRectUdisk);
  GUI_SetBkColor(BK_COLOR);

  uint8_t i = 0;
  udisk_cwd_fcount =  udisk_file_count_dir_and_gcode(gUdiskPath);
  rtt.printf("found udisk cnt %d\n", udisk_cwd_fcount);
  for(i=0; (i + curPageUdisk * NUM_PER_PAGE < udisk_cwd_fcount)
          && (i < NUM_PER_PAGE); i++)                  // folder
  {
    if (!udisk_seek(i + curPageUdisk * NUM_PER_PAGE)) continue;
    if (workFileinfo.fattrib & AM_DIR) 
      printItemsUdisk.items[i].icon = CHAR_FOLDER;
    else 
      printItemsUdisk.items[i].icon = CHAR_FILE;
    printItemsUdisk.items[i].label = (uint8_t* )workFileinfo.fname;
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
      if(curPageUdisk+1 < (udisk_cwd_fcount+(NUM_PER_PAGE-1))/NUM_PER_PAGE)
      {
        curPageUdisk++;
        update=1;
      }
      break;

    case KEY_ICON_7:
      curPageUdisk = 0;
      if(udisk_at_root)
      {
       // clearInfoFile();//如果是根目录就退回到UI
        infoMenu.cur--;
        break;
      }
      else
      {
        ExitDirUdisk(); //返回上级目录
        // filelistUdisk.upDir();
        f_opendir(&workDir, gUdiskPath);
        // f_getcwd(gUdiskPath, sizeof(gUdiskPath));
        if (gUdiskPath[0] == 0) {
          udisk_at_root = true;
        }
        update = 1;
      }
      break;

    case KEY_IDLE:
      break;

    default:                   
      if(key_num <= KEY_ICON_4)
      {	
        if (udisk_seek(curPageUdisk * NUM_PER_PAGE + key_num))
        {
          if (workFileinfo.fattrib & AM_DIR) {
            EnterDirUdisk(workFileinfo.fname);
            if(FR_OK != f_opendir(&workDir, gUdiskPath)) break;
            update = 1;
            curPageUdisk = 0;
            udisk_at_root = false;
          } else { //gcode
            rtt.printf("FIXME: print:%s\n", workFileinfo.fname);
            //card use short filename
            if (!card.getroot().exists(workFileinfo.altname)) {
              //copy a file to sd card. FIXME: check if filesize = 0.
              copy_file_to_sdcard(workFileinfo.altname);
              ExtUI::printFile(workFileinfo.altname);
            }
            else {
              ExtUI::printFile(workFileinfo.altname);
            }
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