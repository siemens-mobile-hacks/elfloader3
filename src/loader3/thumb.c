#include "loader.h"

__arm void l_msg(int flags, int msg) {
    ShowMSG(flags, msg);
}

__arm char * strrchr_a (const char *s, int c) {
    return strrchr(s, c);
}

__arm void *memcpy_a(void *dest, const void *src, size_t size) {
    return memcpy(dest, src, size);
}

__arm int memcmp_a (const void *m1, const void *m2, size_t n) {
    return memcmp(m1, m2, n);
}

__arm void SUBPROC_a(void *elf, void *param) {
    SUBPROC(elf, param);
}
