#ifndef __SHIPS_H
#define __SHIPS_H


#include "internal.h"
#include "shots.h"


#define MAX_ROUNDS	256

#define ANGLE_UP	itofix(128)
#define ANGLE_DOWN	0


typedef typeof(void (*)(Ship *s, int data)) EventProcedure;


enum SHIP_DEVICE_INDEX
{
  SHIP_DEV_FIRST_WEAPON = 0, SHIP_DEV_LAST_WEAPON = 2,
  SHIP_DEV_FIRST_SPECIAL = 3, SHIP_DEV_LAST_SPECIAL = 4,
  SHIP_DEV_SHIELD = 5,

  SHIP_DEV_DRIVE = 6, SHIP_DEV_LEFT_JETS = 7, SHIP_DEV_RIGHT_JETS = 8,
  SHIP_DEV_GENERATOR = 9,

  SHIP_DEVICE_LIST_LENGTH
};

#define SHIP_ACTION_DEVICES	(SHIP_DEV_LAST_SPECIAL-SHIP_DEV_FIRST_WEAPON+1)
#define SHIP_DEVICE_SLOTS	(SHIP_DEV_SHIELD-SHIP_DEV_FIRST_WEAPON+1)


struct ShipList
{
  ShipList *prev, *next;

  ShipList();
};


struct Ship extends ShipList
{
  Player *owner;

// ship properties
  uchar color;
  int mass, max_armor, max_structure, max_internal, max_shield, max_energy;

// equipment
  bool use_device[SHIP_DEVICE_SLOTS], type_toggle[SHIP_DEVICE_SLOTS];
  bool device_broken[SHIP_DEVICE_LIST_LENGTH];
  bool special_on[SHIP_DEV_FIRST_SPECIAL-SHIP_DEV_LAST_SPECIAL+1];
  int broken_devices;
  Device *device[SHIP_DEVICE_LIST_LENGTH];

  Shield *shield;
  ShieldType *shield_type;

  Chargeable *generator;
  ChargeableType *generator_type;

// ship current state
  int armor, structure, internal;
  int damage[DAMAGE_TYPE_LIST_LENGTH];
  int view_left, view_top;
  fixed x, y, last_x, last_y, speed_x, speed_y;
  fixed angle, dir_x, dir_y;

  Event *eventlist_first[MAX_ROUNDS];  // the awaiting events for the next MAX_ROUNDS

// methods
  Ship(Player *owner);

  void init();
  void reset();  // set up ship's initial state in the beginning or after death
  void cleanup();  // clean up the remaining events at the end

  Ship *process_round();  // process moving, keyb input, events etc. for the round
  Ship *hide();
  Ship *revive();  // if the ship is dead, try to revive it

  void add_event(int data, EventProcedure event, uchar timing);

  void assign_damage(int type, int amount);
  void repair(int amount);  // repairs 'amount' hit points

  void recharge(int amount);  // recharges ship generator by 'amount'

  void calc_direction();  // calculate 'dir_x' and 'dir_y'
};


struct Event  // player event
{
  int data;
  EventProcedure event;
  Event *next;  // linked list
};


/* disabled
struct DamagePacket  // a packet of damage dealt to a ship
{
  int type, amount;
  DamagePacket *next;  // linked list
};
*/


extern uchar curr_round;
extern ShipList *ship_head, *dead_ship_head;


#endif
