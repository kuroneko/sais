/* Strange Adventures in Infinite Space GPL
 * Copyright (C) 2005 Richard Carlson, Iikka Keranen and William Sears
 * Copyright (C) 2020 Christopher Collins
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

#include "../safe_cstr.h"

#include <cstdarg>
#include <cstdio>

void safe_strncpy(char *dest, const char *src, size_t n) {
    if (n>0) {
        strncpy(dest, src, n);
        dest[n - 1] = '\0';
    }
}

int safe_snprintf(char *dest, size_t n, const char *format, ...) {
    int rv = -1;
    std::va_list arglist;
    va_start(arglist, format);
    rv = safe_vsnprintf(dest, n, format, arglist);
    va_end(arglist);
    return rv;
}

int safe_vsnprintf(char *dest, size_t n, const char *format, std::va_list args) {
    auto rv = vsnprintf(dest, n, format, args);
    dest[n-1] = '\0';
    return rv;
}

