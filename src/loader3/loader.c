/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */
#include "loader.h"
#include "../config_struct.h"

ElfloaderDebugHook *elfloader_debug_hook = NULL;

unsigned int ferr;

// Проверка валидности эльфа
int CheckElf(Elf32_Ehdr *ehdr)
{
    if(memcmp_a(ehdr, elf_magic_header, sizeof(elf_magic_header))) return E_HEADER;
    if(ehdr->e_machine != EM_ARM) return E_MACHINE;

    return E_NO_ERROR;
}


// Получение нужного размера в раме
unsigned int GetBinSize(Elf32_Exec *ex, Elf32_Phdr* phdrs)
{
    unsigned int i = 0;
    unsigned long maxadr=0;
    unsigned int end_adr;

    while (i < ex->ehdr.e_phnum)
    {
        Elf32_Phdr phdr = phdrs[i];

        if (phdr.p_type == PT_LOAD)
        {
            if (ex->v_addr > phdr.p_vaddr) ex->v_addr = phdr.p_vaddr;
            end_adr = phdr.p_vaddr + phdr.p_memsz;
            if (maxadr < end_adr) maxadr = end_adr;
        }
        ++i;
    }
    return maxadr - ex->v_addr;
}

#pragma optimize=no_inline
static char *LoadData(Elf32_Exec* ex, int offset, int size)
{
    if(size && lseek(ex->fp, offset - ex->v_addr, S_SET, &ferr, &ferr))
    {
        char *data = malloc(size+1);
        if(data && fread(ex->fp, data, size, &ferr) == size)
        {
            data[size] = 0;
            return data;
        }
        else if (data) {
            mfree(data);
        }
    }

    return 0;
}

/* Вспомогательная функция */
#pragma optimize=no_inline
static unsigned int _look_sym(Elf32_Exec *ex, const char *name, unsigned int hash)
{
    Libs_Queue *lib = ex->libs;
    unsigned int func = 0;
    while(lib && !func)
    {
        func = (unsigned int)FindFunction(lib->lib, name, hash);
        lib = lib->next;
    }
    return func;
}


/* функция пролетается рекурсивно по либам которые в зависимостях */
#pragma optimize=no_inline
static unsigned int try_search_in_base(Elf32_Exec* ex, const char *name, unsigned int hash, int bind_type)
{
    printf("Searching '%s' in libs...\n", name);
    unsigned int address = 0;

    if(ex->type == EXEC_LIB && !ex->dyn[DT_SYMBOLIC])
        address = findExport(ex, name, hash);

    if(!address)
        address = (unsigned int)_look_sym(ex, name, hash);

    if( !address )
    {
        if(!address && ex->meloaded)
        {
            Elf32_Exec *mex = (Elf32_Exec*)ex->meloaded;
            while(mex && !address && mex->type == EXEC_LIB)
            {
                address = findExport(mex, name, hash);
                mex = (Elf32_Exec*)mex->meloaded;
            }
        }
    }
    return address;
}

#pragma optimize=no_inline
static bool ResolveBuiltinSymbols(Elf32_Exec *ex, unsigned int *addr, const char *name, unsigned int hash) {
    switch (hash) {
        case 0x000655C8:
            /* запросили указатель на ELF */
            if (__direct_strcmp(name, "__ex") == 0) {
                ex->__is_ex_import = 1;
                *addr = (unsigned int)ex;
                return true;
            }
        break;
        case 0x0AB11643:
            /* запросили указатель на таблицу функций (legacy) */
            if (__direct_strcmp(name, "__sys_switab_addres") == 0) {
                *addr = (unsigned int)ex->switab;
                return true;
            }
        break;
        case 0x06AE0C22:
            /* запросили указатель на таблицу функций (modern) */
            if (__direct_strcmp(name, "__switab") == 0) {
                *addr = (unsigned int)&pLIB_TOP;
                return true;
            }
        break;
    }
    return false;
}

// Релокация
static int DoRelocation(Elf32_Exec* ex, Elf32_Phdr* phdr, bool is_iar_elf)
{
    unsigned int i = 0;
    Elf32_Word libs_needed[64];
    unsigned int libs_cnt = 0;
    char dbg[128];

    if (!ex->dynamic)
        return E_DYNAMIC;

    // Вытаскиваем теги
    while (ex->dynamic[i].d_tag != DT_NULL)
    {
        if (ex->dynamic[i].d_tag <= DT_FLAGS)
        {
            switch(ex->dynamic[i].d_tag)
            {
            case DT_SYMBOLIC:
                // Флаг SYMBOLIC-библиотек. В d_val 0, даже при наличии :(
                ex->dyn[ex->dynamic[i].d_tag] = 1;
                break;
            case DT_DEBUG:
                // Для GDB
                ex->dynamic[i].d_un.d_val = (int32_t) (elfloader_debug_hook ? elfloader_debug_hook->_r_debug : NULL);
                break;
            case DT_NEEDED:
                // Получаем смещения в .symtab на имена либ
                libs_needed[libs_cnt++] = ex->dynamic[i].d_un.d_val;
                break;
            default:
                ex->dyn[ex->dynamic[i].d_tag] = ex->dynamic[i].d_un.d_val;
                break;
            }
        }
        ++i;
    }

    // Таблички. Нужны только либам, и их юзающим)
    ex->symtab = ex->dyn[DT_SYMTAB]? (Elf32_Sym*)(ex->body + ex->dyn[DT_SYMTAB] - ex->v_addr) : 0;
    ex->jmprel = (Elf32_Rel*)(ex->body + ex->dyn[DT_JMPREL] - ex->v_addr);
    ex->strtab = ex->dyn[DT_STRTAB]? ex->body + ex->dyn[DT_STRTAB] - ex->v_addr : 0;
    ex->hashtab = NULL;

    printf("STRTAB: %08X\n", ex->dyn[DT_STRTAB]);
    printf("SYMTAB: %08X\n", ex->dyn[DT_SYMTAB]);
    printf("HASHTAB: %08X\n", ex->dyn[DT_HASH]);

    if(ex->type == EXEC_LIB)
    {
        ex->hashtab = (Elf32_Word *) (ex->dyn[DT_HASH] ? ex->body + ex->dyn[DT_HASH] - ex->v_addr : 0);
        if (!ex->hashtab)
        {
            ep_log(ex, "Hash tab is mising", 18);
            return E_HASTAB;
        }
    }

    // Загрузка библиотек
    for(i=0; i < libs_cnt; ++i)
    {
        char *lib_name = ex->strtab + libs_needed[i];
        Elf32_Lib* lib;
        if( (lib = OpenLib(lib_name, ex)) )
        {
            Libs_Queue* libq = malloc(sizeof(Libs_Queue));
            libq->lib = lib;

            if(ex->libs) libq->next = ex->libs;
            else libq->next = 0;

            ex->libs = libq;
        }
        else
        {
            int csz = sprintf(dbg, "Can't load %s!", lib_name);
            l_msg(1, (int)dbg);
            ep_log(ex, dbg, csz);
            return E_SHARED;
        }
    }

    // Релокация
    if (ex->dyn[DT_RELSZ])
    {
        i=0;
        unsigned int* addr;
        char* name = NULL;
        Elf32_Word r_type;
        unsigned int func = 0;
        int symtab_index = 0;
        int bind_type = 0;
        int reloc_type = 0;

        // Таблица релокаций
        Elf32_Rel* reltab;
        if (is_iar_elf) {
            uint8_t *dynamic_ptr = (uint8_t *) ex->dynamic;
            reltab = (Elf32_Rel *) (dynamic_ptr + ex->dyn[DT_REL]);
        } else {
            reltab = (Elf32_Rel *) (ex->body + ex->dyn[DT_REL] - ex->v_addr);
        }

        if(!reltab)
        {
            elfclose(ex);
            return E_RELOCATION;
        }

        while(i * sizeof(Elf32_Rel) < ex->dyn[DT_RELSZ])
        {
            r_type = ELF32_R_TYPE(reltab[i].r_info);
            symtab_index = ELF32_R_SYM(reltab[i].r_info);
            Elf32_Sym *sym = ex->symtab? &ex->symtab[symtab_index] : 0;
            bind_type = sym ? ELF_ST_BIND(sym->st_info) : 0;
            reloc_type = sym ? ELF_ST_TYPE(sym->st_info) : 0;
            addr = (unsigned int*)(ex->body + reltab[i].r_offset - ex->v_addr);

            switch(r_type)
            {
            case R_ARM_NONE:
                break;
            case R_ARM_RABS32:
                printf("R_ARM_RABS32\n");
                *addr += (unsigned int)(ex->body - ex->v_addr);
                break;
            case R_ARM_ABS32:
                printf("R_ARM_ABS32\n");

                if( !ex->symtab )
                {
                    int csz = sprintf(dbg, "warning: symtab not found, but relocation R_ARM_ABS32 is exist");
                    if(config->loader_warnings)
                      ep_log(ex, dbg, csz);
                    *addr = (unsigned int)ex->body;
                    break;
                }

                if( !ex->strtab )
                {
                    int csz = sprintf(dbg, "warning: symtab not found, but relocation R_ARM_ABS32 is exist");
                    if(config->loader_warnings)
                      ep_log(ex, dbg, csz);
                    
                    *addr = (unsigned int)ex->body;
                    break;
                }

                /* на всякий случай, вдруг сум пустой будет */
                if(sym)
                {
                    /* имя требуемой функции */
                    name = ex->strtab + sym->st_name;

                    /* сразу посчитаем хэш */
                    unsigned int hash = elfhash(name);

                    /* сначала проверим на встроенные EP3-символы */
                    if (ResolveBuiltinSymbols(ex, &func, name, hash)) {
                        *addr += func;
                        break;
                    }

                    switch(reloc_type)
                    {
                    case STT_NOTYPE:
                        if(bind_type != STB_LOCAL)
                           func = (unsigned int)ex->body + sym->st_value;
                        else
                           func = sym->st_value;
                        goto skeep_err;

                    default:
                        if(sym->st_value)
                            func = (unsigned int)ex->body + sym->st_value;
                        else
                            func = try_search_in_base(ex, name, hash, bind_type);
                        break;
                    }

                }
                else
                {
                    func = 0;
                }

                /* ничего не нашли, жаль */
                if(!func && bind_type != STB_WEAK)
                {
                    int csz = sprintf(dbg, "[2] Undefined reference to `%s'\n", name?name : "");
                    ep_log(ex, dbg, csz);
                    return E_UNDEF;
                }

skeep_err:

                /* в ABS32 релоке в *addr всегда должен быть 0 */
                *addr += func;
                break;
            case R_ARM_RELATIVE:
                printf("R_ARM_RELATIVE\n");
                *addr += (unsigned int)(ex->body - ex->v_addr);
                break;

            case R_ARM_GLOB_DAT:
            case R_ARM_JUMP_SLOT:
                printf("R_ARM_GLOB_DAT\n");

                if( !ex->symtab )
                {
                    int csz = sprintf(dbg, "Relocation R_ARM_GLOB_DAT cannot run without symtab\n");
                    ep_log(ex, dbg, csz);
                    return E_SYMTAB;
                }

                if( !ex->strtab )
                {
                    int csz = sprintf(dbg, "Relocation R_ARM_GLOB_DAT cannot run without strtab\n");
                    ep_log(ex, dbg, csz);
                    return E_STRTAB;
                }

                name = sym ? ex->strtab + sym->st_name : NULL;
                printf(" strtab: '%s' \n", name);

                if( symtab_index && name )
                {
                    unsigned int hash = elfhash(name);

                    /* сначала проверим на встроенные EP3-символы */
                    if (ResolveBuiltinSymbols(ex, &func, name, hash)) {
                        *addr = func;
                        break;
                    }

                    switch(reloc_type)
                    {
                    case STT_NOTYPE:
                        if(bind_type != STB_LOCAL)
                           func = (unsigned int)ex->body + sym->st_value;
                        else
                           func = sym->st_value;
                        goto skeep_err1;
                    default:
                        if(sym->st_value)
                            func = (unsigned int)ex->body + sym->st_value;
                        else
                        {
                            func = try_search_in_base(ex, name, hash, bind_type);
                        }
                        break;
                    }


                    if(!func && bind_type != STB_WEAK)
                    {
                        int csz = sprintf(dbg, "[2] Undefined reference to `%s'\n", name?name : "");
                        //l_msg(1, (int)dbg);
                        ep_log(ex, dbg, csz);
                        return E_UNDEF;
                    }

skeep_err1:

                    /* В доках написано что бинды типа STB_WEAK могут быть нулевыми */
                    *addr = func;

                    if(*addr)
                    {
                        printf("found at 0x%X\n", *addr);
                    }
                    else
                    {
                        printf("warning: address is zero\n");
                    }
                }
                else {
                    *addr = sym->st_value;
                }

                break;
            case R_ARM_COPY:
                printf("R_ARM_COPY\n");
                memcpy_a((void *) addr,
                         (void *)(ex->body + sym->st_value), sym->st_size);
                break;

            /* хз чо за релок, ни в одном лоадере его не встречал, 
             * хотя по описанию похож на R_ARM_REL32.
             * Но, если выполнить релокацию по описанию - эльфятник падает.
             * С пропуском - работает, так что хайтек.
             */
            case R_ARM_THM_RPC22:
                printf("R_ARM_THM_RPC22 stub\n");
                break;

                // S + A - P
            case R_ARM_REL32:
                printf("R_ARM_REL32\n");
                *addr += sym->st_value - (unsigned int)addr;
                break;

            default:
                printf("unknow relocation type '%d'\n", r_type);
                int csz = sprintf(dbg, "Fatal error! Unknown type relocation '%d'!\n", r_type);
                //l_msg(1, (int)dbg);
                ep_log(ex, dbg, csz);
                return E_RELOCATION;
            }
            ++i;
        }
    }

    // Биндим функции
    if(ex->dyn[DT_PLTRELSZ])
    {
        i=0;
        while(i * sizeof(Elf32_Rel) < ex->dyn[DT_PLTRELSZ])
        {
            int sym_idx = ELF32_R_SYM(ex->jmprel[i].r_info);
            char* name = ex->strtab + ex->symtab[sym_idx].st_name;
            Elf32_Sym *sym = &ex->symtab[sym_idx];
            Elf32_Word func = 0;
            int bind_type = ELF_ST_BIND(sym->st_info);


            unsigned int hash = elfhash(name);
            func = try_search_in_base(ex, name, hash, bind_type);
            if(!func && bind_type != STB_WEAK)
            {
                int csz = sprintf(dbg, "[3] Undefined reference to `%s'\n", name);
                //l_msg(1, (int)dbg);
                ep_log(ex, dbg, csz);
                return E_UNDEF;
            }

            *((Elf32_Word*)(ex->body + ex->jmprel[i].r_offset)) = func;
            ++i;
        }
    }

    return E_NO_ERROR;
}


// Чтение сегментов из файла
int LoadSections(Elf32_Exec* ex)
{
    Elf32_Phdr* phdrs = malloc(sizeof(Elf32_Phdr) * ex->ehdr.e_phnum);
    if(!phdrs) return E_SECTION;

    unsigned int hdr_offset = ex->ehdr.e_phoff;
    int i = 0;

    unsigned long maxadr=0;
    unsigned int end_adr;

    // Читаем заголовки
    while(i < ex->ehdr.e_phnum)
    {
        if(lseek(ex->fp, hdr_offset, S_SET, &ferr, &ferr) == -1) break;
        if(fread(ex->fp, &phdrs[i], sizeof(Elf32_Phdr), &ferr) != sizeof(Elf32_Phdr))
        {
            /* кривой заголовок, шлём нафиг этот эльф */
            mfree(ex->body);
            ex->body = 0;
            mfree(phdrs);
            return E_PHDR;
        }

        /* тут же и размер бинарника посчитаем */
        if (phdrs[i].p_type == PT_LOAD)
        {
            if (ex->v_addr > phdrs[i].p_vaddr) ex->v_addr = phdrs[i].p_vaddr;
            end_adr = phdrs[i].p_vaddr + phdrs[i].p_memsz;
            if (maxadr < end_adr) maxadr = end_adr;
        }

        hdr_offset += ex->ehdr.e_phentsize;
        ++i;
    }

    ex->bin_size = maxadr - ex->v_addr;

    if(i == ex->ehdr.e_phnum) // Если прочитались все заголовки
    {
        //ex->bin_size = GetBinSize(ex, phdrs);

        if((ex->body = malloc(ex->bin_size+1))) // Если хватило рамы
        {
            memset(ex->body, 0, ex->bin_size+1);
            memset(ex->dyn, 0, sizeof(ex->dyn));

            for(i=0; i < ex->ehdr.e_phnum; ++i)
            {
                Elf32_Phdr phdr = phdrs[i];

                switch (phdr.p_type)
                {
                case PT_LOAD:
                    if(phdr.p_filesz == 0) break; // Пропускаем пустые сегменты
                    printf("PT_LOAD: %X - %X\n", phdr.p_offset, phdr.p_filesz);
                    if(lseek(ex->fp, phdr.p_offset, S_SET, &ferr, &ferr) != -1)
                    {
                         if(fread(ex->fp, ex->body + phdr.p_vaddr - ex->v_addr, phdr.p_filesz, &ferr) == (int) phdr.p_filesz)
                            break;
                    }

                    // Не прочитали сколько нужно
                    mfree(ex->body);
                    ex->body = 0;
                    mfree(phdrs);
                    return E_SECTION;

                case PT_DYNAMIC:
                    if(phdr.p_filesz == 0) break; // Пропускаем пустые сегменты

                    bool is_iar_elf = false;
                    if (!phdr.p_memsz) {
                        // Эльфы IAR почему-то не загружают PT_DYNAMIC через PT_LOAD, хотя по стандарту это невозможно
                        printf("[IAR] Load data dynamic segment: %d - %d\n", phdr.p_offset, phdr.p_filesz);
                        ex->dynamic = (Elf32_Dyn *) LoadData(ex, phdr.p_offset, phdr.p_filesz);
                        is_iar_elf = true;
                    } else {
                        // Нормальные ELF'ы
                        ex->dynamic = (Elf32_Dyn *) (ex->body + phdr.p_vaddr - ex->v_addr);
                    }

                    // Выполним релокации
                    int ret = DoRelocation(ex, &phdr, is_iar_elf);

                    // Освободим ex->dynamic, для IAR-эльфов от него нет толку
                    if (is_iar_elf && ex->dynamic) {
                        mfree(ex->dynamic);
                        ex->dynamic = NULL;
                    }

                    // Ролакции прошли успешно
                    if (ret == 0)
                        break;

                    // Если что-то пошло не так...
                    mfree(ex->body);
                    ex->body = NULL;
                    mfree(phdrs);
                    return E_SECTION;
                }
            }

            if (elfloader_debug_hook && ex->__is_ex_import) {
                elfloader_debug_hook->hook(ELFLOADER_DEBUG_HOOK_LOAD, ex);
            }

            mfree(phdrs);
            return E_NO_ERROR;
        }
    }

    mfree(ex->body);
    ex->body = 0;
    mfree(phdrs);
    return E_RAM;
}


/* constructors */
void run_INIT_Array(Elf32_Exec *ex)
{
    if(!ex->dyn[DT_INIT_ARRAY]) return;
    size_t sz = ex->dyn[DT_INIT_ARRAYSZ] / sizeof (void*);
    void ** arr = (void**)(ex->body + ex->dyn[DT_INIT_ARRAY] - ex->v_addr);

    printf("init_array sz: %d\n", sz);

    for(size_t i=0; i < sz; ++i)
    {
        printf("init %d: 0x%p\n", i, arr[i]);
#ifndef __linux__
        ( (void (*)())arr[i])();
#endif
    }
}


/* destructors */
void run_FINI_Array(Elf32_Exec *ex)
{
    if(!ex->dyn[DT_FINI_ARRAY]) return;
    size_t sz = ex->dyn[DT_FINI_ARRAYSZ] / sizeof (void*);
    void ** arr = (void**)(ex->body + ex->dyn[DT_FINI_ARRAY] - ex->v_addr);

    printf("fini_array sz: %d\n", sz);

    for(size_t i=0; i < sz; ++i)
    {
        printf("fini %d: 0x%p\n", i, arr[i]);
#ifndef __linux__
        ( (void (*)())arr[i])();
#endif
    }
}
