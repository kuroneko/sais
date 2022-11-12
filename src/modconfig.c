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
#ifndef DEMO_VERSION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <physfs.h>

#include "Typedefs.h"
#include "is_fileio.h"
#include "iface_globals.h"
#include "gfx.h"
#include "interface.h"
#include "snd.h"

#define MAX_MODDIRS 64
#define MOD_INTERFACE_COLOR 11

#define MAX_PATH_LEN    1024

struct modDirectory
{
    char *name;
    char *path;
    bool isMapped;
};

static struct modDirectory allModDirectories[MAX_MODDIRS];
static size_t allModDirectoriesCount = 0;

void modDirectory_mapInPHYSFS(struct modDirectory *modDir);
void modDirectory_unmapInPHYSFS(struct modDirectory *modDir);

void
modDirectory_init(struct modDirectory *modDir, const char *modName, const char *modPath)
{
    modDir->name = strdup(modName);
    modDir->path = strdup(modPath);
    modDir->isMapped = false;
}

void
modDirectory_free(struct modDirectory *modDir)
{
    modDirectory_unmapInPHYSFS(modDir);
    if (modDir->name != NULL) {
        free(modDir->name);
        modDir->name = NULL;
    }
    if (modDir->path != NULL) {
        free(modDir->path);
        modDir->path = NULL;
    }
}

void
modDirectory_mapInPHYSFS(struct modDirectory *modDir)
{
    if (modDir->isMapped) {
        return;
    }
    const char *realPath = PHYSFS_getRealDir(modDir->path);
    if (realPath != NULL) {
        char modPath[MAX_PATH_LEN];
        sprintf(modPath, "%s%s%s", realPath, PHYSFS_getDirSeparator(), modDir->path);
        SDL_Log("Mapping %s -> ROOT", modPath);
        if (PHYSFS_mount(modPath, NULL, 0)) {
            modDir->isMapped = true;
        } else {
            SDL_Log("Failed to mount mod: %s", PHYSFS_getLastError());
        }
    } else {
        SDL_Log("Couldn't find mod directory");
    }
}

void
modDirectory_unmapInPHYSFS(struct modDirectory *modDir)
{
    if (modDir->isMapped) {
        PHYSFS_unmount(modDir->path);
        modDir->isMapped = false;
    }
}


static PHYSFS_EnumerateCallbackResult
modconfig_enumerate_cb(void *data, const char *origdir, const char *fname)
{
    SDL_Log("Found possible mod: %s", fname);
    char fullFilename[MAX_PATH_LEN];
    char sfName[MAX_PATH_LEN];
    sprintf(fullFilename, "%s/%s", origdir, fname);
    strcpy(sfName, fname);
    // is it a directory or an archive?
    if (PHYSFS_isDirectory(fullFilename)) {
        size_t idx = allModDirectoriesCount++;
        modDirectory_init(&allModDirectories[idx], fname, fullFilename);
    } else {
        size_t sfNameLen = strlen(sfName);
        if ((sfNameLen >= 5) &&
            (strcasecmp(sfName + sfNameLen-4, ".zip") == 0))
        {
            sfName[sfNameLen-4] = '\0';
            size_t idx = allModDirectoriesCount++;
            modDirectory_init(&allModDirectories[idx], sfName, fullFilename);
        }
    }
    if (allModDirectoriesCount >= MAX_MODDIRS) {
        return PHYSFS_ENUM_STOP;
    }
    return PHYSFS_ENUM_OK;
}

static int
modDirecotry_compare(const void *a, const void *b)
{
    const struct modDirectory *a_md = a;
    const struct modDirectory *b_md = b;

    return strcmp(a_md->name, b_md->name);
}

void modconfig_init()
{
    IS_FileHdl fil;

    // read mod names
    PHYSFS_enumerate("mods", modconfig_enumerate_cb, NULL);

    // sort the mod directories into order
    qsort(allModDirectories, allModDirectoriesCount, sizeof(*allModDirectories),modDirecotry_compare);

    fil = IS_Open_Read("graphics/palette.dat");
    IS_Read(globalpal, 1, 768, fil);
    IS_Close(fil);
    memcpy(currentpal, globalpal, 768);

    Load_WAV("sounds/beep_wait.wav", 0);
    s_volume = 100;
    interface_init();
}

void modconfig_deinit()
{
    int i;

    interface_deinit();
    Delete_Sound(0);

    for (i = 0; i< allModDirectoriesCount; i++) {
        modDirectory_free(&allModDirectories[i]);
    }
    allModDirectoriesCount = 0;
}

void modconfig_clearAllMods()
{
    int i;
    for (i = 0; i < allModDirectoriesCount; i++) {
        modDirectory_unmapInPHYSFS(&allModDirectories[i]);
    }
    moddir[0] = '\0';
    SDL_Log("Cleared all mods");
}

void modconfig_setMod(int modnumber)
{
    modconfig_clearAllMods();
    modDirectory_mapInPHYSFS(&allModDirectories[modnumber]);
    strncpy(moddir, allModDirectories[modnumber].name, 255);
    SDL_Log("Selected Mod: %s", allModDirectories[modnumber].name);
    SDL_Log("Mod basepath: %s (mounted onto tree)", allModDirectories[modnumber].path);
}

int modconfig_main()
{
    int32 t, t0;
    int32 c, mc, mx, my;
    int32 bx, by, h;
    int32 x, y;
    int32 msel, mscr;
    t_ik_image *backy;
    int32 mode;
    int end;
    char modName[MAX_PATH_LEN];

    modconfig_init();

    msel = 0;
    mscr = 0;
    mode = 0;
    backy = ik_load_pcx("graphics/starback.pcx", NULL);

    start_ik_timer(0, 20);
    t = 0;
    end = 0;
    while (!end && !must_quit) {
        ik_eventhandler();
        t0 = t;
        t = get_ik_timer(0);
        c = ik_inkey();
        mc = ik_mclick();
        mx = ik_mouse_x;
        my = ik_mouse_y;

        // enter or space were pressed...
        if (c == 13 || c == 32) {
            end = 1;
            if (mode == 0) {
                modconfig_clearAllMods();
            } else if (mode == 1) {
                // enable the mod.
                modconfig_setMod(msel);
            } else {}
        }

        if (mc & 1) { // button down...
            switch (mode) {
            case 0:    // main menu
                if (mx > bx + 48 && mx < bx + 208 && my > by + 35 && my < by + 108) {
                    c = (my - (by + 36)) / 24;
                    if (c == 0) { // clicked on start game.
                        modconfig_clearAllMods();
                        end = 1;
                        Play_SoundFX3(0, 0, 100);
                    } else if (c == 1) { // switch to mod list.
                        mode = 1;
                        Play_SoundFX3(0, 0, 100);
                    } else if (c == 2) { // quit.
                        must_quit = 1;
                        Play_SoundFX3(0, 0, 100);
                    }
                }
                break;

            case 1:    // mods
                if (mx > bx + 16 && mx < bx + 224 && my > by + 35 && my < by + 112) { // clicked on a mod in the list
                    c = mscr + (my - (by + 36)) / 8;
                    if (c >= 0 && c < allModDirectoriesCount) {
                        msel = c;
                        Play_SoundFX3(0, 0, 100);
                    }
                }
                if (mx > bx + 16 && mx < bx + 80 && my > by + h - 32 && my < by + h - 16) { // back to main menu.
                    mode = 0;
                    Play_SoundFX3(0, 0, 100);
                }
                if (mx > bx + 176 && mx < bx + 240 && my > by + h - 32 && my < by + h - 16) { // clicked start.
                    modconfig_setMod(msel);
                    Play_SoundFX3(0, 0, 100);
                    end = 1;
                }
                break;

            default:;
            }
        }

        if (t > t0) {
            prep_screen();
            ik_copybox(backy, screen, 0, 0, 639, 479, 0, 0);

            switch (mode) {
            case 0:    // main menu
                bx = 192;
                by = 164;
                h = 128;
                interface_drawborder(screen, bx, by, bx + 256, by + h, 1,
                    MOD_INTERFACE_COLOR, "Strange Adventures in Infinite Space");

                interface_drawbutton(screen, bx + 48, by + 40, 160, MOD_INTERFACE_COLOR, "STANDARD GAME");
                interface_drawbutton(screen, bx + 48, by + 64, 160, MOD_INTERFACE_COLOR, "MODS");
                interface_drawbutton(screen, bx + 48, by + 88, 160, MOD_INTERFACE_COLOR, "EXIT");
                break;

            case 1:
                bx = 192;
                by = 148;
                h = 160;
                interface_drawborder(screen, bx, by, bx + 256, by + h, 1,
                    MOD_INTERFACE_COLOR, "Strange Adventures in Infinite Space");

                if (allModDirectoriesCount > msel) {
                    strcpy(modName, allModDirectories[msel].name);
                } else {
                    modName[0] = '\0';
                }
                ik_print(screen, font_6x8, bx + 16, by + 22, 0, "Select a mod", modName);
                interface_thinborder(screen, bx + 16, by + 32, bx + 240, by + 120, MOD_INTERFACE_COLOR, 0);

                for (x = 0; x < allModDirectoriesCount; x++) {
                    y = x - mscr;
                    if (x == msel) {
                        ik_drawbox(screen, bx + 16, by + 35 + y * 8, bx + 239, by + 43 + y * 8,
                            3);
                    } //STARMAP_INTERFACE_COLOR*16+4);
                    ik_print(screen,
                        font_6x8,
                        bx + 20,
                        by + 36 + y * 8,
                        3 * (msel == x),
                        allModDirectories[x].name);
                }

                if (allModDirectoriesCount > 10) {
                    ik_dsprite(screen, bx + 228, by + 36, spr_IFarrows->spr[5], 2 + (MOD_INTERFACE_COLOR << 8));
                    ik_dsprite(screen, bx + 228, by + 108, spr_IFarrows->spr[4], 2 + (MOD_INTERFACE_COLOR << 8));
                    interface_drawslider(screen, bx + 228, by + 44, 1, 64, allModDirectoriesCount - 10, mscr,
                        MOD_INTERFACE_COLOR);
                }
                interface_thinborder(screen, bx + 16, by + 32, bx + 240, by + 120, MOD_INTERFACE_COLOR, -1);

                interface_drawbutton(screen, bx + 16, by + h - 32, 64, MOD_INTERFACE_COLOR, "CANCEL");
                interface_drawbutton(screen, bx + 256 - 80, by + h - 32, 64, MOD_INTERFACE_COLOR, "RUN MOD");
                break;

            default:;
            }

            update_palette();
            ik_blit();
        }
    }

    prep_screen();
    ik_drawbox(screen, 0, 0, 639, 479, 0);
    ik_blit();

    del_image(backy);
    modconfig_deinit();

    return end;
}


#endif
