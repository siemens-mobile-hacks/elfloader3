#ifndef __linux__

#include <stdarg.h>

#include "loader.h"

#define DTRACE_MAX_SIZE 0x800
#define DTRACE_CHANNEL 1
#define DTRACE_FLAGS 0x88

extern __arm void DTrace_WriteRaw(const void *data, unsigned int length,
                                  unsigned int channel, unsigned int flags,
                                  unsigned int already_locked);

__arm void DTrace_Write(const void *data, unsigned int length)
{
    if (data && length)
        DTrace_WriteRaw(data, length, DTRACE_CHANNEL, DTRACE_FLAGS, 0);
}

__arm void DTrace_Printf(const char *format, ...)
{
    char *buffer;
    va_list args;
    int length;

    if (!format)
        return;

    buffer = malloc(DTRACE_MAX_SIZE + 1);
    if (!buffer)
        return;

    va_start(args, format);
    length = vsnprintf(buffer, DTRACE_MAX_SIZE + 1, format, args);
    va_end(args);

    if (length > 0) {
        if (length > DTRACE_MAX_SIZE)
            length = DTRACE_MAX_SIZE;

        DTrace_Write(buffer, length);
    }
    mfree(buffer);
}

#endif
