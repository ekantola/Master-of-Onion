#ifndef __EQUIPMENT_H
#define __EQUIPMENT_H


#include "internal.h"


typedef typeof(void (*)(Shield *shd, int type, int *damage)) ShieldPrevent;
typedef typeof(void (*)(Weapon *wpn)) ActivateWeapon;
typedef typeof(void (*)(Special *spc)) ActivateSpecial;
typedef typeof(void (*)(Special *spc)) DeactivateSpecial;



/******************************** Device types ********************************/

enum DEVICE_TYPE_INDEX
{
  DEV_STANDARD_DRIVE, DEV_FUSION_DRIVE,
  DEV_STANDARD_JETS, DEV_FUSION_JETS,

  DEVICE_TYPE_LIST_LENGTH
};

enum CHARGEABLE_TYPE_INDEX
{
  CHG_NUCLEAR_GENERATOR, CHG_STANDARD_MICROFUSION,

  CHARGEABLE_TYPE_LIST_LENGTH
};

enum SHIELD_TYPE_INDEX
{
  SHD_INTEGRITY_FIELD, SHD_ABSORPTION_SHIELD, SHD_DEFLECTOR_SHIELD,
  SHD_BARRIER_SHIELD, SHD_DAMPER_FIELD, SHD_REFLECTION_FIELD,

  SHIELD_TYPE_LIST_LENGTH
};

enum WEAPON_TYPE_INDEX
{
  WPN_BASIC_CANNON, WPN_VULCAN_CANNON, WPN_GAUSS_CANNON, WPN_LASER_CANNON,
  WPN_PLASMA_TORPEDO,

  WEAPON_TYPE_LIST_LENGTH
};

enum SPECIAL_TYPE_INDEX
{
  SPC_REPAIR_BOT, SPC_CHAMELEON_DEVICE, SPC_AFTERBURNER,

  SPECIAL_TYPE_LIST_LENGTH
};


struct DeviceType  // drives, jets; supertype for all other device types
{
  char *name, *description;
  int mass, max_armor, max_level, power;

  DeviceType(char *name, char *description, int mass, int max_armor, int max_level, int power);
};

struct ChargeableType extends DeviceType  // generators; supertype for shields, weapons & specials
{
  int max_charge, consumption, loading_time, recharge_rate;

  ChargeableType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_rate);
};

struct ShieldType extends ChargeableType
{
  ShieldPrevent prevent;

  ShieldType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_time, ShieldPrevent prevent);
};

struct WeaponType extends ChargeableType
{
  bool type_toggle;
  ShotType *shot_type;

  ActivateWeapon activate;

  WeaponType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_time, bool type_toggle, ShotType *shot_type, ActivateWeapon activate);
};

struct SpecialType extends ChargeableType
{
  ActivateSpecial activate;
  DeactivateSpecial deactivate;

  SpecialType(char *name, char *description, int mass, int max_armor, int max_level, int power, int max_charge, int consumption, int loading_time, int recharge_time, ActivateSpecial activate, DeactivateSpecial deactivate);
};


extern DeviceType device_type[DEVICE_TYPE_LIST_LENGTH];
extern ChargeableType chargeable_type[CHARGEABLE_TYPE_LIST_LENGTH];
extern ShieldType shield_type[SHIELD_TYPE_LIST_LENGTH];
extern WeaponType weapon_type[WEAPON_TYPE_LIST_LENGTH];
extern SpecialType special_type[SPECIAL_TYPE_LIST_LENGTH];


/********************************** Devices ***********************************/

struct Device  // drives, jets; superclass for all other devices
{
  int mass, armor, level, power;
  Ship *master;
  DeviceType *device_type;

  Device(DeviceType *device_type, int level, Ship *master);
};


struct Chargeable extends Device  // generators; superclass for shields, weapons & specials
{
  int charge, consumption, loading;

  Chargeable(ChargeableType *chargeable_type, int level, Ship *master);
};


struct Shield extends Chargeable
{
  ShieldPrevent prevent;

  Shield(ShieldType *shield_type, int level, Ship *master);
};


struct Weapon extends Chargeable
{
  ShotType *shot_type;

  ActivateWeapon activate;

  Weapon(WeaponType *weapon_type, int level, Ship *master);
};


struct Special extends Chargeable
{
  int data;  // 4 bytes of device-specific data

  ActivateSpecial activate;
  DeactivateSpecial deactivate;

  Special(SpecialType *special_type, int level, Ship *master);
};


#endif
