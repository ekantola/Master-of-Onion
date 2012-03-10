#include "keynames.h"

#include <string.h>


const char *key_name[KEY_MAX] =
{
  "", "A", "B", "C", "D", "E", "F", "G",
  "H", "I", "J", "K", "L", "M", "N", "O",
  "P", "Q", "R", "S", "T", "U", "V", "W",
  "X", "Y", "Z", "0", "1", "2", "3", "4",
  "5", "6", "7", "8", "9", "NP Ins", "NP End", "NP Down",
  "NP PgDn", "NP Left", "NP 5", "NP Right", "NP Home", "NP Up", "NP PgUp", "F1",
  "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9",
  "F10", "F11", "F12", "Esc", "Tilde", "Minus", "Equals", "Backspace",
  "Tab", "Open Brace", "Close Brace", "Enter", "Colon", "Quote", "Backslash", "Backslash2",
  "Comma", "Stop", "Slash", "Space", "Insert", "Delete", "Home", "End",
  "Page Up", "Page Down", "Left", "Right", "Up", "Down", "NP Slash", "Asterisk",
  "NP Minus", "NP Plus", "NP Del", "NP Enter", "PrtScr", "Pause", "Abnt C1", "Yen",
  "Kana", "Convert", "No Convert", "At", "Circumflex", "Colon2", "Kanji", "LShift",
  "RShift", "LControl", "RControl", "Alt", "Alt Gr", "LWin", "RWin", "Menu",
  "ScrLock", "Num Lock", "Caps Lock"
};


int scancode(const char *name)
{
  int i;

  for(i=0; i<KEY_MAX; ++i)
    if(strcasecmp(name, key_name[i]) == 0)
      return i;

  return 0;
}
