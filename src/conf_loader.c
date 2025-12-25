#include "loader3/loader.h"
#include <inc/cfg_items.h>
#include "conf_loader.h"
#include "config_struct.h"

static int default_config_disk = 0;
config_structure_t *config = NULL;

// Обновляем диск у всех путей, чтобы юзеру не приходилось делать это вручную
static inline void _SyncDefaultDisk(config_structure_t *config, char disk) {
  config->IMAGES_PATH[0] = disk;
  config->DAEMONS_PATH[0] = disk;
  config->SWIBLIB_PATH[0] = disk;
  config->ELFLOADER_LOG_PATH[0] = disk;
}

static char _FindDiskForPath(char *path) {
    #ifdef NEWSGOLD
    const char disks[] = { '4', '0', '2', '1' };
    #else
    const char disks[] = { '4', '0', 'b', 'a' };
    #endif
    FSTATS fs;
    unsigned int err;

    for (int i = 0; i < (sizeof(disks) / sizeof(disks[0])); i++) {
        path[0] = disks[i];
        if (GetFileStats(path, &fs, &err) >= 0)
            return disks[i];
    }

    return 0;
}

static int _GetDefaultDisk() {
    if (!default_config_disk) {
        char config_path[] = "?:\\ZBin\\etc\\ElfPack.bcfg";
        int disk = _FindDiskForPath(config_path);
        if (!disk) {
            char zbin_etc_path[] = "?:\\ZBin\\etc";
            disk = _FindDiskForPath(zbin_etc_path);
            if (!disk) {
                char zbin_path[] = "?:\\ZBin";
                disk = _FindDiskForPath(zbin_path);
            }
        }
        default_config_disk = disk ? disk : '0';
    }
    return default_config_disk;
}

static char *_FindConfigFile(const char *config_file_mask) {
    if (config_file_mask[0] != '?')
        return NULL;
    unsigned int len = strlen(config_file_mask);
    char *config_file = malloc(len + 1);
    memcpy_a(config_file, config_file_mask, len + 1);
    config_file[0] = _GetDefaultDisk();
    return config_file;
}

int BCFG_SaveConfig(const char *config_file_mask, void *cfg, uint32_t cfg_size) {
    char *found_config_file = _FindConfigFile(config_file_mask);
    unsigned int err;
    int fd = fopen(found_config_file ? found_config_file : config_file_mask, A_ReadWrite + A_Create + A_Truncate, P_READ + P_WRITE, &err);
    if (found_config_file)
        mfree(found_config_file);

    int result = -1;
    if (fd >= 0) {
        if (fwrite(fd, cfg, cfg_size, &err) == cfg_size)
            result = 0;
        fclose(fd, &err);
    }
    return result;
}

int BCFG_LoadConfig(const char *config_file_mask, void *cfg, uint32_t cfg_size) {
    char *found_config_file = _FindConfigFile(config_file_mask);
    unsigned int err;
    int fd = fopen(found_config_file ? found_config_file : config_file_mask, A_ReadOnly + A_BIN, P_READ, &err);
    if (found_config_file)
        mfree(found_config_file);

    int result = -1;
    if (fd >= 0) {
        int file_size = lseek(fd, 0, S_END, &err, &err);
        if (file_size == cfg_size) {
            lseek(fd, 0, S_SET, &err, &err);
            if (fread(fd, cfg, cfg_size, &err) == cfg_size)
                result = 0;
        }
        fclose(fd, &err);
    }
    return result;
}

void InitConfig() {
#ifdef USE_STATIC_MEMORY
    // Если памяти завались, юзаем статическую аллокацию
    static config_structure_t config_storage = { 0 };
    config = &config_storage;
#else
    // Иначе тратим хип
    config = malloc(sizeof(config_structure_t));
#endif

    const char config_path[] = "?:\\ZBin\\etc\\ElfPack.bcfg";
    if (BCFG_LoadConfig(config_path, config, sizeof(config_structure_t)) != 0) {
        memcpy_a(config, &config_structure, sizeof(config_structure));
        _SyncDefaultDisk(config, _GetDefaultDisk());
        BCFG_SaveConfig(config_path, config, sizeof(config_structure_t));
    }
}
