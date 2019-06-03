#ifndef GAME_VALUES__
#define GAME_VALUES__

#define SERVER_IP_1 "65.189.217.119"
#define SERVER_IP_2 "127.0.0.1"

#define NETWORK_PORT 8000
#define TICKS_PER_SECOND 20
#define TICK_PERIOD (1000 / TICKS_PER_SECOND) 

#define ROOM_START_GAME_TIME 2 * TICKS_PER_SECOND

#define PING_TIME 6 // request ping after no response for X seconds
#define DISCONNECT_TIME 5 // dc after no response for X seconds after ping

#define MAX_TEXT_LEN 1320
#define MAX_USERNAME_LEN 20
#define MAX_PASSWORD_LEN 30
#define MAX_EMAIL_LEN 320
#define MIN_PASSWORD_LEN 8

#define MAX_TEAM_SIZE 4
#define MAX_PLAYERS 8
#define MAX_PARTY_SIZE 4

#define MAX_SPELLS 80
#define SPELL_SLOTS 5

#define MAX_ENTITIES 256
#define MAX_ATTACK_SPEED 4

#define MAX_COMMANDS 144
#define MAX_COMMANDS_HISTORY MAX_COMMANDS * 60 * 15
#define MAX_UNIT_TILE_SIZE 64 // w * h   this we change when we know biggest unit

#define MAX_NAME_LEN 40

#define DATA_HERO_CACHE_SIZE 32
#define DATA_SPELL_CACHE_SIZE (DATA_HERO_CACHE_SIZE * SPELL_SLOTS)

#define TEAM0 0
#define TEAM1 1

#define MAX_FRIENDS 100

#define RES_WIDTH 480
#define RES_HEIGHT 270

#define RESPAWN_TIME 20


#define COLOR_WHITE 16777215
#define COLOR_RED 16777215
#define COLOR_GREEN 65280

#endif


