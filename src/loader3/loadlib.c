
/*
 * ���� ���� �������� ������ ��������� ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"
#include "env.h"
#include "../config_struct.h"

char tmp[258] = {0}, dlerr[128]={0};

Global_Queue* lib_top = 0;
Elf32_Lib** handles = 0;
int handles_cnt = 0;

 /*
  * ���������� ��� �����
  */
unsigned int elfhash(const char* name)
{
    unsigned int hash = 0;
    unsigned int hi;
    /* ��� ���� *name ������� ������ ������� */
    register unsigned char c = *name++;
    
    while (c != '\0')
    {
        hash = (hash << 4) + c;
        hi = hash & 0xf0000000;
        /*if (hi != 0)
        {
            hash ^= hi >> 24;
            hash ^= hi;
        }*/
        /* ����������� �� uclibc */
        hash ^= hi;
        hash ^= hi >> 24;
        
        c = *name++;
    }
    return hash;
}



 /*
  * ������� � ���������� ��������� �������
  */
Elf32_Word findExport (Elf32_Exec* ex, const char* name, unsigned int hash)
{
    if(!ex || !ex->hashtab) return 0;

    Elf32_Word nbucket = ex->hashtab[0];
    //Elf32_Word nchain = ex->hashtab[1];
    Elf32_Word* bucket = &ex->hashtab[2];
    Elf32_Word* chain = &ex->hashtab[2 + nbucket];
    Elf32_Word func = 0;
    int symidx;

    for (symidx = bucket[__e_div(nbucket, hash)]; symidx != STN_UNDEF; symidx = chain[symidx])
    {
        Elf32_Sym sym = ex->symtab[symidx];

        if (sym.st_value == 0 || sym.st_shndx == SHN_UNDEF)
            continue;

        switch (ELF_ST_TYPE(sym.st_info))
        {
        case STT_NOTYPE:
        case STT_FUNC:
        case STT_OBJECT:
            break;
        default:
            /* Not a code/data definition. */
            continue;
        }

        if (__direct_strcmp(ex->strtab + sym.st_name, name))
            /* Not the symbol we are looking for. */
            continue;

        switch (ELF_ST_BIND(sym.st_info))
        {
            case STB_GLOBAL:
                /* Global definition.  Just what we need. */
                return (Elf32_Word)ex->body + sym.st_value;
            case STB_WEAK:
                /* Weak definition.  Use this value if we don't find another. */
                func = (Elf32_Word)ex->body + sym.st_value;
                break;
        default:
            /* Local symbols are ignored.  */
            break;
        }
    }

    return func;
}



Elf32_Word FindFunction(Elf32_Lib* lib, const char *name, unsigned int hash)
{
    if(!lib) return 0;
    return findExport(lib->ex, name, hash);
}



 /*
  * ���������� ���������� ���������� LD_LIBRARY_PATH
  * ����1;����2;����3;
  */
static char * envparse(const char *str, char *buf, int num)
{
  if( !str || !buf || num < 0) return 0;
  const char *start = str;
  const char *s = strchr(str, ';');
  
  while(num-- && start)
  {
    start = s? s+1:0;
    if(start)
      s = strchr(start, ';');
    else s = 0;
  }
  
  if(!s){
    switch( start ? 1:0 )
    {
      case 0:
        return 0;
      case 1:
        s = str + strlen(str);
    }
  }

  memcpy_a(buf, start, s-start);
  buf[s-start] = 0;
  return buf;
}



 /*
  * ����� ��������� � ������ ���������� ���������
  */
static const char * findShared(const char *name)
{
    const char *env = getenv("LD_LIBRARY_PATH");
    if(!env || !*env) return 0;
    
    for(int i=0;; ++i)
    {
        if( !envparse(env, tmp, i) ) return 0;
        strcat(tmp, name);
        if( get_file_size(tmp) >= 0 )
        {
            return tmp;
        }
    }

#ifndef __ICCARM__
    /* ����� ������� �� ����� */
    return 0;
#endif
}



 /*
  * ��������� � ������ �������� ����������
  */
Elf32_Lib* OpenLib(const char *name, Elf32_Exec *_ex)
{
    if(!name || !*name) return 0;
    printf("Starting loading shared library '%s'...\n", name);
    int fp, _size = 0;
    Elf32_Ehdr ehdr;
    Elf32_Exec* ex;
    char __mem[256];

    // ������ ����� ��� ����������
    Global_Queue* ready_libs = lib_top;
    
    const char *cmp_share_name = strrchr_a(name, PATH_SEPARATOR);
    if(!cmp_share_name) cmp_share_name = name;
    else cmp_share_name++;
    while(ready_libs)
    {
        Elf32_Lib* lib = ready_libs->lib;

        if(!strcmp (lib->soname, cmp_share_name))
        {
            printf(" '%s' is already loaded\n", cmp_share_name);
            lib->users_cnt++;
            memset(dlerr, 0, 2);
            return lib;
        }
        ready_libs = ready_libs->prev;
    }

    
    const char *ld_path = 0;

#ifdef __linux__
    /* ���� � ��� �������� */
    if(name[0] == '/') {
      ld_path = name;
    }
#else
    /* ���� � ��� �������� */
    if(name[1] == ':') {
      ld_path = name;
    }
#endif
    else
    {
      /* ���� � ��� ��������� ��������� */
      if(_ex && _ex->temp_env)
      {
        /* ��������� �� ���� ������� ���������� */
        ld_path = (const char*)__mem;
        sprintf((char *)ld_path, "%s%s", _ex->temp_env, name);
        
        /* ���� �... */
        if(get_file_size(ld_path) < 0)
        {
          /* �� ������ �� ���������� */
          ld_path = findShared(name);
        }
      } else {
        ld_path = findShared(name);
      }
    }
    
    /* ������ �� ����� */
    if(!ld_path) return 0;
    
    
try_again:

    /* ��������� */
    if((fp = fopen(ld_path, A_ReadOnly+A_BIN,P_READ, &ferr)) == -1) {
      strcpy(dlerr, NO_FILEORDIR);
      return 0;
    }
    /* ������ ����� */
    if( (_size = fread(fp, &ehdr, sizeof(Elf32_Ehdr), &ferr)) <= 0) {
      strcpy(dlerr, BADFILE);
      return 0;
    }
    
    /* ��������� �� ��� ������ ����� */
    if( _size < (int) sizeof(Elf32_Ehdr) || CheckElf(&ehdr) ) // �� ����? �_� �� �������?!
    {
      int ns = lseek(fp, 0, S_END, &ferr, &ferr); // ���� ����� ���� ������ 256 ���� �� ������ ����� ����...
      if(ns < 256 && ns > 0)
      {
        lseek(fp, 0, S_SET, &ferr, &ferr);
        if(fread(fp, tmp, ns, &ferr) != ns){
          fclose(fp, &ferr);
          return 0;
        }
        tmp[ns] = 0;
        ld_path = tmp;
        fclose(fp, &ferr);
        goto try_again;
      }
      strcpy(dlerr, BADFILE);
      fclose(fp, &ferr);
      return 0;
    }
    
    /* ������� ������ ��� ��������� ����� */
    if( !(ex = malloc(sizeof(Elf32_Exec))) ) {
      strcpy(dlerr, OUTOFMEM);
      return 0;
    }
    
    memcpy_a(&ex->ehdr, &ehdr, sizeof(Elf32_Ehdr));
    ex->v_addr = (unsigned int)-1;
    ex->fp = fp;
    ex->type = EXEC_LIB;
    ex->libs = 0;
    ex->complete = 0;
    ex->meloaded = (void*)_ex;
    ex->switab = (int *) (pLIB_TOP ? pLIB_TOP : Library);
    ex->fname  = malloc(strlen(ld_path) + 1);
    strcpy(ex->fname, ld_path);
    
    const char *p = strrchr_a(name, '\\');
    if(p)
    {
      ++p;
      ex->temp_env = malloc(p - name + 2);
      memcpy_a(ex->temp_env, name, p - name);
      ex->temp_env[p - name] = 0;
    } else {
        ex->temp_env = 0;
    }

    /* �������� ������ ��������� ���� */
    if( LoadSections(ex) ){
        strcpy(dlerr, BADFILE);
        fclose(fp, &ferr);
        elfclose(ex);
        return 0;
    }

    /* �� ��� �� ����� */
    fclose(fp, &ferr);

    /* ���������� ���� ��� */
    Elf32_Lib* lib;
    if( !(lib = malloc(sizeof(Elf32_Lib))) ){
        elfclose(ex);
        strcpy(dlerr, OUTOFMEM);
        return 0;
    }

    lib->ex = ex;
    lib->users_cnt = 1;
    
    const char *soname;
    
    
    if(!ex->dyn[DT_SONAME]) // ������ ���� � ������ ���� �_�
    {
      if(name[1]==':') // ���� �������������
      {
        soname = strrchr_a(name, '\\'); // ���������� ����, ���� ���
        if(!soname) // �� �� ����� ���� �����?! 
        {
          soname = name; // ���, ��� ...
        }else
          ++soname;
      }else // ���� �� �������������
      {
        soname = name;
      }
    }else // ��� ����, ��� ���� ����
    {
      soname = ex->strtab + ex->dyn[DT_SONAME];
    }
    
    strcpy(lib->soname, soname?soname : "Error Lib!");

    /*  ���� �������^W ������ ���� �����)) */
    Global_Queue* global_ptr = malloc(sizeof(Global_Queue));
    if(!global_ptr)    // ?????...?? ??? :'(
    {
        strcpy(dlerr, OUTOFMEM);
        CloseLib(lib, 0);
        return 0;
    }

    /* �� ��� ��������� */
    global_ptr->lib = lib;
    global_ptr->next = 0;
    lib->glob_queue = global_ptr;

    if(lib_top)
    {
        lib_top->next = global_ptr;
        global_ptr->prev = lib_top;
    }
    else global_ptr->prev = 0;
    
    lib_top = global_ptr;

    /* �������� ������������ */
    run_INIT_Array(ex);
    ex->complete = 1;

    /* �������� �������� ������������� ����, ���� ������� ������� */
    if(ex->dyn[DT_INIT])
    {
        printf("Run library DT_INIT %08X\n", ex->dyn[DT_INIT]);
#ifndef __linux__
        (( void (*)(const char*) )(ex->body + ex->dyn[DT_INIT] - ex->v_addr))(name);
#endif
    }

    printf(" '%s' load complete\n", name);
    dlerr[0] = 0;
    dlerr[1] = 0;
    return lib;
}


 /*
  * ������� ����� ���������� �������� ���
  */
void sub_clients(Elf32_Lib* lib)
{
  lib->users_cnt--;
}



 /*
  * ��������� ���������� � ����������� �������
  */
int CloseLib(Elf32_Lib* lib, int immediate)
{
    if(!lib) return E_EMPTY;

    if(lib->users_cnt < 1) // ���� ������ ������ ���� :(
    {
        if(!config->realtime_libclean && !immediate) goto end;
        
        Elf32_Exec* ex = lib->ex;
        if(ex->dyn[DT_FINI]) {
            printf("Run library DT_FINI %08X\n", ex->dyn[DT_FINI]);
            #ifndef __linux__
            ((LIB_FUNC*)(ex->body + ex->dyn[DT_FINI] - ex->v_addr))();
            #endif
        }
        
        if(lib->glob_queue)
        {
            // ������� �����������
            Global_Queue* glob_queue = lib->glob_queue;

            Global_Queue* tmp = glob_queue->next;
            
            if( glob_queue == lib_top && !lib_top->prev) lib_top = 0;
            else
            if( glob_queue == lib_top ) lib_top = glob_queue->prev;
              
            if(tmp) tmp->prev = glob_queue->prev;
            tmp = glob_queue->prev;
            if(tmp) tmp->next = glob_queue->next;
            mfree(glob_queue);
        }

        elfclose(ex);
        mfree(lib);
    }
end:
    return E_NO_ERROR;
}



 /*
  * POSIX-�������� dlopen
  */
int dlopen(const char *name)
{
  int handle = -1;
  
  if(!name) return -1;
  
  // ������ ������! :)
  if(!handles_cnt)
  {
    handles_cnt = 256;
    handles = malloc(sizeof(Elf32_Lib*) * handles_cnt);
    
    if(!handles) return -1;
    
    memset(handles, 0, sizeof(Elf32_Lib*) * handles_cnt);
  }
  
  // ���� ��������� ����
  for(int i=0; i<handles_cnt; ++i)
  {
    if(handles[i] == 0)
    {
      handle = i;
      break;
    }
  }
  
  // �� ����� O_o
  if(handle == -1)
  {
    Elf32_Lib** new_handles = realloc(handles, sizeof(Elf32_Lib*) * (handles_cnt + 64));
    
    // ����� ���, � ���� ��������� :'(
    if(!new_handles) return -1;
    
    handle = handles_cnt;
    memset(&new_handles[handles_cnt], 0, sizeof(Elf32_Lib*) * 64);
    handles_cnt += 64;
    handles = new_handles;
  }
  
  Elf32_Lib* lib = OpenLib(name, 0);
  if(!lib) return -1;
  
  handles[handle] = lib;
  return handle;
}



 /*
  * POSIX-�������� dlclose
  */
int dlclose(int handle)
{
  if(handle < 0 || handle > handles_cnt - 1 || !handles) return -1;
  
  if(handles[handle])
  {
    Elf32_Lib* lib = handles[handle];
    handles[handle] = 0;
    sub_clients(lib);
    // �� ��� ����� ����� ���������� ���? handle ��� ����� �� �������...
    return CloseLib(lib, 0);
  }
  
  return 0;
}



 /*
  * POSIX-�������� dlsym
  */
Elf32_Word dlsym(int handle, const char *name)
{
  if(handle < 0 || handle > handles_cnt - 1 || !handles) return 0;
  
  if(handles[handle]) {
      unsigned int hash = elfhash(name);
      return FindFunction(handles[handle], name, hash);
  }
  
  return 0;
}



 /*
  * POSIX-�������� dlerror
  */
const char *dlerror()
{
  return dlerr;
}



 /*
  * ����� ���������� �������^W^W �������� ������ ���
  */
void *SHARED_TOP()
{
  return lib_top;
}



 /*
  * ������� �� ������ ���������
  */
int dlclean_cache()
{
  if(!lib_top) return -1;
  
  Elf32_Lib *bigger = 0;
  Global_Queue *tmp = lib_top, *mem = lib_top, *prev = 0;
  int cleaned = 0;
  while(tmp)
  {
    // ������ ���� ������� ����� ���� ����
    bigger = tmp->lib;
    prev = tmp->prev;
    
    if( bigger->users_cnt < 1 )
    {
      // ������� �, � ��� ������� ���� ���� ������� ���� �����
      CloseLib(bigger, 1); // ��������� ����� ��!
      ++cleaned;
    }
    
    // ��� � ��� ����������, �� ��� ���������, ������
    if(mem != lib_top )
    {
      tmp = lib_top;
      mem = lib_top;
    }
    else // �� �����������, ���� ������ �����
      tmp = prev;
  }
  
  return cleaned;
}




