/* Compile:
gcc extkeys.cc keynames.o -lalleg -o extkeys.exe -Wall -O -g
*/


#include "keynames.h"

#include <allegro.h>
#include <etypes.h>
#include <stdio.h>
#include <string.h>


#define KEY_LIST_LENGTH	4

#define KEY_NORMAL	0x0
#define KEY_EXTENDED	0x80


bool prev_key_down[KEY_LIST_LENGTH];

uchar key_scancode[KEY_LIST_LENGTH] =
{
  KEY_RCONTROL, KEY_DOWN|KEY_EXTENDED, KEY_LCONTROL|KEY_EXTENDED, KEY_RIGHT
};


bool key_down(int index)
{
  return key[key_scancode[index] & 0x7F] == 1 + (key_scancode[index]>>7);
}


int main()
{
  int i;

  puts("Keys:");

  for(i=0; i<KEY_LIST_LENGTH; ++i)
    puts(key_name[key_scancode[i]]);

  bzero(prev_key_down, 4 * sizeof(bool));

  allegro_init();
  install_keyboard();

  while(!key[KEY_ESC])
  {
    for(i=0; i<KEY_LIST_LENGTH; ++i)
    {
      if(key_down(i))
      {
	if(!prev_key_down[i])
	{
	  printf("%s pressed\n", key_name[key_scancode[i]]);
	  prev_key_down[i] = true;
	}
      }
      else
      {
	if(prev_key_down[i])
	{
	  printf("%s released\n", key_name[key_scancode[i]]);
	  prev_key_down[i] = false;
	}
      }
    }
  }
}
