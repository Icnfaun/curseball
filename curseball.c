/*
 * Curseball - A curses based baseball game/simulation.
 * 
 * Creator   - Mason Snyder
 * Version   - 0.0
 */

#include <ncurses.h>

#include "curseball.h"
#include "curse_sound.h"
#include "curse_menu.h"

int settings[3];
int wscreen_width = 0;
int wscreen_height = 0;
char **team_directories = NULL;
int num_directories = 0;
WINDOW *game_w = NULL;


int main() {
  init_libs();
  init_config("curse_config");
  menu();
  free_game();
}
