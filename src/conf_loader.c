#include "loader3/loader.h"
#include <inc/cfg_items.h>
#include "conf_loader.h"
#include "config_struct.h"

static char default_config_disk = 0;
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

char BCFG_GetDefaultDisk() {
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

int BCFG_SaveConfig(const char *config_file, void *cfg, uint32_t cfg_size) {
    unsigned int err;
    int fd = fopen(config_file, A_ReadWrite + A_Create + A_Truncate, P_READ + P_WRITE, &err);
    int result = -1;
    if (fd >= 0) {
        if (fwrite(fd, cfg, cfg_size, &err) == cfg_size)
            result = 0;
        fclose(fd, &err);
    }
    return result;
}

int BCFG_LoadConfig(const char *config_file, void *cfg, uint32_t cfg_size) {
    unsigned int err;
    int fd = fopen(config_file, A_ReadOnly + A_BIN, P_READ, &err);
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

    char config_path[] = "?:\\ZBin\\etc\\ElfPack.bcfg";
    config_path[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(config_path, config, sizeof(config_structure_t)) != 0) {
        memcpy_a(config, &config_structure, sizeof(config_structure));
        _SyncDefaultDisk(config, config_path[0]);
        BCFG_SaveConfig(config_path, config, sizeof(config_structure_t));
    }
}
