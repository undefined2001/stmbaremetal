#ifndef _INC_DEBUG_H
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>

void kprintf(const char *format, ...);

#define LOG(format, ...) kprintf(format, ##__VA_ARGS__)

#define _INC_DEBUG_H
#endif