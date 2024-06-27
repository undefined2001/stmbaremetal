#include "debug.h"

void kprintf(const char *format, ...)
{
    char buffer[255];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UartSendBuffer(buffer);
}