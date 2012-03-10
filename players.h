#ifndef __PLAYERS_H
#define __PLAYERS_H


#include "internal.h"


enum PLAYER_KEY_INDEX
{
  PL_KEY_FIRST_WEAPON = 0, PL_KEY_LAST_WEAPON = 2,
  PL_KEY_FIRST_SPECIAL = 3, PL_KEY_LAST_SPECIAL = 4,
  PL_KEY_SHIELD = 5,

  PL_KEY_ACCELERATE = 6, PL_KEY_TURN_LEFT = 7, PL_KEY_TURN_RIGHT = 8,

  PLAYER_KEY_LIST_LENGTH
};


struct Player
{
  int index;
  Ship *ship;

// player status
  int dead;

// window position on screen
  int window_left, window_top;

// indicator bar positions
  int ship_left, ship_top;

  int bar_hit_points_left, bar_hit_points_top;
  int bar_hit_points_right, bar_hit_points_bottom;

  int bar_internal_left, bar_internal_right;
  int bar_armor_left, bar_armor_right;
  int bar_shield_left, bar_shield_right;

  int bar_energy_top, bar_energy_left;
  int bar_energy_right, bar_energy_bottom;

// player keys
  bool prev_key_down[PLAYER_KEY_LIST_LENGTH];
  uchar key_scancode[PLAYER_KEY_LIST_LENGTH];
  
// methods
  Player(int p);
  
  void init();
  
  void update_indicators();
  
  bool key_down(int index);
  
// static
  static int window_w, window_h;
  static BITMAP *ship_plan;
};


extern uchar default_key[MAX_PLAYERS][PLAYER_KEY_LIST_LENGTH];
extern int players;
extern Player *player[MAX_PLAYERS];


#endif
