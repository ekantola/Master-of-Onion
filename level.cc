/* Objflags:
-Wall -Werror -O -g
*/


#include "level.h"


fixed level_left, level_top, level_right, level_bottom;
BITMAP *level, *background;


bool load_level(const char *name, RGB *palette)
{
  level = load_pcx(name, palette);
  if(level == null)
    return false;

  level_left = itofix(0);
  level_top = itofix(0);
  level_right = itofix(level->w) - 1;
  level_bottom = itofix(level->h) - 1;

  background = create_bitmap(level->w, level->h);
  if(background == null)
    abort();

  blit(level, background, 0, 0, 0, 0, level->w, level->h);

  set_palette(palette);

  return true;
}

