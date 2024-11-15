
/*
 * ���� ���� �������� ������ ��������� ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */


#include "loader.h"

// �������� �����
Elf32_Exec* elfopen(const char* filename)
{
  int fp;
  Elf32_Ehdr ehdr;
  Elf32_Exec* ex;

  if((fp = fopen(filename, A_ReadOnly | A_BIN,P_READ,&ferr)) == -1) return 0;

  if(fread(fp, &ehdr, sizeof(Elf32_Ehdr), &ferr) == sizeof(Elf32_Ehdr))
  {
    if( !CheckElf(&ehdr) )
    {
      ex = malloc(sizeof(Elf32_Exec));

      if(ex)
      {
        memcpy_a(&ex->ehdr, &ehdr, sizeof(Elf32_Ehdr));
        ex->v_addr = (unsigned int)-1;
        ex->fp = fp;
        ex->body = 0;
        ex->type = EXEC_ELF;
        ex->libs = 0;
        ex->hashtab = 0;
        ex->complete = 0;
        ex->__is_ex_import = 0;
        ex->meloaded = 0;
        ex->switab = (int *) (pLIB_TOP ? pLIB_TOP : Library);
        ex->fname = malloc(strlen(filename) + 1);
        strcpy(ex->fname, filename);
        
        const char *p = strrchr_a(filename, PATH_SEPARATOR); 
        if(p)
        {
          ++p;
          ex->temp_env = malloc(p - filename + 2);
          memcpy_a(ex->temp_env, filename, p - filename);
          ex->temp_env[p - filename] = 0;
        } else {
          ex->temp_env = 0;
        }
        
        if(!LoadSections(ex))
        {
          ex->complete = 1;
          fclose(fp, &ferr);
          return ex;
        }
        else
            elfclose(ex);
      }
    }
  }
  fclose(fp, &ferr);
  return 0;
}


void *elf_entry(Elf32_Exec *ex)
{
    if(!ex) return 0;
    return ( ex->body + ex->ehdr.e_entry - ex->v_addr );
}


int elfclose(Elf32_Exec* ex)
{
  if(!ex) return E_EMPTY;

  if(ex->complete)
    run_FINI_Array(ex);
  
  // ��������� ����
  while(ex->libs)
  {
    Libs_Queue* lib = ex->libs;
    sub_clients(lib->lib);
    CloseLib(lib->lib, 0);
    ex->libs = lib->next;
    mfree(lib);
  }

  if (elfloader_debug_hook && ex->__is_ex_import) {
    elfloader_debug_hook->hook(ELFLOADER_DEBUG_HOOK_UNLOAD, ex);
  }

  if(ex->fname) mfree(ex->fname);
  if(ex->body) mfree(ex->body);
  if(ex->temp_env) mfree(ex->temp_env);
  mfree(ex);
  return E_NO_ERROR;
}


int sub_elfclose(Elf32_Exec* ex)
{
  //elfclose(ex);
  SUBPROC_a((void*)elfclose, ex);
  return 0;
}


