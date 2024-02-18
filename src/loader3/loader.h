
/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */


#ifndef __LOADER_H__
#define __LOADER_H__

#include <inc/swilib.h>
#include <stdbool.h>

#include "elf.h"

static const unsigned char elf_magic_header[] =
{
  0x7f, 0x45, 0x4c, 0x46,  /* 0x7f, 'E', 'L', 'F' */
  0x01,                    /* Only 32-bit objects. */
  0x01,                    /* Only LSB data. */
  0x01,                    /* Only ELF version 1. */
};

#ifndef __linux__
  #define printf(...)
  #define PATH_SEPARATOR '\\'
#else
  #define PATH_SEPARATOR '/'
#endif

#define NO_FILEORDIR    "no such file or directory"
#define BADFILE         "bad file type"
#define OUTOFMEM        "out of memory"

enum ERROR{

    E_NO_ERROR = 0x0,
    E_RELOCATION,
    E_READ,
    E_SHARED,
    E_HEADER,
    E_SECTION,
    E_RAM,
    E_EMPTY,
    E_FILE,
    E_MACHINE,
    E_ALIGN,
    E_UNDEF,
    E_SYMTAB,
    E_STRTAB,
    E_PHDR,
    E_HASTAB,
    E_DYNAMIC
};

typedef struct
{
  void *lib;
  void *next;
  void *prev;
} Global_Queue;

typedef struct
{
  void *lib;
  void *next;
} Libs_Queue;

typedef enum elf32_type
{
  EXEC_NONE,
  EXEC_ELF,
  EXEC_LIB,
} Elf32_Type;

typedef struct
{
  char* body;
  unsigned int bin_size;
  Elf32_Ehdr ehdr;
  unsigned int v_addr;
  Elf32_Type type;
  Elf32_Word dyn[DT_FLAGS+1];
  Elf32_Sym* symtab;
  Elf32_Rel* jmprel;
  Elf32_Word* hashtab;
  char* strtab;
  Libs_Queue* libs;
  int fp;
  char complete;
  char __is_ex_import;  // 1 для новых эльфов, 0 для старых
  void *meloaded;
  int *switab;          // библиотека функций
  char *fname;          // путь к ELF или .so
  char *temp_env;       // временное переменное окружение для эльфа
  Elf32_Dyn *dynamic;
} Elf32_Exec;

typedef struct
{
  char soname[64];
  Elf32_Exec* ex;
  int users_cnt;
  void *glob_queue;
} Elf32_Lib;

enum EflLoadeDebugHookType {
        ELFLOADER_DEBUG_HOOK_LOAD = 0,
        ELFLOADER_DEBUG_HOOK_UNLOAD = 1,
};

typedef struct {
        void *_r_debug;
        void(*hook)(int type, void *param);
} ElfloaderDebugHook;

typedef int ELF_ENTRY(const char *, void *);
typedef int LIB_FUNC();

extern ElfloaderDebugHook *elfloader_debug_hook;
extern unsigned int ferr;

extern char tmp[258];
void ep_log(Elf32_Exec *ex, const char *data, int size);

#define lprintf(...) { int __dsz = snprintf(tmp, 256, __VA_ARGS__);\
      ep_log(tmp, __dsz); }

extern const uint32_t *pLIB_TOP;
extern const uint32_t Library[];

#define __direct_strcmp(...) ((int (*)(char const *, char const *)) Library[0x0085])(__VA_ARGS__)

extern int __e_div(int delitelb, int delimoe);
extern int get_file_size(const char *fl);
extern __arm char *strrchr_a(const char *s, int c);
extern __arm int memcmp_a(const void *m1, const void *m2, size_t n);
extern __arm void *memcpy_a(void *dest, const void *src, size_t size);
extern __arm void SUBPROC_a(void *elf, void *param);
extern __arm void l_msg(int flags, int msg);

int CheckElf(Elf32_Ehdr *ehdr);
unsigned int GetBinSize(Elf32_Exec *ex, Elf32_Phdr* phdrs);
int LoadSections(Elf32_Exec* ex);
unsigned int elfhash(const char* name);
Elf32_Word findExport(Elf32_Exec* ex, const char* name, unsigned int hash);
Elf32_Word FindFunction(Elf32_Lib* lib, const char* name, unsigned int hash);

/* shared support */
Elf32_Lib* OpenLib(const char *name, Elf32_Exec *ex);
int CloseLib(Elf32_Lib* lib, int immediate);
int dlopen(const char *name);
int dlclose(int handle);
Elf32_Word dlsym(int handle, const char *name);

/* executable support */
Elf32_Exec* elfopen(const char* filenam);
int elfclose(Elf32_Exec* ex);
void *elf_entry(Elf32_Exec *);

void sub_clients(Elf32_Lib* lib);

/* init/fini arrays support */
void run_INIT_Array(Elf32_Exec *ex);
void run_FINI_Array(Elf32_Exec *ex);

#endif
