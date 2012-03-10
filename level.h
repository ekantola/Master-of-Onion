#ifndef __LEVEL_H
#define __LEVEL_H


#include "internal.h"


extern fixed level_left, level_top, level_right, level_bottom;
extern BITMAP *level, *background;


bool load_level(const char *name, RGB *palette);


#endif
