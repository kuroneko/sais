/* Strange Adventures in Infinite Space
 * Copyright (C) 2005 Richard Carlson, Iikka Keranen and William Sears
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
// ----------------
//     INCLUDES
// ----------------

#include <vector>

#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <physfs.h>
#include <SDL.h>

#include "Typedefs.h"
#include "gfx.h"
#include "is_fileio.h"
#include "combat.h"
#include "starmap.h"

IS_FileHdl logfile;
int last_logdate;

char moddir[256];

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void IS_Close(IS_FileHdl fileHandle) {
    if (!PHYSFS_close(fileHandle)) {
        auto physfsErr = PHYSFS_getLastErrorCode();
        SDL_Log("Error trying to close file: %s", PHYSFS_getErrorByCode(physfsErr));
    }
}

int read_line(IS_FileHdl in, char *out1, char *out2) {
    char c;
    char *sptr;
    int flag;
    int end;
    int readStatus;

    // flag
    // 0: start of line
    // 1: get command symbol
    // 2: between command and params
    // 3: get params

    out1[0] = out2[0] = 0;

    end = 0;
    flag = 0;
    sptr = out1;
    while (!end) {
        readStatus = IS_Read(&c, 1, 1, in);
        if (readStatus < 1 || (c < 32 && flag > 0)) {
            // end of line or an error
            *sptr = 0;
            if (!IS_EOF(in)) {
                end = 1;
            } else {
                end = 2;
            }
        } else {
            if (flag == 0 && c > 32) {
                flag = 1;
            }
            if (flag == 1 && c <= 32) {
                flag = 2;
            }
            if (flag == 2 && c > 32) {
                flag = 3;
                *sptr = 0;
                sptr = out2;
            }
            if (flag & 1) {
                *sptr++ = c;
            }
        }
    }

    //		printf("%s - %s\n", out1, out2);

    return end - 1;
}

int read_line1(IS_FileHdl in, char *out1) {
    char c;
    char *sptr;
    int flag;
    int end;
    int readStatus;

    // flag
    // 0: start of line
    // 1: get command symbol
    // 2: between command and params
    // 3: get params

    out1[0] = 0;

    end = 0;
    flag = 0;
    sptr = out1;
    while (!end) {
        readStatus = IS_Read(&c, 1, 1, in);
        if ((c < 32 && flag > 0) || readStatus < 1)  // end of line
        {
            *sptr = 0;
            if (!IS_EOF(in)) {
                end = 1;
            } else {
                end = 2;
            }
        } else {
            if (flag == 0 && c > 32) {
                flag = 1;
            }
            if (flag & 1) {
                *sptr++ = c;
            }
        }
    }

    //		printf("%s - %s\n", out1, out2);

    return end - 1;
}

void ik_start_log() {
    int n;
    IS_FileHdl fil;
    char fname[32];

    logfile = nullptr;

    n = 0;
    while (n < 1000) {
        sprintf(fname, "log%04d.txt", n);
        if (!IS_exists(fname)) {
            logfile = IS_Open_Write(fname);
//			ik_save_tga(fname, img, pal);
            break;
        }
        n++;
    }
    last_logdate = -1;
}

int
IS_Printf(IS_FileHdl fileHdl, const char *format, ...)
{
    size_t lineOutLen;
    va_list ap;
    va_list ap2;
    va_start(ap, format);
    va_copy(ap2, ap);
    lineOutLen = vsnprintf(nullptr, 0, format, ap);
    va_end(ap);

    // build the buffer. - must include 1 extra for NUL storage
    std::vector<char>   bufOut(lineOutLen+1);
    vsnprintf(bufOut.data(), lineOutLen, format, ap2);
    va_end(ap2);

    return IS_Write(bufOut.data(), lineOutLen, 1, fileHdl);
}

int
IS_VPrintf(IS_FileHdl fileHdl, const char *format, va_list arglist)
{
    size_t lineOutLen;
    va_list ap2;
    va_copy(ap2, arglist);
    lineOutLen = vsnprintf(nullptr, 0, format, ap2);
    va_end(ap2);
    // build the buffer. - must include 1 extra for NUL storage
    std::vector<char>   bufOut(lineOutLen+1);
    vsnprintf(bufOut.data(), lineOutLen, format, arglist);

    return IS_Write(bufOut.data(), lineOutLen, 1, fileHdl);
}


void ik_print_log(const char *format, ...) {
    char text1[256];
    int d = 0, m = 0, y = 0;
    int date = player.stardate;
    va_list ap;
    va_list ap2;
    va_start(ap, format);
    va_copy(ap2, ap);

    // clear the text1 buffer.
    memset(text1, 0, sizeof(text1));

    if (logfile) {
        if (date > last_logdate) {
            d = date % 365;
            for (y = 0; y < 12; y++)
                if (d >= months[y].sd && d < months[y].sd + months[y].le)
                    m = y;
            d = d + 1 - months[m].sd;
            y = 4580 + (date / 365);
            snprintf(text1, 256, "Captain %s of the %s", player.captname, player.shipname);
            IS_Printf(logfile, "\n%s%*s %02d %s %d\n", text1, (int)(52-strlen(text1)), "Date:", d, months[m].name, y);
            IS_Printf(logfile, "================================================================\n");
            last_logdate = date;
        }
        IS_VPrintf(logfile, format, ap);
    }
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, ap2);
    va_end(ap2);
    va_end(ap);
}