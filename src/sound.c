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
#include "Typedefs.h"
#include "iface_globals.h"
#include "snd.h"

int8 s_volume;

#ifndef DEMO_VERSION
void load_all_sfx(void)
{
	// combat
	Load_WAV("sounds/combat/beam1.wav",			WAV_BEAM1);
	Load_WAV("sounds/combat/beam2.wav",			WAV_BEAM2);
	Load_WAV("sounds/combat/beam3.wav",			WAV_BEAM3);
	Load_WAV("sounds/combat/beam4.wav",			WAV_BEAM4);
	Load_WAV("sounds/combat/beam5.wav",			WAV_BEAM5);
	Load_WAV("sounds/combat/beam6.wav",			WAV_BEAM6);
	Load_WAV("sounds/combat/beam7.wav",			WAV_BEAM7);
	Load_WAV("sounds/combat/misl1.wav",			WAV_PROJ1);
	Load_WAV("sounds/combat/misl1.wav",			WAV_PROJ2);
	Load_WAV("sounds/combat/misl3.wav",			WAV_PROJ3);
	Load_WAV("sounds/combat/misl1.wav",			WAV_PROJ4);
	Load_WAV("sounds/combat/misl1.wav",			WAV_PROJ5);
	Load_WAV("sounds/combat/pgun1.wav",			WAV_PROJ6);
	Load_WAV("sounds/combat/pgun2.wav",			WAV_PROJ7);
	Load_WAV("sounds/combat/pgun3.wav",			WAV_PROJ8);
	Load_WAV("sounds/combat/pgun4.wav",			WAV_PROJ9);
	Load_WAV("sounds/combat/pgun5.wav",			WAV_PROJ10);
	Load_WAV("sounds/combat/pgun6.wav",			WAV_PROJ11);
	Load_WAV("sounds/combat/lowboom.wav",		WAV_HIT1);
	Load_WAV("sounds/combat/bulhit02.wav",	WAV_HIT2);
	Load_WAV("sounds/combat/plashit01.wav",	WAV_HIT3);
	Load_WAV("sounds/combat/lowboom.wav",		WAV_HIT4);
	Load_WAV("sounds/combat/lowboom.wav",		WAV_HIT5);
	Load_WAV("sounds/combat/explo01.wav",		WAV_EXPLO1);
	Load_WAV("sounds/combat/explo02.wav",		WAV_EXPLO2);
	Load_WAV("sounds/combat/shield02.wav",	WAV_SHIELD);
	Load_WAV("sounds/combat/cloak_in.wav",	WAV_CLOAKIN);
	Load_WAV("sounds/combat/cloak_out.wav",	WAV_CLOAKOUT);
	Load_WAV("sounds/combat/boardship.wav",	WAV_BOARD);
	Load_WAV("sounds/combat/sys_dmg.wav",		WAV_SYSDAMAGE);
	Load_WAV("sounds/combat/sys_hit.wav",		WAV_SYSHIT1);
	Load_WAV("sounds/combat/sys_hit_2.wav",	WAV_SYSHIT2);
	Load_WAV("sounds/combat/sys_fixed.wav",	WAV_SYSFIXED);
	Load_WAV("sounds/combat/teleport.wav",	WAV_TELEPORT);
	Load_WAV("sounds/combat/fieryfury.wav",	WAV_FIERYFURY);
	Load_WAV("sounds/combat/fighter.wav",		WAV_FIGHTERLAUNCH);
	Load_WAV("sounds/combat/sim_end.wav",		WAV_ENDSIMULATION);
	// interface
	Load_WAV("sounds/beep_yes.wav",       WAV_YES);
	Load_WAV("sounds/beep_no.wav",				WAV_NO);
	Load_WAV("sounds/beep_accept.wav",    WAV_ACCEPT);
	Load_WAV("sounds/beep_decline.wav",   WAV_DECLINE);
	Load_WAV("sounds/beep_dot.wav",       WAV_DOT);
	Load_WAV("sounds/beep_dot2.wav",      WAV_DOT2);
	Load_WAV("sounds/beep_select.wav",    WAV_SELECT);
	Load_WAV("sounds/beep_deselect.wav",  WAV_DESELECT);
	Load_WAV("sounds/beep_selectstar.wav",WAV_SELECTSTAR);
	Load_WAV("sounds/beep_info.wav",			WAV_INFO);
	Load_WAV("sounds/beep_selship.wav",   WAV_SELECTSHIP);
	Load_WAV("sounds/beep_wait.wav",			WAV_WAIT);
	Load_WAV("sounds/swut.wav",						WAV_SLIDER);
	Load_WAV("sounds/install.wav",				WAV_INSTALL);
	Load_WAV("sounds/install2.wav",				WAV_INSTALL2);
	Load_WAV("sounds/lock.wav",						WAV_LOCK);
	Load_WAV("sounds/depart02.wav",				WAV_DEPART);
	Load_WAV("sounds/arrive02.wav",				WAV_ARRIVE);
	Load_WAV("sounds/hyperdrv.wav",				WAV_HYPERDRIVE);
	Load_WAV("sounds/foldmove.wav",				WAV_FOLDSPACE);
	Load_WAV("sounds/radar1.wav",					WAV_RADAR);
	Load_WAV("sounds/scan_loop.wav",			WAV_SCANNER);
	Load_WAV("sounds/bridge_loop.wav",		WAV_BRIDGE);
	Load_WAV("sounds/message.wav",				WAV_MESSAGE);
	Load_WAV("sounds/message2.wav",				WAV_TANRUMESSAGE);
	Load_WAV("sounds/pay_loop.wav",				WAV_PAYMERC);
	Load_WAV("sounds/trade01.wav",				WAV_TRADE);
	Load_WAV("sounds/cash02.wav",					WAV_CASH);
	Load_WAV("sounds/probe_l.wav",				WAV_PROBE_LAUNCH);
	Load_WAV("sounds/probe_d.wav",				WAV_PROBE_DEST);
	Load_WAV("sounds/fomax_hi.wav",				WAV_FOMAX_HI);
	Load_WAV("sounds/fomax_bye.wav",			WAV_FOMAX_BYE);
	Load_WAV("sounds/fomax_wish.wav",			WAV_FOMAX_WISH);
	Load_WAV("sounds/timer.wav",					WAV_TIMER);
	Load_WAV("sounds/warning.wav",				WAV_WARNING);
	Load_WAV("sounds/opticals.wav",				WAV_OPTICALS);
	Load_WAV("sounds/alien1.wav",					WAV_TITLE1);
	Load_WAV("sounds/alien2.wav",					WAV_TITLE2);
	Load_WAV("sounds/alien3.wav",					WAV_TITLE3);
	Load_WAV("sounds/titzap01.wav",				WAV_TITLE4);
	Load_WAV("sounds/titzap02.wav",				WAV_TITLE5);
	Load_WAV("sounds/logo.wav",						WAV_LOGO);
	// race encounters
	Load_WAV("sounds/races/klakar.wav",		WAV_KLAKAR);
	Load_WAV("sounds/races/zorg.wav",			WAV_ZORG);
	Load_WAV("sounds/races/muktian.wav",	WAV_MUKTIAN);
	Load_WAV("sounds/races/garthan.wav",	WAV_GARTHAN);
	Load_WAV("sounds/races/tanru.wav",		WAV_TANRU);
	Load_WAV("sounds/races/urluquai.wav",	WAV_URLUQUAI);
	Load_WAV("sounds/races/kawangi.wav",	WAV_KAWANGI);
	// events
	Load_WAV("sounds/blakhole.wav",				WAV_BLACKHOLE);
	Load_WAV("sounds/blakhol2.wav",				WAV_BLACKHOLEDEATH);
	Load_WAV("sounds/collapse.wav",				WAV_COLLAPSER);
	// cards
	Load_WAV("sounds/cards/ally.wav",						WAV_ALLY);
	Load_WAV("sounds/cards/flare_loop.wav",			WAV_FLARE);
	Load_WAV("sounds/cards/intruder_loop.wav",	WAV_SPY);
	Load_WAV("sounds/cards/nova_loop.wav",			WAV_NOVA);
	Load_WAV("sounds/cards/intruder_loop.wav",	WAV_SABOTEUR);
	Load_WAV("sounds/cards/whales.wav",					WAV_WHALES);
	Load_WAV("sounds/cards/cube_loop.wav",			WAV_CUBE);
	Load_WAV("sounds/cards/hulk_loop.wav",			WAV_SPACEHULK);
	Load_WAV("sounds/cards/gas_loop.wav",				WAV_GASGIANT);
	Load_WAV("sounds/cards/no_planet.wav",			WAV_NOPLANET);
	// items
	Load_WAV("sounds/cards/weapon.wav",					WAV_WEAPON);
	Load_WAV("sounds/cards/system_loop.wav",		WAV_SYSTEM);
	Load_WAV("sounds/cards/device.wav",					WAV_DEVICE);
	Load_WAV("sounds/cards/lifeform.wav",				WAV_LIFEFORM);
	Load_WAV("sounds/cards/drive_loop.wav",			WAV_DRIVE);
	// artifacts
	Load_WAV("sounds/cards/platinum.wav",				WAV_PLATINUM);
	Load_WAV("sounds/cards/titanium.wav",				WAV_TITANIUM);
	Load_WAV("sounds/cards/brass.wav",					WAV_BRASS);
	Load_WAV("sounds/cards/plastic.wav",				WAV_PLASTIC);
	Load_WAV("sounds/cards/cenotaph.wav",				WAV_CENOTAPH);
	Load_WAV("sounds/cards/torc.wav",						WAV_TORC);
	Load_WAV("sounds/cards/gong.wav",						WAV_GONG);
	Load_WAV("sounds/cards/mantle.wav",					WAV_MANTLE);
	Load_WAV("sounds/cards/whistle.wav",				WAV_WHISTLE);
	Load_WAV("sounds/cards/horloge_loop.wav",		WAV_HORLOGE);
	Load_WAV("sounds/cards/toy.wav",						WAV_TOY);
	Load_WAV("sounds/cards/codex.wav",					WAV_CODEX);
	Load_WAV("sounds/cards/sculpture.wav",			WAV_SCULPTURE);
	Load_WAV("sounds/cards/conograph_loop.wav",	WAV_CONOGRAPH);
	Load_WAV("sounds/cards/conograph_use.wav",	WAV_CONOGRAPH2);
	Load_WAV("sounds/cards/monocle.wav",				WAV_MONOCLE);
	Load_WAV("sounds/cards/bauble.wav",					WAV_BAUBLE);
	Load_WAV("sounds/cards/mirror.wav",					WAV_MIRROR);
	Load_WAV("sounds/cards/mummy.wav",					WAV_MUMMY);
	Load_WAV("sounds/cards/monolith.wav",				WAV_MONOLITH);
	// music
	Load_WAV("sounds/music/start.wav",					WAV_MUS_START);
	Load_WAV("sounds/music/splash.wav",					WAV_MUS_SPLASH);
	Load_WAV("sounds/music/theme.wav",					WAV_MUS_THEME);
	Load_WAV("sounds/music/title.wav",					WAV_MUS_TITLE);
	Load_WAV("sounds/music/death.wav",					WAV_MUS_DEATH);
	Load_WAV("sounds/music/victory.wav",				WAV_MUS_VICTORY);
	Load_WAV("sounds/music/combat.wav",					WAV_MUS_COMBAT);
	Load_WAV("sounds/music/nebula.wav",					WAV_MUS_NEBULA);
	Load_WAV("sounds/music/hiscore.wav",				WAV_MUS_HISCORE);
	Load_WAV("sounds/music/rock.wav",						WAV_MUS_ROCK);
	Load_WAV("sounds/music/simulator.wav",			WAV_MUS_SIMULATOR);
}
#else
void load_all_sfx(void)	// Reduced for DEMO_VERSION
{
	// combat
	Load_WAV("sounds/combat/beam1.wav",			WAV_BEAM1);
	Load_WAV("sounds/combat/beam2.wav",			WAV_BEAM2);
	Load_WAV("sounds/combat/beam3.wav",			WAV_BEAM3);
	Load_WAV("sounds/combat/misl1.wav",			WAV_PROJ1);
	Load_WAV("sounds/combat/pgun1.wav",			WAV_PROJ2);
	Load_WAV("sounds/combat/pgun2.wav",			WAV_PROJ3);
	Load_WAV("sounds/combat/pgun3.wav",			WAV_PROJ4);
	Load_WAV("sounds/combat/lowboom.wav",		WAV_HIT1);
	Load_WAV("sounds/combat/bulhit02.wav",	WAV_HIT2);
	Load_WAV("sounds/combat/explo01.wav",		WAV_EXPLO1);
	Load_WAV("sounds/combat/explo02.wav",		WAV_EXPLO2);
	Load_WAV("sounds/combat/shield02.wav",	WAV_SHIELD);
	Load_WAV("sounds/combat/sys_dmg.wav",		WAV_SYSDAMAGE);
	Load_WAV("sounds/combat/sys_hit.wav",		WAV_SYSHIT1);
	Load_WAV("sounds/combat/sys_hit_2.wav",	WAV_SYSHIT2);
	Load_WAV("sounds/combat/sys_fixed.wav",	WAV_SYSFIXED);
	Load_WAV("sounds/combat/fighter.wav",		WAV_FIGHTERLAUNCH);
	Load_WAV("sounds/combat/sim_end.wav",		WAV_ENDSIMULATION);
	// interface
	Load_WAV("sounds/beep_yes.wav",       WAV_YES);
	Load_WAV("sounds/beep_no.wav",				WAV_NO);
	Load_WAV("sounds/beep_accept.wav",    WAV_ACCEPT);
	Load_WAV("sounds/beep_decline.wav",   WAV_DECLINE);
	Load_WAV("sounds/beep_dot.wav",       WAV_DOT);
	Load_WAV("sounds/beep_dot2.wav",      WAV_DOT2);
	Load_WAV("sounds/beep_select.wav",    WAV_SELECT);
	Load_WAV("sounds/beep_deselect.wav",  WAV_DESELECT);
	Load_WAV("sounds/beep_selectstar.wav",WAV_SELECTSTAR);
	Load_WAV("sounds/beep_info.wav",			WAV_INFO);
	Load_WAV("sounds/beep_selship.wav",   WAV_SELECTSHIP);
	Load_WAV("sounds/beep_wait.wav",			WAV_WAIT);
	Load_WAV("sounds/swut.wav",						WAV_SLIDER);
	Load_WAV("sounds/install.wav",				WAV_INSTALL);
	Load_WAV("sounds/install2.wav",				WAV_INSTALL2);
	Load_WAV("sounds/lock.wav",						WAV_LOCK);
	Load_WAV("sounds/depart02.wav",				WAV_DEPART);
	Load_WAV("sounds/arrive02.wav",				WAV_ARRIVE);
	Load_WAV("sounds/hyperdrv.wav",				WAV_HYPERDRIVE);
	Load_WAV("sounds/radar1.wav",					WAV_RADAR);
	Load_WAV("sounds/scan_loop.wav",			WAV_SCANNER);
	Load_WAV("sounds/bridge_loop.wav",		WAV_BRIDGE);
	Load_WAV("sounds/message.wav",				WAV_MESSAGE);
	Load_WAV("sounds/pay_loop.wav",				WAV_PAYMERC);
	Load_WAV("sounds/trade01.wav",				WAV_TRADE);
	Load_WAV("sounds/cash02.wav",					WAV_CASH);
	Load_WAV("sounds/probe_l.wav",				WAV_PROBE_LAUNCH);
	Load_WAV("sounds/probe_d.wav",				WAV_PROBE_DEST);
	Load_WAV("sounds/timer.wav",					WAV_TIMER);
	Load_WAV("sounds/warning.wav",				WAV_WARNING);
	Load_WAV("sounds/alien1.wav",					WAV_TITLE1);
	Load_WAV("sounds/alien2.wav",					WAV_TITLE2);
	Load_WAV("sounds/alien3.wav",					WAV_TITLE3);
	Load_WAV("sounds/titzap01.wav",				WAV_TITLE4);
	Load_WAV("sounds/titzap02.wav",				WAV_TITLE5);
	Load_WAV("sounds/logo.wav",						WAV_LOGO);
	// race encounters
	Load_WAV("sounds/races/klakar.wav",		WAV_KLAKAR);
	Load_WAV("sounds/races/garthan.wav",	WAV_GARTHAN);
	// events
	Load_WAV("sounds/blakhole.wav",				WAV_BLACKHOLE);
	Load_WAV("sounds/blakhol2.wav",				WAV_BLACKHOLEDEATH);
	// cards
	Load_WAV("sounds/cards/ally.wav",						WAV_ALLY);
	Load_WAV("sounds/cards/flare_loop.wav",			WAV_FLARE);
	Load_WAV("sounds/cards/intruder_loop.wav",	WAV_SPY);
	Load_WAV("sounds/cards/gas_loop.wav",				WAV_GASGIANT);
	// items
	Load_WAV("sounds/cards/weapon.wav",					WAV_WEAPON);
	Load_WAV("sounds/cards/system_loop.wav",		WAV_SYSTEM);
	Load_WAV("sounds/cards/device.wav",					WAV_DEVICE);
	Load_WAV("sounds/cards/lifeform.wav",				WAV_LIFEFORM);
	Load_WAV("sounds/cards/drive_loop.wav",			WAV_DRIVE);
	// artifacts
	Load_WAV("sounds/cards/titanium.wav",				WAV_TITANIUM);
	Load_WAV("sounds/cards/brass.wav",					WAV_BRASS);
	Load_WAV("sounds/cards/cenotaph.wav",				WAV_CENOTAPH);
	Load_WAV("sounds/cards/whistle.wav",				WAV_WHISTLE);
	Load_WAV("sounds/cards/horloge_loop.wav",		WAV_HORLOGE);
	Load_WAV("sounds/cards/toy.wav",						WAV_TOY);
	Load_WAV("sounds/cards/codex.wav",					WAV_CODEX);
	Load_WAV("sounds/cards/sculpture.wav",			WAV_SCULPTURE);
	Load_WAV("sounds/cards/monocle.wav",				WAV_MONOCLE);
	// music
	Load_WAV("sounds/music/start.wav",					WAV_MUS_START);
	Load_WAV("sounds/music/splash.wav",					WAV_MUS_SPLASH);
	Load_WAV("sounds/music/theme.wav",					WAV_MUS_THEME);
	Load_WAV("sounds/music/title.wav",					WAV_MUS_TITLE);
	Load_WAV("sounds/music/death.wav",					WAV_MUS_DEATH);
	Load_WAV("sounds/music/victory.wav",				WAV_MUS_VICTORY);
	Load_WAV("sounds/music/combat.wav",					WAV_MUS_COMBAT);
	Load_WAV("sounds/music/nebula.wav",					WAV_MUS_NEBULA);
	Load_WAV("sounds/music/hiscore.wav",				WAV_MUS_HISCORE);
	Load_WAV("sounds/music/simulator.wav",			WAV_MUS_SIMULATOR);
}
#endif