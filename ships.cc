/* Objflags:
-Wall -Werror -O -g
*/


#include "colors.h"
#include "equipment.h"
#include "level.h"
#include "misc.h"
#include "ships.h"
#include "players.h"


#include <string.h>


uchar curr_round;  // current round, needed by player event timing
ShipList *ship_head = new ShipList, *dead_ship_head = new ShipList;


extern int base_working_rate, collision_damage;
extern fixed gravity, base_turning;
extern fixed viscosity[VISCOSITY_LIST_LENGTH];



/********************************* ShipList ***********************************/

ShipList::ShipList()
{
  this->prev = this->next = this;
}



/****************************** Ship lifespan *********************************/

Ship::Ship(Player *owner)
{
  this->owner = owner;

  bzero(this->eventlist_first, MAX_ROUNDS * sizeof(Event *));
}


void Ship::init()
{
  int i;

// shortcuts
  this->shield = (Shield *)this->device[SHIP_DEV_SHIELD];
  this->shield_type = (ShieldType *)this->shield->device_type;

  this->generator = (Chargeable *)this->device[SHIP_DEV_GENERATOR];
  this->generator_type = (ChargeableType *)this->generator->device_type;

// device key types
  for(i=0; i<=SHIP_DEV_LAST_WEAPON; ++i)  // weapons
    this->type_toggle[i] = ((WeaponType *)this->device[i]->device_type)->type_toggle;

  for(; i<=SHIP_DEV_SHIELD; ++i)  // specials & shield
    this->type_toggle[i] = true;

// maximum internal, shield & energy
  this->max_internal = this->max_structure;

  for(i=0; i<SHIP_DEVICE_LIST_LENGTH; ++i)
    this->max_internal += this->device[i]->armor;

  this->max_shield = (this->shield != null) ? this->shield_type->max_charge : 0;
  this->max_energy = (this->generator != null) ? this->generator_type->max_charge : 0;

// reset it, too
  this->reset();
}


void Ship::reset()
{
  int i, x, y;

  this->owner->dead = 0;

// add ship at the end of the active ships' list
  this->prev = ship_head->prev;
  this->next = ship_head;
  ship_head->prev->next = this;
  ship_head->prev = this;

// reset ship state
  this->color = COLOR_FIRST_PLAYER + this->owner->index;

  this->internal = this->max_internal;
  this->structure = this->max_structure;
  this->armor = this->max_armor;

  for(i=0; i<DAMAGE_TYPE_LIST_LENGTH; ++i)
    this->damage[i] = 0;

  for(i=0; i<SHIP_DEVICE_LIST_LENGTH; ++i)
    this->device_broken[i] = (this->device[i] != null) ? false : true;

  if(!this->device_broken[SHIP_DEV_SHIELD])
    this->shield->charge = 0;

  if(!this->device_broken[SHIP_DEV_GENERATOR])
    this->generator->charge = 0;

  for(i=0; i<=SHIP_DEV_LAST_SPECIAL; ++i)
    this->use_device[i] = false;
  this->use_device[i] = true;

  this->speed_x = this->speed_y = 0;
  this->angle = ANGLE_UP;

  this->calc_direction();

// start at a random location
  do
  {
    x = rand() % level->w;
    y = rand() % level->h;
  }
  while(_getpixel(level, x, y) >= COLOR_FIRST_BASE);

  this->x = itofix(x);
  this->y = itofix(y);
}


void Ship::cleanup()
{
  int i;
  Event *curr_event, *next_event;

// destroy remaining events
  for(i=0; i<MAX_ROUNDS; ++i)
  {
    curr_event = this->eventlist_first[i];

    if(curr_event != null)
    {
      this->eventlist_first[i] = null;

      do
      {
        next_event = curr_event->next;
        delete curr_event;
        curr_event = next_event;
      }
      while(curr_event != null);
    }
  }
}



/******************************* Ship events ********************************/

// Adds an event into the ship's event queue for the round 'curr_round + timing'.
// There can be virtually any number of events waiting per a round.
void Ship::add_event(int data, EventProcedure event, uchar timing)
{
  Event *new_event;
  Event **curr_el_first = &this->eventlist_first[curr_round + timing];

  new_event = new Event;

  new_event->data = data;
  new_event->event = event;
  new_event->next = *curr_el_first;

  *curr_el_first = new_event;
}



/****************************** Round processing ******************************/

Ship *Ship::process_round()
{
  uchar pixel;
  int i, x, y, x1, y1, x2, y2;
  int damage, type;
  fixed f;
  Weapon *wpn;
  Special *spc;
  Event *curr_event, *next_event;
  Player *pl;
  Ship *temp = (Ship *)this->next;

  pl = this->owner;

// move ship
  this->last_x = this->x;
  this->last_y = this->y;

  this->x += this->speed_x;
  this->y += (this->speed_y += gravity);

// check that we didn't cross level borders
  if((this->x < level_left) || (this->x > level_right))
  {
    this->x = this->last_x;
    this->speed_x = 0;
  }
  if((this->y < level_top) || (this->y > level_bottom))
  {
    this->y = this->last_y;
    this->speed_y = 0;
  }

// get integer part of position
  x = ffloor(this->x);
  y = ffloor(this->y);

// scroll player view
  this->view_left = x - (Player::window_w >> 1);
  this->view_top = y - (Player::window_h >> 1);

  if(this->view_left < 0)
    this->view_left = 0;
  else if(this->view_left >= level->w - Player::window_w)
    this->view_left = level->w - Player::window_w;

  if(this->view_top < 0)
    this->view_top = 0;
  else if(this->view_top >= level->h - Player::window_h)
    this->view_top = level->h - Player::window_h;

// check for ground collisions
  pixel = _getpixel(background, x, y);

  if(pixel >= COLOR_FIRST_BASE)  // the ship has been hit by the ground
  {
    if((pixel < COLOR_FIRST_SOFT_GROUND) || (pixel > COLOR_LAST_SOFT_GROUND))
    {
      f = dist(this->speed_x, this->speed_y) - ftofix(.5);
      if(f > 0)
        this->assign_damage(DMG_COLLISION, collision_damage);
    }

    if((pixel <= COLOR_LAST_BASE) || (pixel == COLOR_PL1_BASE + this->owner->index))
    {
      this->repair(base_working_rate);

      if(!this->device_broken[SHIP_DEV_GENERATOR])
	this->recharge(base_working_rate);

      if((this->angle >= ANGLE_DOWN) && (this->angle < ANGLE_UP))
      {
        this->angle += base_turning;
        if(this->angle > ANGLE_UP)
          this->angle = ANGLE_UP;
      }
      else
      {
        this->angle -= base_turning;
        if(this->angle < ANGLE_UP)
          this->angle = ANGLE_UP;
      }

      this->calc_direction();
    }

    this->speed_x = this->speed_y = 0;
  }
  else if(pixel >= COLOR_FIRST_WATER)  // water or slimes
  {
    f = itofix(1) - viscosity[1 + ((pixel-COLOR_FIRST_WATER) >> 2)];

    this->speed_x = fmul(this->speed_x, f);
    this->speed_y = fmul(this->speed_y, f);
  }
  else  // air
  {
    f = itofix(1) - *viscosity;

    this->speed_x = fmul(this->speed_x, f);
    this->speed_y = fmul(this->speed_y, f);
  }

// movement keys
  if(pl->key_down(PL_KEY_ACCELERATE) && !this->device_broken[SHIP_DEV_DRIVE])
  {
    f = this->device[SHIP_DEV_DRIVE]->power / this->mass;

    this->speed_x += fmul(f, this->dir_x);
    this->speed_y += fmul(f, this->dir_y);
  }

  if(pl->key_down(PL_KEY_TURN_LEFT) && !this->device_broken[SHIP_DEV_LEFT_JETS])
  {
    this->angle += this->device[SHIP_DEV_LEFT_JETS]->power;
    if(this->angle >= itofix(256))
      this->angle -= itofix(256);

    this->calc_direction();
  }

  if(pl->key_down(PL_KEY_TURN_RIGHT) && !this->device_broken[SHIP_DEV_RIGHT_JETS])
  {
    this->angle -= this->device[SHIP_DEV_RIGHT_JETS]->power;
    if(this->angle < 0)
      this->angle += itofix(256);

    this->calc_direction();
  }

// weapon & special keys
  for(i=SHIP_DEV_FIRST_WEAPON; i<=SHIP_DEV_SHIELD; ++i)
  {
    if(pl->key_down(i))
    {
      if(!pl->prev_key_down[i])  // the key was just pressed
      {
        this->use_device[i] = (this->type_toggle[i] ? this->use_device[i]^true : true);
        pl->prev_key_down[i] = true;
      }
    }
    else  // the key is up
      pl->prev_key_down[i] = false;
  }

// using weapons
  for(i=SHIP_DEV_FIRST_WEAPON; i<=SHIP_DEV_LAST_WEAPON; ++i)
  {
    if(this->use_device[i])
    {
      if(!this->type_toggle[i])  // non-toggle weapons act only once per a keypress
        this->use_device[i] = false;

      wpn = (Weapon *)this->device[i];

      if(!this->device_broken[i] && (this->generator->charge >= wpn->consumption))
      {
        this->generator->charge -= wpn->consumption;

        wpn->activate(wpn);
      }
    }
  }

// using specials
  for(i=SHIP_DEV_FIRST_SPECIAL; i<=SHIP_DEV_LAST_SPECIAL; ++i)
  {
    if(!this->device_broken[i])
    {
      spc = (Special *)this->device[i];

      if(this->use_device[i])
      {
	if(!this->device_broken[i] && (this->generator->charge >= spc->consumption))
        {
	  this->special_on[i-SHIP_DEV_FIRST_SPECIAL] = true;
          spc->activate(spc);
        }
      }
      else if(special_on[i-SHIP_DEV_FIRST_SPECIAL])
      {
        this->special_on[i-SHIP_DEV_FIRST_SPECIAL] = false;

	if(spc->deactivate != null)
          spc->deactivate(spc);
      }
    }
  }

// process events
  curr_event = this->eventlist_first[curr_round];

  if(curr_event != null)
  {
    this->eventlist_first[curr_round] = null;

  // walk through the list, removing events after processing them
    do
    {
      next_event = curr_event->next;  // store next address, 'curr_event' dies soon

      curr_event->event(this, curr_event->data);  // execute the event procedure
      delete curr_event;

      curr_event = next_event;
    }
    while(curr_event != null);
  }

// deal all the damage assigned to this player
  for(type=0; type<DAMAGE_TYPE_LIST_LENGTH; ++type)
  {
    damage = this->damage[type];
    if(damage == 0)
      continue;

    this->damage[type] = 0;

    if(!this->device_broken[SHIP_DEV_SHIELD] && (this->shield->charge > 0))
      this->shield->prevent(this->shield, type, &damage);

    if(damage > 0)  // the shield couldn't prevent all damage
    {
      this->armor -= damage;

      if(this->armor < 0)  // armor's gone, further dmg goes to internal systems
      {
        damage = -this->armor;
        this->armor = 0;
	this->internal -= damage;

        i = rand() % (2 * SHIP_DEVICE_LIST_LENGTH);

        if((i < SHIP_DEVICE_LIST_LENGTH) && !this->device_broken[i])  // device takes dmg
        {
          this->device[i]->armor -= damage;

          if(this->device[i]->armor < 0)  // device destroyed
          {
	    damage = -this->device[i]->armor;

	    this->device[i]->armor = 0;
	    this->device_broken[i] = true;

	    if(i <= SHIP_DEV_SHIELD)
	      this->use_device[i] = false;

	    ++broken_devices;
	  }
	  else
	    continue;
	}

      // still have some damage left? structure will take it
	this->structure -= damage;

	if(this->structure < 0)  // the ship took lethal damage
	{
	  pl->dead = 1000;  // the player reincarnates after 1000 rounds

	// remove the ship from active list and add into dead list
	  this->prev->next = this->next;
	  this->next->prev = this->prev;

	  this->prev = dead_ship_head->prev;
	  this->next = dead_ship_head;
	  dead_ship_head->prev->next = this;
	  dead_ship_head->prev = this;

	// clear indicator bars
	  rectfill(screen, pl->bar_hit_points_left, pl->bar_hit_points_top, pl->bar_hit_points_right, pl->bar_hit_points_bottom, 0);
	  rectfill(screen, pl->bar_energy_left, pl->bar_energy_top, pl->bar_energy_right, pl->bar_energy_bottom, 0);

	  this->cleanup();
	  return temp;
	}
      }
    }
  }

// recharge shield
  if(this->use_device[SHIP_DEV_SHIELD] == true)
  {
    i = this->max_shield - this->shield->charge;  // maximum increment

    if(i >= this->shield_type->recharge_rate)
    {
      i = this->shield_type->recharge_rate;
      f = this->shield_type->consumption;
    }
    else
      f = fmul(fdiv(i, this->shield_type->recharge_rate), this->shield_type->consumption);

    if(this->generator->charge >= f)
    {
      this->shield->charge += i;
      this->generator->charge -= f;
    }
  }
  else if(!this->device_broken[SHIP_DEV_SHIELD] && (this->shield->charge > 0))
    --this->shield->charge;

// recharge generator
  if(!this->device_broken[SHIP_DEV_GENERATOR])
    this->recharge(this->generator_type->recharge_rate);

// draw
  x = ffloor(this->x);
  y = ffloor(this->y);

  x1 = x + ffloor(3 * this->dir_x);
  y1 = y + ffloor(3 * this->dir_y);
    
  f = this->angle + itofix(96);
  x2 = x + ffloor(3 * fsin(f));
  y2 = y + ffloor(3 * fcos(f));
    
  line(level, x1, y1, x2, y2, this->color);
  line(level, x2, y2, x, y, this->color);
    
  f = this->angle - itofix(96);
  x2 = x + ffloor(3 * fsin(f));
  y2 = y + ffloor(3 * fcos(f));
    
  line(level, x1, y1, x2, y2, this->color);
  line(level, x2, y2, x, y, this->color);

  return temp;
}


Ship *Ship::hide()
{
  int x, y;

  x = ffloor(this->x) - 3;
  y = ffloor(this->y) - 3;

  blit(background, level, x, y, x, y, 7, 7);

  return (Ship *)this->next;
}


Ship *Ship::revive()
{
  Ship *temp = (Ship *)this->next;

  --this->owner->dead;

  if(this->owner->dead == 0)  // resurrect
  {
  // remove from the dead list
    this->prev->next = this->next;
    this->next->prev = this->prev;

    this->reset();
  }

  return temp;
}



/****************************** Damage-related ********************************/

// Assigns 'amount' damage of type 'type' to this player.
void Ship::assign_damage(int type, int amount)
{
  this->damage[type] += amount;
}


// Repairs the player's ship, restoring 'amount' hit points.
void Ship::repair(int amount)
{
  int i;
  Device *dev;

  if(this->broken_devices)
  {
    i = rand() % SHIP_DEVICE_LIST_LENGTH;

    if(this->device_broken[i])  // this device is broken, repair it
    {
      dev = this->device[i];

      dev->armor += amount;

      if(dev->armor > dev->device_type->max_armor)  // device fixed
      {
	dev->armor = dev->device_type->max_armor;

	this->device_broken[i] = false;
        --this->broken_devices;
      }
      else
        return;
    }
  }

  this->structure += amount;
  amount = this->structure - this->max_structure;

  if(amount > 0)  // structure fixed
  {
    this->structure -= amount;

    this->armor += amount;
    if(this->armor > this->max_armor)  // armor fixed
      this->armor = this->max_armor;
  }
}



/****************************** Energy-related ********************************/

void Ship::recharge(int amount)
{
  if(!this->device_broken[SHIP_DEV_GENERATOR])
  {
    this->generator->charge += amount;
    if(this->generator->charge > this->generator_type->max_charge)
      this->generator->charge = this->generator_type->max_charge;
  }
}



/*********************************** Other ************************************/

void Ship::calc_direction()
{
  this->dir_x = fsin(this->angle);
  this->dir_y = fcos(this->angle);
}
