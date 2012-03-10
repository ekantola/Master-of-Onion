#ifndef __CONFIG_H
#define __CONFIG_H


#include <allegro.h>


enum CFG_KEY_INDEX
{
  CFG_KEY_ACCELERATE, CFG_KEY_TURN_LEFT, CFG_KEY_TURN_RIGHT,
  CFG_KEY_FIRST_ACTION = 3, CFG_KEY_LAST_ACTION = 8,

  CFG_KEY_LIST_LENGTH
};


enum CFG_VARIABLE_INDEX
{
  CFG_GAME, CFG_PLAYER_1, CFG_PLAYER_2, CFG_PLAYER_3, CFG_PLAYER_4,

  CFG_SECTION_LIST_LENGTH
};


extern const char *cfg_player_key[CFG_KEY_LIST_LENGTH];
extern const char *cfg_section[CFG_SECTION_LIST_LENGTH];


inline const char *get_cfg_string(int section, const char *name, const char *def)  { return get_config_string(cfg_section[section], name, def); }
inline int get_cfg_int(int section, const char *name, int def)         { return get_config_int(cfg_section[section], name, def); }


#endif
