#include "loader.h"
#include "../config_struct.h"

void ep_log(Elf32_Exec *ex, const char *data, int size)
{
#ifdef __linux__
    printf("[ep_log] %s\n", data);
#else
    if(!*config->ELFLOADER_LOG_PATH || !config->max_log_size) return;
    
    unsigned int err;
    int log_size;
    TDate td;
    TTime tt;
    int fp = fopen(config->ELFLOADER_LOG_PATH, A_Create | A_Append | A_BIN | A_WriteOnly, P_WRITE, &err);
    if(fp == -1) return;
    
    
    int mem = lseek(fp, 0, S_CUR, &err, &err);
    log_size = lseek(fp, 0, S_END, &err, &err);
    lseek(fp, mem, S_SET, &err, &err);
 
    char *_data = malloc( strlen(ex->fname) + size + 128 );
    
    GetDateTime(&td, &tt);
    int sz = sprintf(_data, " [%d.%02d.%02d %02d:%02d:%02d] %s: %s\n",
                     td.year, td.month, td.day,
                     tt.hour, tt.min, tt.sec,
                     ex->fname, data );
    
    if(log_size+sz > (int) config->max_log_size) {
      setfilesize(fp, 0, &err);
    }
    
    fwrite(fp, _data, sz, &err);
    fclose(fp, &err);
    mfree(_data);
#endif
}

