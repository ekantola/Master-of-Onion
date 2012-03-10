#ifndef __SHOTS_H
#define __SHOTS_H


#include "internal.h"


enum SHOT_TYPE_INDEX
{
  SHOT_NONE, SHOT_BULLET, SHOT_BEAM, SHOT_TORPEDO,

  SHOT_TYPE_LIST_LENGTH
};


enum DAMAGE_TYPE_INDEX
{
  DMG_COLLISION, DMG_PIERCING, DMG_LASER,

  DAMAGE_TYPE_LIST_LENGTH
};


struct ShotType
{
  char *name;
  uchar color;
  int damage_type, damage;
  void (*draw)(Shot *s), (*hide)(Shot *s), (*destroy)(Shot *s);
};


struct Shot
{
  int damage, mass, data;
  fixed x, y, speed_x, speed_y;
  Ship *master;
  ShotType *shot_type;
  Shot *prev, *next;  // dually linked list

  Shot();
  Shot(Weapon *wpn, fixed x, fixed y, fixed speed_x, fixed speed_y);
  ~Shot();
};


extern ShotType shot_type[SHOT_TYPE_LIST_LENGTH];


#endif
