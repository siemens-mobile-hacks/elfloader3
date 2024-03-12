#include "loader3/loader.h"
#include <inc/cfg_items.h>
#include "conf_loader.h"
#include "config_struct.h"

config_structure_t *config = NULL;
const char *successed_config_filename = NULL;

// Обновляем диск у всех путей, чтобы юзеру не приходилось делать это вручную
static inline void SyncDefaultDisk(config_structure_t *config, char disk) {
  config->IMAGES_PATH[0] = disk;
  config->DAEMONS_PATH[0] = disk;
  config->SWIBLIB_PATH[0] = disk;
  config->ELFLOADER_LOG_PATH[0] = disk;
}

static int LoadConfigData(const char *fname)
{
  int f;
  unsigned int ul;
  int result=0;
  void *cfg;
  void *cfg_init;
  unsigned int rlen, end;
  
  cfg=(void *)config;
  cfg_init = (void *)&config_structure;
  
  unsigned int len = sizeof(config_structure);
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    rlen=fread(f, cfg, len, &ul);
    end=lseek(f,0,S_END,&ul,&ul);
    fclose(f,&ul);
    if (rlen!=end || rlen!=len)  goto L_SAVENEWCFG;
  }
  else
  {
  L_SAVENEWCFG:
    memcpy_a(cfg, cfg_init, len);
    SyncDefaultDisk(config, fname[0]);
    if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
    {
      if (fwrite(f,cfg,len,&ul)!=len) result=-1;
      fclose(f,&ul);
    }
    else
      result=-1;
  }
  if (result>=0)
  {
    successed_config_filename=fname;
  }
  return(result);
}

void InitConfig()
{
#ifdef USE_STATIC_MEMORY
  // Если памяти завались, юзаем статическую аллокацию
  static config_structure_t config_storage = { 0 };
  config = &config_storage;
#else
  // Иначе тратим хип
  config = malloc(sizeof(config_structure_t));
#endif
  memcpy_a(config, &config_structure, sizeof(config_structure_t));
  
  if( LoadConfigData("4:\\ZBin\\etc\\ElfPack.bcfg")<0)
  {
    LoadConfigData("0:\\ZBin\\etc\\ElfPack.bcfg");
  }
}
