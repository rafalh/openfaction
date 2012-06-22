/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/config.h
*  PURPOSE:     Server compile-time configuration
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifdef _DEBUG
//#define DUMP_PACKETS_SCREEN 1 /* Dump sent and received packets on screen */
//#define DUMP_PACKETS_FILE   1 /* Dump sent and received packets to file */
#endif

#define DEFAULT_NAME            "Open Faction Server"
#define DEFAULT_GAME_TYPE       RF_DM
#define DEFAULT_MAX_PLAYERS     8
#define DEFAULT_TIME_LIMIT      5 /* minutes */
#define DEFAULT_KILLS_LIMIT     100
#define DEFAULT_CAPTURES_LIMIT  10
#define DEFAULT_GEOMOD_LIMIT    255
#define DEFAULT_TEAM_DAMAGE     false
#define DEFAULT_FALL_DAMAGE     false
#define DEFAULT_WEAPON_STAY     true
#define DEFAULT_FORCE_RESPAWN   false
#define DEFAULT_BALANCE_TEAMS   true
#define DEFAULT_LEVEL           "dm01.rfl"

#define DEFAULT_PORT            7755
#define DEFAULT_MASK_IP         false
#define DEFAULT_NET_FPS         6

#define TIMEOUT_VALUE            10
#define TRACKER_PING_INTERVAL    100*1000
#define PLAYER_PING_INTERVAL     5*1000
#define STATS_UPDATE_INTERVAL    3*1000
#define ITEMS_UPDATE_INTERVAL    2*1000
#define LOADING_LEVEL_DELAY      3
#define MAX_PACKETS_PER_MIN      10000
#define MAX_PACKET_SIZE          512
#define PACKETS_BUFFERING        1 // FIXME: RFSB doesn't support buffered packets
#define MASK_AS_PF               0
#define SEND_RCON_OUTPUT         1
#define LOW_BANDWIDTH_USAGE      1
#define CREATE_PLAYERS_FOR_NPCS  0

#endif // CONFIG_H_INCLUDED
