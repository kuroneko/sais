-----------------------------------------
Strange Adventures in Infinite Space v1.5
          source code release
           11 November 2005
-----------------------------------------

To celebrate the release of Weird Worlds: Return to Infinite
Space (see http://www.shrapnelgames.com ) we have decided to
release the source code to the game that started it all:
Strange Adventures in Infinite Space.

This release includes source code that can be used to build
executables for either the DEMO or the RETAIL version of 
Strange Adventures in Infinite Space. The game content of 
the DEMO version is included for your convenience, along with
Win32 binaries. This is an unsupported release; proceed at 
your own risk.

The main intentions behind the release are to allow user-made
modifications that include both code and new game content; 
and make it possible to port the executables to different 
platforms for users who own a retail copy of the game but want
to run it on Linux etc. 

To facilitate the latter, the code has been converted to use 
SDL and SDL_Mixer libraries rather than DirectX. You will need
to download these libraries for your programming platform at:
http://www.libsdl.org

Strange Adventures in Infinite Space source code is released
under the GNU General Public License version 2 or later. See
the attached gpl.txt or read more at the GNU website:
http://www.gnu.org/copyleft/gpl.html

To sum it up, you can do anything you want with the code
including selling derivatives of it, as long as you release 
the modified source code along with your application.

Please note that the license only applies to the source code 
and executable programs provided in this release. In order 
to make the release more useful, the content files required 
to run the DEMO version of Strange Adventures in Infinite 
Space are included. These files are identical to the demo 
available at SAIS website. You are free to distribute the 
DEMO contents as long as no modifications are made. 

Unauthorized distribution of the content (such as graphics, 
sounds etc) of the RETAIL version of Strange Adventures in 
Infinite Space is prohibited by copyright law. However, you
may use this source code to create and distribute executable
programs which utilize said content if legally obtained by 
the end-user.

Should you create a completely original set of game content,
you could obviously do anything you want with it.

SDL and SDL_Mixer are licensed under the LGPL license:
http://www.gnu.org/licenses/lgpl.html

-------------------------
Notes from the programmer
-------------------------

* The code is hideous. Sorry. This was my second Win32 app.

* There may be some vestiges of DirectX/Win32 code left over,
  such as #include statements. They should be safe to remove
  as the code now uses SDL for all input, video and sound.

* Project file for MSVC6 is included.

* sdl_main.cpp includes the main() function which sets the
  fullscreen/windowed flag. By default it runs in a window
  for easier debugging.

* If you make and distribute modified versions of the SAIS
  demo (as in new executable, using the demo content) I'd 
  appreciate if you labeled it as an "unofficial" version.

* Mods intended for those who own a retail copy should not
  include any of the game's original artwork or other 
  content in the distribution package, just the new code
  and your own content.

* Visit the SAIS website to read about the game:
  http://www.digital-eel.com/sais/

* Infinite Space federation is a third-party site with a
  SAIS modding forum which may be useful to you:
  http://www.freewebs.com/strangefederation/

Iikka Keranen

----------
Copyrights
----------

Copyright (C) 2005 Richard Carlson, Iikka Keranen and William Sears

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.



