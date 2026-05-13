//
// Created by kuroneko on 5/13/26.
//

#include "log.h"

#include <stdarg.h>

#include <SDL.h>

void SYS_LogError(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SYS_LogErrorV(fmt, ap);
    va_end(ap);
}

void SYS_LogErrorV(const char *fmt, va_list ap)
{
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, ap);
}

void SYS_Log(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    SYS_LogV(fmt, ap);
    va_end(ap);
}

void SYS_LogV(const char *fmt, va_list ap)
{
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, ap);
}

