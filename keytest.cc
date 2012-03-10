/* Compile:
gcc keytest.cc misc.o -lalleg -o keytest.exe -Wall -Werror -O -g
*/


#include "internal.h"
#include "keynames.h"

#include <allegro/keyboard.h>
#include <stdio.h>
#include <string.h>


int main()
{
  int prev_key[KEY_MAX];
  int i;
  
  allegro_init();
  
  if(set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) != 0)  // gfx init fails
  {
    allegro_exit();
    puts(allegro_error);
    exit(-1);
  }
  
  install_keyboard();

  memset(prev_key, 0, KEY_MAX*sizeof(int));

  for(i=0; i<KEY_MAX; ++i)
    printf("%-8.7s", key_name[i]);
  
  printf("\n\n");

  fflush(stdout);

  do
  {
    for(i=0; i<KEY_MAX; ++i)
    {
      if(key[i] != prev_key[i])
      {
	prev_key[i] = key[i];
	printf("<%s%s %d>", (!key[i] ? "/" : ""), key_name[i], i);
	fflush(stdout);
      }
    }
  }
  while(!key[KEY_ESC]);
  
  return -1;
}
END_OF_MAIN()
