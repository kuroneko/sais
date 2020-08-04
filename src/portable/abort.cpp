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
#include <cstdio>
#include <cstdlib>
#include <SDL.h>

#include "../port.h"

[[noreturn]] void
SYS_abort(const char *format, ...) {
    char messageOut[1024];
    std::va_list        va;

    va_start(va, format);
    vsprintf_s(messageOut, sizeof(messageOut), format, va);
    va_end(va);

    if (0 != SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Strange Adventures in Infinite Space!", messageOut, nullptr)) {
        // failed to show the dialog - log it via SDL_Log.
        SDL_Log("ABORTING: %s", messageOut);
    }
    exit(1);
}