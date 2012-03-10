/* Objflags:
-Wall -Werror -O -g
*/


#include "players.h"

#include "colors.h"
#include "equipment.h"
#include "ships.h"

#include <allegro.h>
#include <aliases.h>
#include <string.h>


uchar default_key[MAX_PLAYERS][PLAYER_KEY_LIST_LENGTH] =
{
  { KEY_W, KEY_A, KEY_D, KEY_TAB, KEY_TILDE, KEY_CAPSLOCK, KEY_1, KEY_2, KEY_LSHIFT },
  { KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_PGDN, KEY_END, KEY_DEL, KEY_PGUP, KEY_HOME, KEY_INSERT },
  { KEY_P, KEY_L, KEY_QUOTE, KEY_M, KEY_N, KEY_B, KEY_J, KEY_H, KEY_G },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int players = 2;
Player *player[MAX_PLAYERS];

int Player::window_w, Player::window_h;
BITMAP *Player::ship_plan;


/****************************** Helper functions ******************************/

static void draw_bar(int left, int top, int right, int bottom, int value, int max_value, uchar color)
{
  int i;
  
  i = left + fmul(right-left+1, fdiv(value, max_value));

  if(i > left)
    rectfill(screen, left, top, i-1, bottom, color);
  if(i < right)
    rectfill(screen, i, top, right, bottom, 0);
}



/*********************************** Init *************************************/

extern int screen_w, screen_h;


Player::Player(int p)
{
  this->index = p;
  this->ship = new Ship(this);
}


void Player::init()
{
  int i;
  Ship *s;

  s = this->ship;

// ship init
  s->init();

// window position
  this->window_left = 3 + (this->index % 2) * (screen_w>>1);
  this->window_top = 3 + (this->index >> 1) * (screen_h>>1);

// indicator bars
  this->ship_left = this->window_left;
  this->ship_top = this->window_top + Player::window_h + 1;

  this->bar_hit_points_left = this->ship_left + Player::ship_plan->w + 1;
  this->bar_hit_points_top = this->ship_top;
  this->bar_hit_points_right = this->window_left + Player::window_w - 1;
  this->bar_hit_points_bottom = this->bar_hit_points_top + 3;

  this->bar_energy_left = this->bar_hit_points_left;
  this->bar_energy_top = this->bar_hit_points_bottom + 2;
  this->bar_energy_right = this->bar_hit_points_right;
  this->bar_energy_bottom = this->bar_energy_top + 3;

  i = s->max_internal + s->max_armor + s->max_shield;  // maximum hit points

  this->bar_internal_left = this->bar_hit_points_left;
  this->bar_internal_right = this->bar_internal_left + fmul(Player::window_w, fdiv(s->max_internal, i));
  this->bar_armor_left = this->bar_internal_right + 1;
  this->bar_armor_right = this->bar_armor_left + fmul(Player::window_w, fdiv(s->max_armor, i));
  this->bar_shield_left = this->bar_armor_right + 1;
  this->bar_shield_right = this->bar_hit_points_right;
}



/********************************** Methods ***********************************/

void Player::update_indicators()
{
  Ship *s;

  s = this->ship;

// devices


// internal
  draw_bar(this->bar_internal_left, this->bar_hit_points_top, this->bar_internal_right, this->bar_hit_points_bottom,
           s->internal, s->max_internal, COLOR_INFOBAR_INTERNAL);

// armor
  draw_bar(this->bar_armor_left, this->bar_hit_points_top, this->bar_armor_right, this->bar_hit_points_bottom,
           s->armor, s->max_armor, COLOR_INFOBAR_ARMOR);

// shield
  if(s->max_shield != 0)
    draw_bar(this->bar_shield_left, this->bar_hit_points_top, this->bar_shield_right, this->bar_hit_points_bottom,
             s->device_broken[SHIP_DEV_SHIELD] ? 0 : s->shield->charge,
	     s->max_shield, COLOR_INFOBAR_SHIELD);

// energy
  if(s->max_energy != 0)
    draw_bar(this->bar_energy_left, this->bar_energy_top, this->bar_energy_right, this->bar_energy_bottom,
             s->device_broken[SHIP_DEV_SHIELD] ? 0 : s->generator->charge,
	     s->max_energy, COLOR_INFOBAR_ENERGY);
}


bool Player::key_down(int index)
{
  return key[this->key_scancode[index]];
}
