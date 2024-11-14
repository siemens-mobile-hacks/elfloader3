#include <inc/cfg_items.h>
#include "loader3\loader.h"
#include "config_struct.h"
//Конфигурация


const config_structure_t config_structure = 
{
  {CFG_UINT, "Alpha-chan threshold", 0,255},
  128,
  
  {CFG_UINT,"PNG cache size",20,512},
  50,
  
  {CFG_CBOX,"Default bit-depth",0,3},
#if NEWSGOLD || X75
  2,
#else
  1,
#endif
  {"8 bits","16 bits","24 bits+alpha"},
  
  {CFG_CHECKBOX,"Load daemon in subproc",0,2},
  1,
  
  {CFG_STR_UTF8, "Path to images", 0, 127},
  "0:\\ZBin\\img\\",

  {CFG_STR_UTF8, "Path to daemons", 0, 127},
  "0:\\ZBin\\Daemons\\",
  
  {CFG_STR_UTF8, "Path to swi.blib", 0, 127},
  "0:\\ZBin\\swi.blib",
  
  {CFG_CHECKBOX,"Realtime lib cache cleaner",0,2},
  1,
  
  {CFG_STR_UTF8, "LD_LIBRARY_PATH", 0, 255},
  "0:\\ZBin\\lib\\;4:\\ZBin\\lib\\;",
  
  {CFG_STR_UTF8, "Path to log", 0, 127},
  "0:\\ep3_log.txt",
  
  {CFG_UINT,"Max log size in bytes",0,50*1024},
  512,
  
  {CFG_CHECKBOX, "Loader warnings", 0, 2},
  0
};



