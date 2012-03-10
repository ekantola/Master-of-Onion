#ifndef __KEYNAMES_H
#define __KEYNAMES_H


#include <allegro/keyboard.h>


#define KEY_NAME_LIST_LENGTH	KEY_MAX

#define KEY_NORMAL	0x0
#define KEY_EXTENDED	0x80


extern const char *empty;  // a zero-lenght string
extern const char *key_name[KEY_NAME_LIST_LENGTH];


int scancode(const char *name);


#endif
