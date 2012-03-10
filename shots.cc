/* Objflags:
-Wall -Werror -O -g
*/


#include "colors.h"
#include "equipment.h"
#include "misc.h"
#include "players.h"
#include "ships.h"
#include "shots.h"

#include <stdio.h>


void draw_bullet(Shot *shot);
void draw_beam(Shot *shot);

void hide_bullet(Shot *shot);
void hide_beam(Shot *shot);


ShotType shot_type[SHOT_TYPE_LIST_LENGTH] =
{
  { "None", 0, 0, 0, null, null, null },
  { "Bullet", COLOR_BULLET, DMG_PIERCING, 100, draw_bullet, hide_bullet, null },
  { "Laser beam", COLOR_LASER1, DMG_LASER, 20, draw_beam, hide_beam, null }
};

Shot *shotlist_head, *shotlist_tail;  // linked list head and tail


void init_shots()
{
  shotlist_head = new Shot;
  shotlist_tail = new Shot;

  shotlist_head->next = shotlist_tail;
  shotlist_tail->prev = shotlist_head;
  shotlist_head->prev = shotlist_tail->next = null;
}


void remove_shots()
{
  Shot *temp = shotlist_head->next, *next;

  while(temp != shotlist_tail)
  {
    next = temp->next;
    free(temp);  // to avoid the destructor call

    temp = next;
  }

  shotlist_head->next = shotlist_tail;
  shotlist_tail->prev = shotlist_head;
}



/****************************** Constructing **********************************/

Shot::Shot()
{

}


Shot::Shot(Weapon *wpn, fixed x, fixed y, fixed speed_x, fixed speed_y)
{
  this->damage = wpn->power;
  this->x = x;
  this->y = y;
  this->speed_x = speed_x;
  this->speed_y = speed_y;

  this->master = wpn->master;
  this->shot_type = wpn->shot_type;

  this->next = shotlist_head->next;
  this->prev = shotlist_head;

  shotlist_head->next->prev = this;
  shotlist_head->next = this;
}


Shot::~Shot()
{
  if(this->shot_type->destroy)
    this->shot_type->destroy(this);

  this->prev->next = this->next;
  this->next->prev = this->prev;
}



/********************* Shot drawing and hiding functions **********************/

extern fixed level_left, level_top, level_right, level_bottom;
extern BITMAP *level, *background;

extern fixed gravity;
extern fixed viscosity[VISCOSITY_LIST_LENGTH];


inline void draw_shot(Shot *shot)  { shot->shot_type->draw(shot); }
inline void hide_shot(Shot *shot)  { shot->shot_type->hide(shot); }


void draw_shots()
{
  Shot *temp = shotlist_head->next, *next;

  while(temp != shotlist_tail)  // draw from first to last
  {
    next = temp->next;  // we must store the next address because
    draw_shot(temp);    // the drawing function may destroy temp

    temp = next;
  }
}


void hide_shots()
{
  Shot *temp = shotlist_tail->prev, *prev;

  while(temp != shotlist_head)  // hide from last to first
  {
    prev = temp->prev;  // we must store the prev address because
    hide_shot(temp);    // the hiding function may destroy temp

    temp = prev;
  }
}


void draw_bullet(Shot *shot)
{
  int i, k, x = 0, y = 0, speed_coef, pixel;
  fixed f, speed_x = shot->speed_x, speed_y = shot->speed_y;
  Ship *s;

  if((ABS(speed_x) > itofix(1)) || (ABS(speed_y) > itofix(1)))
  {
    speed_coef = ffloor(MAX(ABS(speed_x), ABS(speed_y))) + 1;

    speed_x /= speed_coef;
    speed_y /= speed_coef;
  }
  else
    speed_coef = 1;

  for(i=0; i<speed_coef; ++i)
  {
    shot->x += speed_x;
    shot->y += (speed_y += gravity/speed_coef);

    if((shot->x < level_left) || (shot->x > level_right) || (shot->y < level_top) || (shot->y > level_bottom))
    {
      delete shot;
      return;
    }

    x = ffloor(shot->x);
    y = ffloor(shot->y);

    pixel = _getpixel(level, x, y);

    if((pixel >= COLOR_FIRST_PLAYER) && (pixel <= COLOR_LAST_PLAYER))
    {
      s = player[pixel-COLOR_FIRST_PLAYER]->ship;

      f = speed_coef * itofix(shot->mass) / s->mass;

      s->speed_x += fmul(f, speed_x);
      s->speed_y += fmul(f, speed_y);

      s->assign_damage(shot->shot_type->damage_type, shot->damage);

      delete shot;
      return;
    }
    else if(pixel >= COLOR_FIRST_BASE)
    {
      if(pixel >= COLOR_FIRST_SOFT_GROUND)
      {
        _putpixel(level, x, y, COLOR_BACKGROUND);
        _putpixel(background, x, y, COLOR_BACKGROUND);
      }
      else if(pixel >= COLOR_FIRST_TURNS_INTO_WATER)
      {
        k = pixel - (COLOR_FIRST_TURNS_INTO_WATER - COLOR_FIRST_WATER);

	_putpixel(level, x, y, k);
        _putpixel(background, x, y, k);
      }

      delete shot;
      return;
    }
    else if(pixel >= COLOR_FIRST_WATER)  // water and slimes
    {
      f = itofix(1) - viscosity[1 + ((pixel-COLOR_FIRST_WATER) >> 2)];

      speed_x = fmul(speed_x, f);
      speed_y = fmul(speed_y, f);
    }
    else  // air
    {
      f = itofix(1) - *viscosity;

      speed_x = fmul(speed_x, f);
      speed_y = fmul(speed_y, f);
    }
  }

  shot->speed_x = speed_coef * speed_x;
  shot->speed_y = speed_coef * speed_y;

  _putpixel(level, x, y, shot->shot_type->color);
}


void hide_bullet(Shot *shot)
{
  int x = ffloor(shot->x), y = ffloor(shot->y);

  _putpixel(level, x, y, _getpixel(background, x, y));
}


void draw_beam(Shot *shot)
{
  int pixel, *data = &shot->data;
  fixed x = shot->x, y = shot->y;
  fixed speed_x = shot->speed_x, speed_y = shot->speed_y;

  shot->data = 0;  // counts how many times the loop has been executed

  while((x >= level_left) && (y >= level_top) && (x <= level_right) && (y <= level_bottom))
  {
    pixel = _getpixel(level, ffloor(x), ffloor(y));

    if((pixel > COLOR_BACKGROUND) && (pixel != shot->master->color))
    {
      if(pixel <= COLOR_LAST_PLAYER)
      {
        player[pixel-COLOR_FIRST_PLAYER]->ship->assign_damage(shot->shot_type->damage_type, shot->damage);
        return;
      }
      else if(pixel >= COLOR_FIRST_SLIME1)
        return;
    }

    _putpixel(level, ffloor(x), ffloor(y), shot->shot_type->color);

    x += speed_x;
    y += speed_y;

    ++(*data);
  }

  if(!*data)
    delete shot;
}


void hide_beam(Shot *shot)
{
  int i;
  fixed x = shot->x, y = shot->y;
  fixed speed_x = shot->speed_x, speed_y = shot->speed_y;

  for(i=shot->data; i>0; --i)
  {
    _putpixel(level, ffloor(x), ffloor(y), _getpixel(background, ffloor(x), ffloor(y)));

    x += speed_x;
    y += speed_y;
  }

  delete shot;
}
