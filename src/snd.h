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
#ifndef SAIS_SND_H
#define SAIS_SND_H

#ifdef __cplusplus
extern "C" {
#endif

#define CHN_SFX 0
#define NUM_SFX 15

#ifndef DEMO_VERSION
enum sfxsamples
{
	// combat
	WAV_BEAM1,
	WAV_BEAM2,
	WAV_BEAM3,
	WAV_BEAM4,
	WAV_BEAM5,
	WAV_BEAM6,
	WAV_BEAM7,
	WAV_PROJ1,
	WAV_PROJ2,
	WAV_PROJ3,
	WAV_PROJ4,
	WAV_PROJ5,
	WAV_PROJ6,
	WAV_PROJ7,
	WAV_PROJ8,
	WAV_PROJ9,
	WAV_PROJ10,
	WAV_PROJ11,
	WAV_HIT1,
	WAV_HIT2,
	WAV_HIT3,
	WAV_HIT4,
	WAV_HIT5,
	WAV_EXPLO1,
	WAV_EXPLO2,
	WAV_SHIELD,
	WAV_CLOAKIN,
	WAV_CLOAKOUT,
	WAV_BOARD,
	WAV_SYSDAMAGE,
	WAV_SYSHIT1,
	WAV_SYSHIT2,
	WAV_SYSFIXED,
	WAV_TELEPORT,
	WAV_FIERYFURY,
	WAV_FIGHTERLAUNCH,
	WAV_ENDSIMULATION,
	// interface
	WAV_YES,
	WAV_NO,
	WAV_ACCEPT,
	WAV_DECLINE,
	WAV_DOT,
	WAV_DOT2,
	WAV_SELECT,
	WAV_DESELECT,
	WAV_SELECTSTAR,
	WAV_INFO,
	WAV_SELECTSHIP,
	WAV_WAIT,
	WAV_SLIDER,
	WAV_INSTALL,
	WAV_INSTALL2,
	WAV_LOCK,
	WAV_DEPART,
	WAV_ARRIVE,
	WAV_HYPERDRIVE,
	WAV_FOLDSPACE,
	WAV_RADAR,
	WAV_SCANNER,
	WAV_BRIDGE,
	WAV_MESSAGE,
	WAV_TANRUMESSAGE,
	WAV_PAYMERC,
	WAV_TRADE,
	WAV_CASH,
	WAV_PROBE_LAUNCH,
	WAV_PROBE_DEST,
	WAV_FOMAX_HI,
	WAV_FOMAX_BYE,
	WAV_FOMAX_WISH,
	WAV_TIMER,
	WAV_WARNING,
	WAV_OPTICALS,
	WAV_TITLE1,
	WAV_TITLE2,
	WAV_TITLE3,
	WAV_TITLE4,
	WAV_TITLE5,
	WAV_LOGO,
	// races
	WAV_KLAKAR,
	WAV_ZORG,
	WAV_MUKTIAN,
	WAV_GARTHAN,
	WAV_TANRU,
	WAV_URLUQUAI,
	WAV_KAWANGI,
	// events
	WAV_BLACKHOLE,
	WAV_BLACKHOLEDEATH,
	WAV_COLLAPSER,
	// cards
	WAV_ALLY,
	WAV_FLARE,
	WAV_SPY,
	WAV_NOVA,
	WAV_SABOTEUR,
	WAV_WHALES,
	WAV_CUBE,
	WAV_SPACEHULK,
	WAV_GASGIANT,
	WAV_NOPLANET,
	// normal item categories
	WAV_WEAPON,
	WAV_SYSTEM,
	WAV_DEVICE,
	WAV_LIFEFORM,
	WAV_DRIVE,
	// artifacts
	WAV_PLATINUM,
	WAV_TITANIUM,
	WAV_BRASS,
	WAV_PLASTIC,
	WAV_CENOTAPH,
	WAV_TORC,
	WAV_GONG,
	WAV_MANTLE,
	WAV_WHISTLE,
	WAV_HORLOGE,
	WAV_TOY,
	WAV_CODEX,
	WAV_SCULPTURE,
	WAV_CONOGRAPH,
	WAV_MONOCLE,
	WAV_BAUBLE,
	WAV_MIRROR,
	WAV_MUMMY,
	WAV_MONOLITH,
	WAV_CONOGRAPH2,
	// music
	WAV_MUS_START,
	WAV_MUS_SPLASH,
	WAV_MUS_THEME,
	WAV_MUS_TITLE,
	WAV_MUS_DEATH,
	WAV_MUS_VICTORY,
	WAV_MUS_COMBAT,
	WAV_MUS_NEBULA,
	WAV_MUS_HISCORE,
	WAV_MUS_ROCK,
	WAV_MUS_SIMULATOR,
	WAV_MAX
};

#define SND_BEAMS WAV_BEAM1
#define SND_PROJS WAV_PROJ1
#define SND_HITS	WAV_HIT1
#define SND_ITEMS WAV_WEAPON
#define SND_ARTIF WAV_PLATINUM

#else
enum sfxsamples // Reduced for DEMO_VERSION
{
	// combat
	WAV_BEAM1,
	WAV_BEAM2,
	WAV_BEAM3,
	WAV_PROJ1,
	WAV_PROJ2,
	WAV_PROJ3,
	WAV_PROJ4,
	WAV_HIT1,
	WAV_HIT2,
	WAV_EXPLO1,
	WAV_EXPLO2,
	WAV_SHIELD,
	WAV_SYSDAMAGE,
	WAV_SYSHIT1,
	WAV_SYSHIT2,
	WAV_SYSFIXED,
	WAV_FIGHTERLAUNCH,
	WAV_ENDSIMULATION,
	// interface
	WAV_YES,
	WAV_NO,
	WAV_ACCEPT,
	WAV_DECLINE,
	WAV_DOT,
	WAV_DOT2,
	WAV_SELECT,
	WAV_DESELECT,
	WAV_SELECTSTAR,
	WAV_INFO,
	WAV_SELECTSHIP,
	WAV_WAIT,
	WAV_SLIDER,
	WAV_INSTALL,
	WAV_INSTALL2,
	WAV_LOCK,
	WAV_DEPART,
	WAV_ARRIVE,
	WAV_HYPERDRIVE,
	WAV_RADAR,
	WAV_SCANNER,
	WAV_BRIDGE,
	WAV_MESSAGE,
	WAV_PAYMERC,
	WAV_TRADE,
	WAV_CASH,
	WAV_PROBE_LAUNCH,
	WAV_PROBE_DEST,
	WAV_TIMER,
	WAV_WARNING,
	WAV_TITLE1,
	WAV_TITLE2,
	WAV_TITLE3,
	WAV_TITLE4,
	WAV_TITLE5,
	WAV_LOGO,
	// races
	WAV_KLAKAR,
	WAV_GARTHAN,
	// events
	WAV_BLACKHOLE,
	WAV_BLACKHOLEDEATH,
	// cards
	WAV_ALLY,
	WAV_FLARE,
	WAV_SPY,
	WAV_GASGIANT,
	// normal item categories
	WAV_WEAPON,
	WAV_SYSTEM,
	WAV_DEVICE,
	WAV_LIFEFORM,
	WAV_DRIVE,
	// artifacts
	WAV_TITANIUM,
	WAV_BRASS,
	WAV_CENOTAPH,
	WAV_WHISTLE,
	WAV_HORLOGE,
	WAV_TOY,
	WAV_CODEX,
	WAV_SCULPTURE,
	WAV_MONOCLE,
	// music
	WAV_MUS_START,
	WAV_MUS_SPLASH,
	WAV_MUS_THEME,
	WAV_MUS_TITLE,
	WAV_MUS_DEATH,
	WAV_MUS_VICTORY,
	WAV_MUS_COMBAT,
	WAV_MUS_NEBULA,
	WAV_MUS_HISCORE,
	WAV_MUS_SIMULATOR,
	WAV_MAX
};

#define SND_BEAMS WAV_BEAM1
#define SND_PROJS WAV_PROJ1
#define SND_HITS	WAV_HIT1
#define SND_ITEMS WAV_WEAPON
#define SND_ARTIF WAV_TITANIUM

#endif

typedef struct {
	char name[64];
	void *wave;
} t_wavesound;

typedef struct 
{
	int id;      // sample playing
	int st, et;  // start and end time
} t_sfxchannel;

extern t_sfxchannel sfxchan[NUM_SFX];
extern t_wavesound wavesnd[WAV_MAX];

// ******** SOUND *********

int Load_WAV(const char *filename, int id);
void load_all_sfx(void);
int Delete_Sound(int id);
int Delete_All_Sounds(void);

#define Play_Sound2(id,ch) Play_Sound(id,ch,0,-1,-1,0)
#define Play_Sound3(id,ch,fl) Play_Sound(id,ch,fl,-1,-1,0)
#define Play_Sound4(id,ch,fl,vol) Play_Sound(id,ch,fl,vol,-1,0)
int Play_Sound(int id, int ch, int flags,int volume, int rate, int pan);

#define Play_SoundFX1(id) Play_SoundFX(id, 0, -1, -1, 0, 30)
#define Play_SoundFX2(id,t) Play_SoundFX(id, t, -1, -1, 0, 30)
#define Play_SoundFX3(id,t,v) Play_SoundFX(id, t, v, -1, 0, 30)
int Play_SoundFX(int id, int t, int volume, int rate, int pan, int cutoff);
int Set_Sound_Volume(int ch,int vol);
int Set_Sound_Freq(int ch,int freq);
int Set_Sound_Pan(int ch,int pan);
int Stop_Sound(int ch);
int Stop_All_Sounds(void);
int Status_Sound(int ch);
int Get_Sound_Size(int id);
int Get_Sound_Rate(int id);

// ********* MUSIC ********

typedef struct 
{
	int8 volseq[64];
	int8 panseq[64];
	int8 sync[4];
	int8 samp[4];
} t_song;

extern int32 m_freq[4];
extern int8 m_mainvol;
extern int8 s_volume;
extern t_song m_song;
extern int8 m_playing;

void start_music();
void upd_music(int pos);
int m_get_pan(int ch, int pos);
int m_get_vol(int ch, int pos);

void save_cur_music(char *fname);
void load_cur_music(char *fname);
void prep_music(int n); // copy from songs[] to song
void plop_music(int n); // copy from song to songs[]

#ifdef __cplusplus
};
#endif

#endif /* #ifndef SAIS_SND_H */
