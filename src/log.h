//
// Created by kuroneko on 5/13/26.
//

#ifndef STRANGE_LOG_H
#define STRANGE_LOG_H

#include <stdarg.h>

extern void SYS_LogError(const char *fmt, ...);
extern void SYS_LogErrorV(const char *fmt, va_list ap);

extern void SYS_Log(const char *fmt, ...);
extern void SYS_LogV(const char *fmt, va_list ap);
#endif //STRANGE_LOG_H
