#include <inc/swilib.h>
#include <stdbool.h>
#include "imgcache.h"
#include "conf_loader.h"
#include "loader3/loader.h"
#include "loader3/env.h"
#include "config_struct.h"

int dlclean_cache();
const uint32_t *pLIB_TOP = NULL;
__no_init bool is_initialized;

extern BXR1(void *, void (*)(void *));

static __arm char *RamPressedKey_a() {
  return RamPressedKey();
}

/* �������� ����� */
int elfload(char *filename, void *param1, void *param2, void *param3){
  
  Elf32_Exec *ex = elfopen(filename);
  if(!ex){
    l_msg(1, (int)"Elf corrupt or missing");
    return -1;
  }
  
  int (*entry)(char *, void *, void*, void*) = (int (*)(char *, void *, void*, void*))elf_entry(ex);
  if(!entry){
   l_msg(1, (int)"Entry point not found");
   elfclose(ex);
   return -2;
  }
  
    
  if(!ex->__is_ex_import && ex->libs)
  {
    l_msg(1, (int)"Incorrect elf, symbol __ex not found.");
    elfclose(ex);
    return -3;
  }
  
  extern __arm void ExecuteIMB(void);
  ExecuteIMB();
  
  //run_INIT_Array(ex);
  entry(filename, param1, param2, param3);

  // ������� ��� IAR-style ������
  if(!ex->__is_ex_import && !ex->libs)
  {
    ex->body = NULL; // ���� ����� �� �����������!!!
    elfclose(ex);
  }
  

  return 0;
}


static void InitLoaderSystem()
{
  setenv("LD_LIBRARY_PATH", *config->LD_LIBRARY_PATH_env ? config->LD_LIBRARY_PATH_env : "0:\\ZBin\\lib\\;4:\\ZBin\\lib\\;", 1);
}


int main()
{
  return 0;
}


static int elfloader_onload(WSHDR *filename, WSHDR *ext, void *param){
  char fn[128];
  ws_2str(filename,fn,126);
  if (elfload(fn, param, 0, 0)) return 0; else return 1;
}

//=======================================================================
//
//=======================================================================
extern void(*OldOnClose)(void *);
extern void(*OldOnCreate)(void *);
#ifdef NEWSGOLD
extern void(*OldShowMsg)(int, int);
#else
extern void(*OldTxtOpen)(WSHDR*, WSHDR*);
#endif

extern unsigned int DEFAULT_DISK_N;

//-----------------------------------------------------------------------

#ifdef NEWSGOLD
//#define HELPER_CEPID 0x4339
#define HELPER_CEPID 0x440A
#else
//#define HELPER_CEPID 0x4331
#define HELPER_CEPID 0x4407

#endif
#define MSG_HELPER_RUN 0x0001

static __arm void proc_HELPER(void)
{
  GBS_MSG msg;
  if (GBS_RecActDstMessage(&msg))
  {
    if (msg.msg==MSG_HELPER_RUN)
    {
      if (msg.data0)
      {
  ((void (*)(int, void *))(msg.data0))(msg.submess,msg.data1);
      }
    }
    else
    {
      GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,msg.pid_from,msg.msg,msg.submess);
    }
  }
}

static __arm void CreateHELPER_PROC(void)
{
  static const char name[]="HELPER";
  CreateGBSproc(HELPER_CEPID, name, proc_HELPER, 0x80, 0);
}

__arm void REDRAW_impl(void)
{
  LockSched();
#ifdef NEWSGOLD
  PendedRedrawGUI();
#else
  PendedRedrawGUI();
  GBS_SendMessage(MMI_CEPID,0x90);
#endif
  UnlockSched();
}

__arm void SUBPROC_impl(void *f, int p2, void *p1)
{
  GBS_SendMessage(HELPER_CEPID,MSG_HELPER_RUN,p2,f,p1);
}

void SEQKILLER_impl(void *data, void(*next_in_seq)(void *), void *data_to_kill)
{
  next_in_seq(data);
  mfree(data_to_kill);
}

static void LoadDaemons(void)
{

  DIR_ENTRY de;
  unsigned int err;
  unsigned int pathlen;
  char name[256];

  // ������������ ��������
  sprintf(name, "%sCore\\DebugHook.elf", config->DAEMONS_PATH);
  if (get_file_size(name) > 0) {
    elfload(name, 0, 0, &elfloader_debug_hook);
  }

  sprintf(name, "%s*.elf", config->DAEMONS_PATH);
  pathlen=strlen(config->DAEMONS_PATH);
  
  if (FindFirstFile(&de,name,&err))
  {
    do
    {
      name[pathlen] = 0;
      strcat(name, de.file_name);
      strcat(name, ".skip");
      
      // ��������� .elf ������ ���� ����� ����������� ���� "NAME.elf.skip"
      if (get_file_size(name) < 0) {
        name[pathlen] = 0;
        strcat(name, de.file_name);
        elfload(name, 0, 0, 0);
      }
    }
    while(FindNextFile(&de,&err));
  }
  FindClose(&de,&err);
}

int get_file_size(const char * fname)
{
  FSTATS fs;
  unsigned int err;
  if (GetFileStats(fname,&fs,&err)==-1) return (-1);
  else return (fs.size);
}

static inline uint32_t *AllocSwilib(void) {
#ifdef USE_STATIC_MEMORY
  static uint32_t swilib_storage[4096] = { 0 };
  return swilib_storage;
#else
  return malloc(sizeof(uint32_t) * 4096);
#endif
}

static inline void FreeSwilib(uint32_t *swilib) {
#ifndef USE_STATIC_MEMORY
  mfree(swilib);
#endif
}

// ��������� ������������� swi.blib � ���������
static bool IsSwiBlibCompatible(uint32_t *new_lib) {
  // ������ �������� 4 ��������� �������, ��� ������� ELFLoader �� ������� ��
  const int funcs[] = { 0x17, 0x18, 0x19, 0x1A };
  for (int i = 0; i < (sizeof(funcs) / sizeof(funcs[0])); i++) {
    if (new_lib[funcs[i]] != 0xFFFFFFFF && Library[funcs[i]] != new_lib[funcs[i]])
      return false;
  }
  return true;
}

// ������� ������� swilib
static void SetDynSwilib(uint32_t *new_swilib) {
  if (!new_swilib) {
    new_swilib = AllocSwilib();
    memset(new_swilib, 0xFF, sizeof(uint32_t) * 4096);
  }

  for (int i = 0; i < 0x1000; i++) {
    // ���� ������� ��� � swi.blib, ���������� ����������� �� library.vkp
    if (new_swilib[i] == 0xFFFFFFFF)
      new_swilib[i] = Library[i];
    
    // ����������� ������ � ���, ��� ������� �� ����������
    // ������������ ��� GCC, ��� ���������� ������������ SWI
    if (new_swilib[i] == 0xFFFFFFFF)
      new_swilib[i] = 0xFFFF0000 | (i << 4); // ������ �������: FFFFxxx0 (xxx - ����� swi)
  }
  
  // �������� ������� �������
  pLIB_TOP = new_swilib;
}

// �������� swi.blib
static bool LoadDynSwilibFromFile(void)
{
  unsigned int err;
  int sz = get_file_size(config->SWIBLIB_PATH);
  if (sz == -1) // no swi.blib
    return false;

  // ������ ������ ���� ������
  if (sz != 0x4000) {
    l_msg(1, (int) "Illegal swi.blib size!");
    return false;
  }

  int fd = fopen(config->SWIBLIB_PATH, A_ReadOnly + A_BIN, P_READ, &err);
  if (fd < 0)
    return 0;

  uint32_t *new_lib = AllocSwilib();
  if (!new_lib || fread(fd, new_lib, sz, &err) != 0x4000) {
    l_msg(1, (int) "Can't read swi.blib!");
    fclose(fd, &err);
    FreeSwilib(new_lib);
    return false;
  }

  fclose(fd, &err);

  if (!IsSwiBlibCompatible(new_lib)) {
    l_msg(1, (int) "swi.blib is not compatible with your phone!");
    FreeSwilib(new_lib);
    return false;
  }

  // ���� �� ��, �������� ���������� �������
  SetDynSwilib(new_lib);
  
  return true;
}

char smallicons_str[32] = { 0 };
char bigicons_str[32] = { 0 };

#pragma segment="DATA_Z"
static __thumb void InitLoaderMemory() {
  unsigned char *mem = __segment_begin("DATA_Z");
  while (mem != __segment_end("DATA_Z")) {
    *mem++ = 0;
  }
}

__thumb void InitLoader(void *data)
{
#ifndef USE_LL_INIT
  // ������ ������� �������������, ��� ������ � ������������� ��
  InitLoaderMemory();
#endif

  static const int smallicons[2]={(int)smallicons_str,0};
  static const int bigicons[2]={(int)bigicons_str,0};
  
#ifdef NEWSGOLD
  static const REGEXPLEXT elf_reg=
  {
    "elf",
    0x55,
    0xFF,
    8, //������� Misc
    MENU_FLAG2,
    smallicons,
    bigicons,
    (int)"Open",    //LGP "�������"
    (int)"AltOpen", //LGP "�����"
    LGP_DOIT_PIC,
    (void *)elfloader_onload,
    0
  };
#else
  static const REGEXPLEXT elf_reg=
  {
    "elf",
    0x55,
    0xFF,
    7,
    MENU_FLAG2,
    smallicons,
    bigicons,
    (void *)elfloader_onload,
    0
  };
#endif
  CreateHELPER_PROC();
  InitConfig();

  bool is_safe_mode = *RamPressedKey_a() == '#';

  // ��������� swi.blib, �� ������ ���� �� ����������� SafeMode
  if (is_safe_mode || !LoadDynSwilibFromFile()) {
    // ���� �� ������ ��������� swi.blib, ������ �������� library.vkp � ����
    SetDynSwilib(NULL);
  }

  InitLoaderSystem();
  ImgCache_Init();
  //strcpy(smallicons_str+1,":\\ZBin\\img\\elf_small.png");
  //strcpy(bigicons_str+1,":\\ZBin\\img\\elf_big.png");
  //smallicons_str[0]=bigicons_str[0]=DEFAULT_DISK_N+'0';
  sprintf(smallicons_str, "%self_small.png", config->IMAGES_PATH);
  sprintf(bigicons_str, "%self_big.png", config->IMAGES_PATH);
  RegExplorerExt(&elf_reg);

  if(!is_safe_mode)
  {
    if(config->load_in_suproc) {
      // ���������, �� ���������� ������
      SUBPROC_a((void *)LoadDaemons, NULL);
    } else {
      // ����� ������� ������, �� �� �� ����� ELF ����������
      LoadDaemons();
    }
  }
}

// ���������� ��� �������� IDLE CSM
__arm void MyIDLECSMonCreate(void *data)
{
  if (!is_initialized) {
    is_initialized = true;
    InitLoader(data);
  }
  BXR1(data, OldOnCreate);
}

// ���������� ��� �������� IDLE CSM
__arm void MyIDLECSMonClose(void *data)
{
  // ������ ��� ��� ���������� ELFLoader, �� � ��� ��� ������:
  // 1. ����� ����� ����������� ������� ��� ���������� ��������?
  // 2. IDLE CSM ��� �� ���� ����� ��������� �� ����� ���������� ������ �������� (��������, ����� ������� ����� ����)
  BXR1(data, OldOnClose);
}

static unsigned int char8to16(int c)
{
  if (c==0xA8) c=0x401;
  if (c==0xAA) c=0x404;
  if (c==0xAF) c=0x407;
  if (c==0xB8) c=0x451;
  if (c==0xBA) c=0x454;
  if (c==0xBF) c=0x457;
  if (c==0xB2) c=0x406;
  if (c==0xB3) c=0x456;
  if ((c>=0xC0)&&(c<0x100)) c+=0x350;
  return(c);
}

static void ascii2ws(char *s, WSHDR *ws)
{
  int c;
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c));
  }
}
#ifdef NEWSGOLD
__arm void ESI(WSHDR *ws, int dummy, char *s)
#else
__arm void ESI(char *s, WSHDR *ws)
#endif
{
  if (((unsigned int)s>>28)==0xA) //������ ������ �� ������� RAM/FLASH
  {
    CutWSTR(ws,0); 
    ascii2ws(s,ws);
  }
  else
  {
    wsprintf(ws,"|%d|",s);
  }
}

/*int toupper(int c)
{
if ((c>='a')&&(c<='z')) c+='A'-'a';
return(c);
}*/

//static const char extfile[]=DEFAULT_DISK ":\\ZBin\\etc\\extension.cfg";

static __arm void DoUnknownFileType(WSHDR *filename)
{
  WSHDR *wsmime=AllocWS(15);
  wsprintf(wsmime,"txt");
  ExecuteFile(filename,wsmime,0);
  FreeWS(wsmime);
}

#ifdef NEWSGOLD
__no_init int *EXT2_AREA;
#ifdef ELKA
__no_init int EXT2_CNT @ "REGEXPL_CNT";
#endif
#else
__no_init TREGEXPLEXT *EXT2_AREA;
__no_init int EXT2_CNT @ "REGEXPL_CNT";
#endif

#ifdef NEWSGOLD 
#ifdef ELKA
#else
__arm int *GET_EXT2_TABLE(void)
{
  int *p=EXT2_AREA;
  if (p)
  {
    return(p+1);
  }
  p=malloc(4);
  *p=0;
  return ((EXT2_AREA=p)+1);
}
#endif
#endif  

#ifdef NEWSGOLD 
#ifdef ELKA
__arm int *EXT2_REALLOC(void)
{
  int size;
  size=sizeof(REGEXPLEXT);
  int *p;
  int *p2;
  int n;
  LockSched();
  n=EXT2_CNT;
  p=EXT2_AREA;
  p2=malloc((n+1)*size);
  if (p) 
  {    
    memcpy(p2,p,n*size);
    mfree(p);
  }
  EXT2_CNT=n+1;
  EXT2_AREA=p2;
  p2+=(n*(size/sizeof(int)));
  UnlockSched();
  return (p2);
}
#else
__arm int *EXT2_REALLOC(void)
{
  int size;
  size=sizeof(REGEXPLEXT);
  int *p;
  int *p2;
  int n;
  LockSched();
  n=*(p=EXT2_AREA);
  p2=malloc((n+1)*size+4);
  memcpy(p2,p,n*size+4);
  *p2=n+1;
  mfree(p);
  EXT2_AREA=p2;
  p2+=(n*(size/sizeof(int)))+1;
  UnlockSched();
  return (p2);
}
#endif
#else
__arm TREGEXPLEXT *EXT2_REALLOC(void)
{
  TREGEXPLEXT *p,*p2;
  int n;
  LockSched();
  n=EXT2_CNT;
  p=EXT2_AREA;
  p2=malloc((n+1)*sizeof(TREGEXPLEXT));
  zeromem(p2,(n+1)*sizeof(TREGEXPLEXT));
  if (p) 
  {    
    memcpy(p2,p,n*sizeof(TREGEXPLEXT));
    mfree(p);
  }
  EXT2_CNT=n+1;
  EXT2_AREA=p2;
  UnlockSched();
  return (p2+n);
}


#endif



#ifdef NEWSGOLD
__thumb MyShowMSG(int p1, int p2)
{
  if (p2!=0x1DD1)
  {
    OldShowMsg(p1, p2);
    return;
  }
  asm("MOVS R0,R6\n");
  DoUnknownFileType((WSHDR *)p1);
}
#else

__arm void PropertyPatch(WSHDR *unk_foldername, WSHDR *unk_filename)
{
  WSHDR *ws;
  ws=AllocWS(255);
  wstrcpy(ws,unk_foldername);
  wsAppendChar(ws,'\\');
  wstrcat (ws,unk_filename);
  DoUnknownFileType(ws);
  FreeWS(ws);
}

#endif

__arm void FUNC_ABORT(int f)
{
  char s[32];
  extern void StoreErrInfoAndAbort(int code,const char *module_name,int type,int unk3);
  extern void StoreErrString(const char *);
  sprintf(s,"%d(%03X)",f,f);
  StoreErrString(s);
  loopback2();
  StoreErrInfoAndAbort(0xFFFF,"\1\1No function in lib\xA1",2,2);
}

#ifdef USE_LL_INIT
// ��� ������� ���������� �� ������ ������ ������������� �� (����� ������������� ExtRAM)
__arm void LowLevelInit()
{
  EXT2_AREA = NULL;
  is_initialized = false;
  InitLoaderMemory();
}
#endif

//�����
#pragma diag_suppress=Pe177
__root static const int NEW_ONCREATE @ "PATCH_ONCREATE" = (int)MyIDLECSMonCreate;

__root static const int NEW_ONCLOSE @ "PATCH_ONCLOSE" = (int)MyIDLECSMonClose;

#ifdef NEWSGOLD
__root static const int NEW_SHOWMSG @ "PATCH_SHOWMSG_BLF" = (int)MyShowMSG;
#endif

__root static const int SWILIB_FUNC171 @ "SWILIB_FUNC171" = (int)SUBPROC_impl;

__root static const int SWILIB_FUNC172 @ "SWILIB_FUNC172" = (int)REDRAW_impl;

__root static const int SWILIB_FUNC19C @ "SWILIB_FUNC19C" = (int)SEQKILLER_impl;
#pragma diag_default=Pe177


