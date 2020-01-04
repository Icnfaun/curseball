/*
 * Curseball - A curses based baseball game/simulation.
 * 
 * Creator   - Mason Snyder
 * Version   - 0.0
 */

#include <ncurses.h>
#include <malloc.h>
#include <string.h>


#include "curseball.h"
#include "curse_sound.h"
#include "curse_menu.h"
#include "curse_player_control.h"
#include "curse_utilities.h"

char *start_menu = "menus/main_menu";
int settings[3];
int wscreen_width = 0;
int wscreen_height = 0;
char **team_directories = NULL;
int num_directories = 0;
WINDOW *game_w = NULL;


int main() {
  menu_n *tester = malloc(sizeof(menu_n));
  tester->link = malloc(sizeof(link_n));
  tester->link->link_c = strdup("vottojo01");
  player_info_menu(tester);
  char *testing = get_player_id_lines("Fielding.csv", "vottojo01");
  int total = -1;
  char **testing2 = get_spreadsheet_info(&total, "Fielding.csv", "vottojo01", 5);
  char **testing3 = get_spreadsheet_info(NULL, "Fielding.csv", "vottojo01", 6);
  int *frequencies = char_to_int_array(testing3, total);
  //printf("%d", total);
  for (int i =0; i< total; i++) {
    //printf("%s: %d\n", (*(testing2+i)), (*(frequencies + i)));
  }
  //printf("%s", testing);
  //printf("%d", evaluate_main_position("vottojo01")); 
  init_libs();
  init_config("curse_config");
  menu();
  free_game();
}
