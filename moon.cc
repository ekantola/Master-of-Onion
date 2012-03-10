#include "colors.h"
#include "config.h"
#include "equipment.h"
#include "keynames.h"
#include "level.h"
#include "misc.h"
#include "players.h"
#include "ships.h"

#include <aliases.h>
#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


enum EXIT_MESSAGE_INDEX
{
  EXIT_ABORT, EXIT_SHIP_VICTORY,

  EXIT_MESSAGE_LIST_LENGTH
};


char key_break, key_pause;
char prev_key[KEY_MAX];
int screen_w, screen_h;
int frames_skipped, max_frameskips;
int exit_message;

int base_working_rate, collision_damage;
fixed gravity, base_turning;
fixed viscosity[VISCOSITY_LIST_LENGTH];

static RGB palette[256];


void init_shots();
void remove_shots();
void draw_shots();
void hide_shots();


// timer handler for the game
volatile int timer_counter;
void timer_handler()
{
  ++timer_counter;
}
END_OF_FUNCTION(timer_handler);


void static_init()
{
  int p;

// players
  Player::ship_plan = load_pcx("shipdev.pcx", palette);
  
  if(Player::ship_plan == null)
  {
    allegro_exit();
    puts("Could not load 'shipdev.pcx'");
    exit(-1);
  }

  for(p=0; p<MAX_PLAYERS; ++p)
    player[p] = new Player(p);

// ship lists (dually linked, circular)
  ship_head->prev = ship_head->next = ship_head;
  dead_ship_head->prev = dead_ship_head->next = dead_ship_head;
}


void init()
{
  int i, k, p;
  int cfg_section;
  Player *pl;
  Ship *s;

  LOCK_VARIABLE(timer_counter);
  LOCK_FUNCTION(timer_handler);

#ifdef DEBUG_FILE
  freopen(DEBUG_FILE, "wb", stdout);
#endif

// init
  srand(time(0));
  init_shots();

  allegro_init();
  install_timer();

  set_config_file("moon.cfg");

  install_keyboard();
  key_led_flag = 0;

// screen dimensions
  screen_w = get_cfg_int(CFG_GAME, "screen_w", 320);
  screen_h = get_cfg_int(CFG_GAME, "screen_h", 200);

  if(set_gfx_mode(GFX_AUTODETECT, screen_w, screen_h, 0, 0) != 0)  // gfx init fails
  {
    allegro_exit();
    puts(allegro_error);
    exit(-1);
  }

  set_clip(screen, 0, 0, screen_w, screen_h);

// game keys
  key_break = scancode(get_cfg_string(CFG_GAME, "key_break", key_name[KEY_ESC]));
  key_pause = scancode(get_cfg_string(CFG_GAME, "key_pause", key_name[KEY_PAUSE]));

// level
  load_level(get_cfg_string(CFG_GAME, "level", "mushroom.lev"), palette);

// physics
  players = get_cfg_int(CFG_GAME, "players", 2);
  base_working_rate = get_cfg_int(CFG_GAME, "base_working_rate", 20);
  collision_damage = get_cfg_int(CFG_GAME, "collision_damage", 100);
  gravity = get_cfg_int(CFG_GAME, "gravity", 100);
  base_turning = get_cfg_int(CFG_GAME, "base_turning", 40000);

  viscosity[VISC_AIR] = get_cfg_int(CFG_GAME, "air_viscosity", 17);
  viscosity[VISC_WATER] = get_cfg_int(CFG_GAME, "water_viscosity", 1000);
  viscosity[VISC_SLIME1] = get_cfg_int(CFG_GAME, "slime1_viscosity", 2000);
  viscosity[VISC_SLIME2] = get_cfg_int(CFG_GAME, "slime2_viscosity", 4000);
  
  static_init();
  
// player window width
  i = (players>1) + 1;
  Player::window_w = screen_w/i - i*6;
  
  if(Player::window_w > level->w)
    Player::window_w = level->w;
  
// player window height
  i = (players>2) + 1;
  Player::window_h = screen_h/i - i*18;
  
  if(Player::window_h > level->h)
    Player::window_h = level->h;
  
  for(p=0; p<players; ++p)
  {
    pl = player[p];
    s = pl->ship;
    
  // player keys
    cfg_section = CFG_PLAYER_1 + p;
    
    for(i=0; i<PLAYER_KEY_LIST_LENGTH; ++i)
    {
      k = scancode(get_cfg_string(cfg_section, cfg_player_key[i], ""));
      
      pl->key_scancode[i] = (k > 0) ? k : default_key[p][i];
    }
    
  // for testing
    s->mass = 2000;
    s->max_armor = 8000;
    s->max_structure = 5000;
    
    s->device[0] = new Weapon(&weapon_type[WPN_BASIC_CANNON], 1, s);
    s->device[1] = new Weapon(&weapon_type[WPN_VULCAN_CANNON], 1, s);
    s->device[2] = new Weapon(&weapon_type[WPN_LASER_CANNON], 1, s);
    s->device[3] = new Special(&special_type[SPC_AFTERBURNER], 1, s);
    s->device[4] = new Special(&special_type[SPC_REPAIR_BOT], 1, s);
    s->device[SHIP_DEV_SHIELD] = new Shield(&shield_type[SHD_INTEGRITY_FIELD], 1, s);

    s->device[SHIP_DEV_DRIVE] = new Device(&device_type[DEV_STANDARD_DRIVE], 2, s);
    s->device[SHIP_DEV_LEFT_JETS] = new Device(&device_type[DEV_FUSION_JETS], 1, s);
    s->device[SHIP_DEV_RIGHT_JETS] = new Device(&device_type[DEV_STANDARD_JETS], 2, s);
    s->device[SHIP_DEV_GENERATOR] = new Chargeable(&chargeable_type[CHG_NUCLEAR_GENERATOR], 1, s);
    
  // init player
    pl->init();
    
    blit(Player::ship_plan, screen, 0, 0, pl->ship_top, pl->ship_left, Player::ship_plan->w, Player::ship_plan->h);
  }
  
// init frameskip
  frames_skipped = 0;
  max_frameskips = get_cfg_int(CFG_GAME, "max_frameskips", 5);

// install game timer
  timer_counter = 1;
  install_int_ex(timer_handler, BPS_TO_TIMER(get_cfg_int(CFG_GAME, "timer_speed", 200)));
}


int game()
{
  int p;
  Player *pl;
  Ship *s;

// main loop
  while(!key[(int)key_break])
  {
    if (key[(int)key_pause]) {
      while(key[(int)key_pause]);
      timer_counter = 0;
    }
    
    ++curr_round;  // next round begins

    hide_shots();

  // hide players form last to first
    s = (Ship *)ship_head->next;
    while((ShipList *)s != ship_head)
      s = s->hide();
    
  // process and draw players from first to last
    s = (Ship *)ship_head->next;
    while((ShipList *)s != ship_head)
      s = s->process_round();
    
  // process any dead players
    s = (Ship *)dead_ship_head->next;
    while((ShipList *)s != dead_ship_head)
      s = s->revive();
    
    draw_shots();
    
  // update screen and handle frameskipping
    --timer_counter;
    
    if((timer_counter == 0) || (frames_skipped > max_frameskips))  // time to blit
    {
      for(p=0; p<players; ++p)
      {
	pl = player[p];
	
	frames_skipped = 0;
	
	blit(level, screen, pl->ship->view_left, pl->ship->view_top,
			    pl->window_left, pl->window_top,
			    Player::window_w, Player::window_h);
	
	if(!pl->dead)
	  pl->update_indicators();
      }
    }
    else if(timer_counter < 0)  // hold back the machine if going too fast
      while(timer_counter <= 0);
    else  // the frame was skipped
      ++frames_skipped;
  }

  return EXIT_ABORT;
}


void deinit()
{
  remove_int(timer_handler);
  remove_shots();

  destroy_bitmap(level);
  destroy_bitmap(background);

  allegro_exit();
}


int main()
{
  init();
  exit_message = game();
  deinit();

  if(exit_message == EXIT_ABORT)
    puts("Game aborted");
  else if(exit_message >= EXIT_SHIP_VICTORY)
    printf("Ship %i won\n", exit_message);
  else
    puts("Abnormal exit");

  return 0;
}
END_OF_MAIN()
