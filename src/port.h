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

#ifndef STRANGE_PORT_H
#define STRANGE_PORT_H

/** OS Specific Interfaces
 *
 * Each OS needs to either use the implementation from portable, or
 * provide it's own.
 *
 * These functions are mostly for general housekeeping - displaying critical
 * failure messages, crash handling, OS specific setup.  etc.
 */

/* NORETURN should indicate the function never returns */
#define NORETURN __attribute__((noreturn))

/** SYS_abort immediately causes the application to exit, displaying the given
 * message (formatted by printf) in a manner visible to the user.
 *
 * @param format The format string as per printf
 * @param ... arguments for the foramt.
 */
NORETURN void SYS_abort(const char *format, ...);


#endif //STRANGE_PORT_H
