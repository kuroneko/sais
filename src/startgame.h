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
#ifndef SAIS_STARTGAME_H
#define SAIS_STARTGAME_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------
//    CONSTANTS
// ----------------

// ----------------
//     TYPEDEFS
// ----------------

// ----------------
// GLOBAL VARIABLES
// ----------------

// ----------------
//    PROTOTYPES
// ----------------

int32 startgame();

void startgame_init();
void startgame_deinit();

void loadconfig();
void saveconfig();

#ifdef __cplusplus
};
#endif

#endif /* #ifndef SAIS_STARTGAME_H */
