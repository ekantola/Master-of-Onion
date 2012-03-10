/* Objflags:
-Wall -Werror -O -g
*/


#include "config.h"


const char *cfg_player_key[CFG_KEY_LIST_LENGTH] =
{
  "key_weapon1", "key_weapon2", "key_weapon3", "key_special1", "key_special2",
  "key_shield", "key_accelerate", "key_turn_left", "key_turn_right"
};


const char *cfg_section[CFG_SECTION_LIST_LENGTH] =
{
  "game", "player_1", "player_2", "player_3", "player_4"
};
