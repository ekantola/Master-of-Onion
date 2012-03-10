/* Objflags:
-Wall -Werror -O -g
*/


#include "equipment.h"

#include "players.h"
#include "ships.h"
#include "shots.h"



/********************************** Devices ***********************************/

/* values:
name, description,
mass, max_armor, max_level, power
*/

DeviceType device_type[DEVICE_TYPE_LIST_LENGTH] =
{
// ship drives
  DeviceType("Standard Drive", "...",
             1200, 2000, 5, 300000),

  DeviceType("Fusion Drive", "...",
             1500, 2500, 4, 380000),

// turning jets
  DeviceType("Standard Jets", "...",
             300, 1100, 5, 30000),

  DeviceType("Fusion jets", "...",
             370, 1350, 4, 38000)
};



/* values:
mass, max_armor, max_level, power, max_charge, consumption, loading_time,
  recharge_rate
*/


ChargeableType chargeable_type[CHARGEABLE_TYPE_LIST_LENGTH] =
{
  ChargeableType("Nuclear Generator", "...",
             800, 2000, 4, 0, 10000, 0, 0, 9),

  ChargeableType("Standard MicroFusion", "...",
             1050, 2200, 6, 0, 14000, 0, 0, 12)
};



/********************************** Shields ***********************************/

void shd_integrity_field(Shield *shd, int type, int *damage);
void shd_absorption_shield(Shield *shd, int type, int *damage);
void shd_deflector_shield(Shield *shd, int type, int *damage);
void shd_barrier_shield(Shield *shd, int type, int *damage);
void shd_damper_field(Shield *shd, int type, int *damage);
void shd_reflection_field(Shield *shd, int type, int *damage);


/* values:
name, description,
mass, max_armor, max_level, power (=dmg threshold), max_charge, consumption,
  loading_time, recharge_time,
prevent
*/

ShieldType shield_type[SHIELD_TYPE_LIST_LENGTH] =
{
  ShieldType("Integrity Field", "Integrity fields are basic shield-like ship protection devices, mainly used by freighters and other non-combat sips.",
             800, 1400, 5, 70, 10000, 10, 0, 10,
             shd_integrity_field),

  ShieldType("Absorption Shield", "An absorption shield provides more protection against energy weapon attacks than integrity fields do, and doesn't leak but a little. Part of the absorbed energy is directed in the ship'shot batteries.",
             1500, 2050, 8, 120, 20000, 15, 0, 15,
             shd_absorption_shield),

  ShieldType("Deflector Shield", "A deflector shield, as it's name implies, deflects a part of the attacs back. Like reflection fields, but in smaller scale, they direct some amounts of energy back to the attacker; the advantage is the deflectors' shield-like nature.",
             1300, 1800, 8, 100, 15000, 15, 0, 13,
             shd_deflector_shield),

  ShieldType("Barrier Shield", "Barrier shield, \"The Protector\": extremely powerful and almost impenetrable until completely fallen. The side effect is, however, the huge energy consumption. Barrier shields are used in heavy battlecruisers with powerful generators.",
             2500, 3000, 6, 320, 50000, 60, 0, 20,
             shd_barrier_shield),

  ShieldType("Damper Field", "Unlike the traditional shields, a damper field blocks damage only little, but heavily reduces it instead. The field generator is heavily armored, for the damper field leaves the ship vulnerable by armor piercing weapons.",
             2000, 2000, 8, 15, 0, 20, 0, 0,
             shd_damper_field),

  ShieldType("Reflection Field", "This little device acts somewhat like a damper field, but reflects a part of the attack back instead of just reducing it. Reflection-fielded ships are usually avoided by enemies because of the instant retaliation.",
             2500, 3500, 6, 10, 0, 30, 0, 0,
             shd_reflection_field),
};



/********************************** Weapons ***********************************/

void wpn_basic_cannon(Weapon *wpn);
void wpn_vulcan_cannon(Weapon *wpn);
void wpn_gauss_cannon(Weapon *wpn);
void wpn_beam_weapon(Weapon *wpn);
void wpn_plasma_torpedo(Weapon *wpn);


/* values:
name, description,
mass, max_armor, max_level, power (=damage), max_charge, consumption, loading_time,
  recharge_time, type_toggle, shot_type,
activate
*/

WeaponType weapon_type[WEAPON_TYPE_LIST_LENGTH] =
{
  WeaponType("Basic Cannon", "This is a standard single-shot 40mm ship cannon that uses armor piercing cannon shells.",
             100, 400, 6, 1300, 300, 1, 10, 0, false, &shot_type[SHOT_BULLET],
             wpn_basic_cannon),

  WeaponType("Vulcan Cannon", "Shoots bursts of high velocity shells that can penetrate quite thick armor, too.",
             150, 400, 8, 600, 200, 3, 25, 0, false, &shot_type[SHOT_BULLET],
             wpn_vulcan_cannon),

  WeaponType("Gauss Cannon", "Shoots small particles that have been accelerated to extreme velocities. Can pierce almost any shield and armor.",
             250, 400, 6, 2400, 200, 7, 20, 0, false, &shot_type[SHOT_BULLET],
             wpn_gauss_cannon),

  WeaponType("Laser Cannon", "A basic laser cannon, consumes plain energy instead of physical ammo.",
             150, 500, 8, 60, 0, 40, 0, 0, true, &shot_type[SHOT_BEAM],
             wpn_beam_weapon),

  WeaponType("Plasma Torpedo", "Super-heated bolts of plasma that fry about everything in their way.",
             300, 700, 4, 16000, 10, 120, 150, 1300, false, &shot_type[SHOT_TORPEDO],
             wpn_plasma_torpedo),
};



/******************************* Special devices ******************************/

void spc_repair_bot(Special *spc);
void spc_chameleon_device(Special *spc);
void spc_afterburner(Special *spc);

void spc_chameleon_device_off(Special *spc);

/* values:
name, description,
mass, max_armor, max_level, power, max_charge, consumption, loading_time,
  recharge_time,
activate, deactivate
*/

SpecialType special_type[SPECIAL_TYPE_LIST_LENGTH] =
{
  SpecialType("Repair Bot", "An automated robot being able to repair damaged internal systems and restore armor at a cost of energy. Extremely useful with a damper or reflection field.",
              180, 1200, 8, 2, 0, 15, 0, 0,
              spc_repair_bot, null),

  SpecialType("Chameleon Device", "This device changes the user's color so that he can effectively blend in the surroundings.",
              400, 800, 3, -1, 0, 20, 0, 0,
              spc_chameleon_device, spc_chameleon_device_off),

  SpecialType("Afterburner", "...",
              650, 1000, 3, 400000, 100, 1, 5, 10,
              spc_afterburner, null),
};





/******************************* Constructing *********************************/

// device types
DeviceType::DeviceType(char *name, char *description, int mass, int max_armor, int max_level, int power)
{
  this->name = name;
  this->description = description;
  this->mass = mass;
  this->max_armor = max_armor;
  this->max_level = max_level;
  this->power = power;
}


ChargeableType::ChargeableType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_rate): DeviceType(name, description, mass, max_armor, max_level, power)
{
  this->max_charge = max_charge;
  this->consumption = consumption;
  this->loading_time = loading_time;
  this->recharge_rate = recharge_rate;
}


ShieldType::ShieldType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_rate, ShieldPrevent prevent): ChargeableType(name, description, mass, max_armor, max_level, power, max_charge, consumption, loading_time, recharge_rate)
{
  this->prevent = prevent;
}


WeaponType::WeaponType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_time, bool type_toggle, ShotType *shot_type, ActivateWeapon activate): ChargeableType(name, description, mass, max_armor, max_level, power, max_charge, consumption, loading_time, recharge_time)
{
  this->type_toggle = type_toggle;
  this->shot_type = shot_type;
  this->activate = activate;
}


SpecialType::SpecialType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_time, ActivateSpecial activate, DeactivateSpecial deactivate): ChargeableType(name, description, mass, max_armor, max_level, power, max_charge, consumption, loading_time, recharge_time)
{
  this->activate = activate;
  this->deactivate = deactivate;
}


// devices
Device::Device(DeviceType *device_type, int level, Ship *master)
{
  fixed f = itofix(1) + (itofix(level-1) >> 1);  // 1 + (level-1)/2

  this->mass = fmul(f, device_type->mass);
  this->armor = device_type->max_armor;
  this->level = level;
  this->power = fmul(f, device_type->power);
  this->master = master;
  this->device_type = device_type;
}


Chargeable::Chargeable(ChargeableType *chargeable_type, int level, Ship *master): Device(chargeable_type, level, master)
{
  fixed f = itofix(1) + (itofix(level-1) >> 1);  // 1 + (level-1)/2

  this->charge = 0;
  this->consumption = fmul(f, chargeable_type->consumption);
  this->loading = 0;
}


Shield::Shield(ShieldType *shield_type, int level, Ship *master): Chargeable(shield_type, level, master)
{
  this->prevent = shield_type->prevent;
}


Weapon::Weapon(WeaponType *weapon_type, int level, Ship *master): Chargeable(weapon_type, level, master)
{
  this->shot_type = weapon_type->shot_type;
  this->activate = weapon_type->activate;
}


Special::Special(SpecialType *special_type, int level, Ship *master): Chargeable(special_type, level, master)
{
  this->data = 0;
  this->activate = special_type->activate;
  this->deactivate = special_type->deactivate;
}



/****************************** Using shields *********************************/

void shd_integrity_field(Shield *shd, int type, int *damage)
{
  shd->charge -= *damage;

  if(shd->charge < 0)  // the shield went down and lets the rest of the dmg through
  {
    *damage = -shd->charge;
    shd->charge = 0;
  }
  else  // all damage was dealt to shields
    *damage = 0;
}

void shd_absorption_shield(Shield *shd, int type, int *damage)
{
  shd->charge -= (*damage);
  shd->master->recharge((*damage) >> 3);

  if(shd->charge < 0)  // the shield went down and lets the rest of the dmg through
  {
    *damage = -shd->charge;
    shd->charge = 0;
  }
  else  // all damage was dealt to shields
    *damage = 0;
}

void shd_deflector_shield(Shield *shd, int type, int *damage)
{
  shd->charge -= (*damage);

  if(shd->charge < 0)  // the shield went down and lets the rest of the dmg through
  {
    *damage = -shd->charge;
    shd->charge = 0;
  }
  else  // all damage was dealt to shields
    *damage = 0;
}

void shd_barrier_shield(Shield *shd, int type, int *damage)
{
  shd->charge -= (*damage);

  if(shd->charge < 0)  // the shield went down and lets the rest of the dmg through
  {
    *damage = -shd->charge;
    shd->charge = 0;
  }
  else  // all damage was dealt to shields
    *damage = 0;
}

void shd_damper_field(Shield *shd, int type, int *damage)
{
  *damage /= (shd->power + 2);
}

void shd_reflection_field(Shield *shd, int type, int *damage)
{
  *damage /= (shd->power + 2);
}



/***************************** Using weapons **********************************/

#define BULLET_START	ftofix(4.5)


extern BITMAP *level, *background;


// bullet weapons
void use_bullet_weapon(Weapon *wpn, int shot_mass, int speed_coef)
{
  fixed f;
  Shot *shot;
  Ship *s = wpn->master;

  shot = new Shot(wpn, s->x + fmul(BULLET_START, s->dir_x),
		       s->y + fmul(BULLET_START, s->dir_y),
		       s->speed_x + speed_coef * s->dir_x,
		       s->speed_y + speed_coef * s->dir_y);

  shot->mass = shot_mass;

  f = itofix(shot_mass) / s->mass;

  s->speed_x -= fmul(f, shot->speed_x);
  s->speed_y -= fmul(f, shot->speed_y);
}

void event_vulcan_cannon(Ship *s, int data)
{
  use_bullet_weapon((Weapon *)data, 5, 4 + ((Weapon *)data)->level);
}


void wpn_basic_cannon(Weapon *wpn)
{
  use_bullet_weapon(wpn, 20, 2);
}

void wpn_vulcan_cannon(Weapon *wpn)
{
  int i;
  for(i=0; i<3; ++i)
    wpn->master->add_event((int)wpn, event_vulcan_cannon, i);
}

void wpn_gauss_cannon(Weapon *wpn)
{
  use_bullet_weapon(wpn, 2, 16 + (wpn->power<<2));
}

void wpn_beam_weapon(Weapon *wpn)
{
  Ship *s;
  Shot *shot;

  s = wpn->master;
  shot = new Shot(wpn, s->x + fmul(BULLET_START, s->dir_x),
		       s->y + fmul(BULLET_START, s->dir_y),
		       s->dir_x, s->dir_y);

  shot->data = 0;
}


// torpedoes etc.
void wpn_plasma_torpedo(Weapon *wpn)
{
  // ...
}



/****************************** Using specials ********************************/

void spc_repair_bot(Special *spc)
{
  spc->master->repair(spc->power);
}


void spc_chameleon_device(Special *spc)
{
  uchar *c;
  RGB color;
  Ship *s;

  s = spc->master;

  if(!spc->data)
    get_color(s->color, (RGB *)&spc->data);  // store the previous color here

  get_color(_getpixel(background, ffloor(s->x), ffloor(s->y)), &color);

  for(c=&color.r; c<=&color.b; ++c)
    *c ^= 0x7;  // modify the three least signigicant bits

  set_color(s->color, &color);
}

void spc_chameleon_device_off(Special *spc)
{
  set_color(spc->master->color, (RGB *)&spc->data);  // restore default color
}

void spc_afterburner(Special *spc)
{
  fixed f;
  Ship *s;

  s = spc->master;
  f = spc->power / s->mass;

  s->speed_x += fmul(f, s->dir_x);
  s->speed_y += fmul(f, s->dir_y);
}
