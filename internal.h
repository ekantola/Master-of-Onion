#ifndef __INTERNAL_H
#define __INTERNAL_H


#include <aliases.h>
#include <allegro.h>


#define DEBUG
//#define DEBUG_FILE "debug.txt"

#ifdef DEBUG
#include <stdio.h>
#endif


#define MAX_PLAYERS	4


#define extends :public
#define null NULL

typedef unsigned char uchar;




/*********************************** Types ************************************/

struct Player;

struct ShipList;
struct Ship;
struct Event;

struct Shot;
struct ShotType;

struct DeviceType;
struct ChargeableType;
struct ShieldType;
struct WeaponType;
struct SpecialType;

struct Device;
struct Chargeable;
struct Shield;
struct Weapon;
struct Special;



/********************************** Inlines ***********************************/

inline fixed fsq(fixed f)            { return fmul(f, f); }
inline fixed dist(fixed a, fixed b)  { return fsqrt(fsq(a) + fsq(b)); }


#endif
