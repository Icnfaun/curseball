/*
 * Curseball - Menu System
 *
 * Creator: Mason Snyder
 * Version 0.0
 */

#include <ncurses.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "curseball.h"
#include "curse_sound.h"
#include "curse_menu.h"

#define UP    (0)
#define DOWN  (1)
#define START (1)
#define WHITESPACE (1)
#define MAX_LINE_LEN (1000)

/*
 * Unallocates all allocated space taken by team files, sdl, and ncurses
 */
void free_game() {
  free(team_directories);
  sdl_shutdown();
  endwin();
} /* free_game() */

/*
 * Main function for creating and maintaining menus. initializes window size,
 * starts up first menu (to be changed to be more modular), and draws menus
 * that are selected until "menus/exit" is the next menu to be called. Then it
 * calls closing and freeing functions.
 */
void menu() {
  game_w = newwin(settings[0]/2, settings[0], 0, 0);
  char *current_menu = malloc(sizeof(char) * 25);
  strcpy(current_menu, "menus/main_menu");
  while (strcmp(current_menu, "menus/exit") != 0) {
    display_menu(game_w, &current_menu);
  }
  free(current_menu);
} /* menu() */

/*
 * This function initializes ncurses and sdl so certain settings like
 * color and hiding the cursor work.
 */
void init_libs() {
  initscr();
  noecho();
  start_color();
  curs_set(0);
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  keypad(stdscr, TRUE);
  sdl_startup();
} /* init_libs() */

/*
 * This function takes in a filename (by default the file "curse_config" is used)
 * and uses tags to initialize settings. For example the file will be read up to "="
 * and the word before "=" will be parsed and set to whatever is after "="
 *
 * Format: Setting=Value
 */
void init_config(char * filename) {
  FILE *config_file = fopen(filename, "r");
  if (config_file == NULL) {
    return;
  }
  char current_line[MAX_LINE_LEN];
  while (fgets(current_line, MAX_LINE_LEN, config_file) != NULL) {
    char current_setting[MAX_LINE_LEN];
    if (sscanf(current_line, "%[^=]=", current_setting) == 1) {
      if (strncmp("window_size", current_setting, 11) == 0) {
        sscanf(current_line, "%*[^=]=%i", &settings[0]);
        wscreen_width = settings[0] - 2;
        wscreen_height = settings[0]/2 -2;
      }
      if (strncmp("difficulty", current_setting, 10) == 0) {
        sscanf(current_line, "%*[^=]=%i", &settings[2]);
      }
      if (strncmp("sound", current_setting, 5) == 0) {
        sscanf(current_line, "%*[^=]=%i", &settings[1]);
      }
      if (strncmp("new_dir", current_setting, 7) == 0) {
        if (num_directories < 10) {
          sscanf(current_line, "%*[^=]=%[^s]", (team_directories + num_directories));
          num_directories++;
        }
      }
    }
  }
  fclose(config_file);
  config_file = NULL;
} /* init_config() */

/*
 * This function displays a menu file onto an ncurses window, and also parses keypresses
 */
void display_menu (WINDOW *window , char **filename_p) {
  char *filename = (*filename_p);
  menu_n *menu = create_menu(filename);
  if (menu == NULL) {
    strcpy((*filename_p), "menus/exit");
    return;
  }
  int selected = menu_select_init(menu);
  int key = -1;
  menu_n *current_node = NULL;
  while ((key != 10) || (get_selected(menu, selected)->type != 1)) {
    werase(window);
    refresh();
    draw_menu(window, menu, selected);
    key = getch();
    current_node = get_selected(menu, selected);
    if (key == KEY_UP) {
      selected = menu_select(menu, selected, UP);
    }
    if (key == KEY_DOWN) {
      selected = menu_select(menu, selected, DOWN);
    }
    if (key == KEY_RIGHT) {
      current_node->link = current_node->link->next;
    }
    if (key == KEY_LEFT) {
      current_node->link = current_node->link->prev;
    }
    if ((key == 10) && (current_node->type == 2)) {
      current_node->select_function(setting_finder(current_node->content), atoi(current_node->link->link_c));
      free_menu(menu);
      clear();
      return;
    }
  }
  char new_menu[25] = "menus/"; 
  strcpy(filename, strcat(new_menu, get_selected(menu, selected)->link->link_c));
  free_menu(menu);
} /* display_menu() */

/*
 * This function takes in a filename for a menu file and creates an allocated doubly linked list
 * (that can be seen in the header file) for easy displaying and use in other functions
 */
menu_n *create_menu(char *filename) {
  FILE *read_file = fopen(filename, "r");
  if (read_file == NULL) {
    return NULL;
  }  
  menu_n *head = NULL;
  menu_n *traversal_node = NULL;
  menu_n *previous_node = NULL;
  char current_line[MAX_LINE_LEN];
  int current_node = 0;
  while (fgets(current_line, sizeof(current_line), read_file) != NULL) {
    char node_contents[MAX_LINE_LEN];
    int num_links = 0;
    int bytes_read = 0;
    int new_bytes = 0;
    traversal_node = malloc(sizeof(menu_n));
    sscanf(current_line, "%d,%d,%[^,],%i,%n"
           , &traversal_node->type
           , &traversal_node->selectable
           , node_contents
           , &num_links
           , &bytes_read);
    switch(traversal_node->type) {
      case 1:
        traversal_node->draw_function = &draw_plain_text;
        traversal_node->select_function = NULL;
        break;
      case 2:
        traversal_node->draw_function = &draw_option;
        traversal_node->select_function = &option_select;
        break;
      case 3:
        break;
    }
    traversal_node->content = malloc(sizeof(char) * (strlen(node_contents) + 1));
    char current_link[MAX_LINE_LEN];
    link_n *previous_link = NULL;
    for (int i = 0; i < num_links; i++) {
      sscanf(current_line + bytes_read, "%s%*c%n", current_link, &new_bytes);
      bytes_read += new_bytes;
      traversal_node->link = malloc(sizeof(struct link_node));
      traversal_node->link->link_c = malloc(sizeof(char) * (strlen(current_link) + 1));
      strcpy(traversal_node->link->link_c, current_link);
      traversal_node->link->next = NULL;
      traversal_node->link->prev = previous_link;
      if (previous_link != NULL) {
        previous_link->next = traversal_node->link;
      }
      previous_link = traversal_node->link;
      if (i != num_links - 1) {
        traversal_node->link = traversal_node->link->next;
      }
    }
    if (previous_link != NULL) {
      while (previous_link->prev != NULL) {
        previous_link = previous_link->prev;
      }
      previous_link->prev = traversal_node->link;
      traversal_node->link->next = previous_link;
    }
    else {
      traversal_node->link->next = traversal_node->link;
      traversal_node->link->prev = traversal_node->link;
    }
    strcpy(traversal_node->content, node_contents);
    if (current_node != 0) {
      previous_node->next = traversal_node;
      traversal_node->prev = previous_node;
    }
    else {
      head = traversal_node;
      traversal_node->prev = NULL;
    }
    traversal_node->next = NULL;
    previous_node = traversal_node;
    traversal_node += 1;
    current_node++;
  }
  fclose(read_file);
  return head;
} /* create_menu() */

/*
 * This function frees allocated menu nodes, although I have not thoroughly tested this.
 */
void free_menu(menu_n *menu) {
  execute_sfx("menu_select.wav");
  menu_n *traversal_node = menu;
  while (menu != NULL) {
    traversal_node = menu->next;
    free(menu->content);
    menu->content = NULL;
    menu_n *traversal_link = menu;
    while (traversal_link->link != NULL) {
      menu->link = menu->link->next;
      free(traversal_link->link->link_c);
      traversal_link->link->link_c = NULL;
      free(traversal_link->link);
      traversal_link->link = NULL;
      traversal_link->prev = NULL;
      traversal_link->next = NULL;
      traversal_link = menu;
    }
    free(menu);
    menu->next = NULL;
    menu->prev = NULL;
    menu = traversal_node;
  }
} /* free_menu() */

/*
 * This function draws contents of menu files to the window specified
 * it returns the number of lines that it took to write the contents
 */
int draw_lines(char *node_contents, int col, WINDOW *game_w) {
  int extra_cols = 0;
  int current_line = START;
  char *current_word = malloc(sizeof(char) * wscreen_width);
  int total_chars_read = 0;
  int cur_chars_read = 0;
  while (total_chars_read < strlen(node_contents)) {
    sscanf(node_contents + total_chars_read , "%s%n", current_word, &cur_chars_read);
    if ((strlen(current_word) + current_line) > wscreen_width) {
      current_line = START;
      extra_cols++;
      col++;
    }
    total_chars_read += cur_chars_read;
    mvwprintw(game_w, col, current_line, current_word);
    current_line += strlen(current_word) + WHITESPACE;
  }
  return extra_cols;
} /* draw_lines() */

/*
 * This function is the "draw_function" of plain text menu nodes
 * This includes selectable links and just text.
 */
int draw_plain_text(menu_n *node, int col, WINDOW *game_w) {
  return draw_lines(node->content, col, game_w);
} /* draw_plain_text() */

/*
 * This function is the "draw_function" for options/settings that can be adjusted.
 */
int draw_option(menu_n *node, int col, WINDOW *game_w) {
  char full_line[MAX_LINE_LEN];
  strcpy(full_line, node->content);
  strcat(full_line, " <");
  strcat(full_line, node->link->link_c);
  strcat(full_line, ">");
  return draw_lines(full_line, col, game_w);
} /* draw_option() */

/*
 * This function goes through each node in a menu list and draws it to the
 * given window, it also highlights the current selected node
 */
void draw_menu(WINDOW *game_w, menu_n *menu, int selected) {
  menu_n *traversal_node = menu;
  int col = 1;
  int offset = 0;
  int total_multilines = 0;
  int current_multilines = 0;
  int current_node = 1;
  while (traversal_node != NULL) {
    if (current_node == selected) {
      if (col > wscreen_height + 1) {
        werase(game_w);
        col = 1;
      }
      wattron(game_w, COLOR_PAIR(2));
    }
    current_multilines = traversal_node->draw_function(traversal_node, col, game_w);
    if (current_node == selected) {
      wattroff(game_w, COLOR_PAIR(2));
    }
    total_multilines += current_multilines;
    col += current_multilines;
    traversal_node = traversal_node->next;
    current_node++;
    col += 2;
  }
  box(game_w, 0, 0);
  wrefresh(game_w);
} /* draw_menu() */

/*
 * finds the first selectable node and returns how many nodes in it is.
 */
int menu_select_init(menu_n *menu) {
  menu_n *traversal_node = menu;
  int current_node = 1;
  while(traversal_node->selectable != 1) {
    traversal_node = traversal_node->next;
    current_node++;
  }
  return current_node;
} /* menu_select_init() */

/*
 * Takes in a selected direction from a specified node, and finds/returns
 * the first available selectable node.
 */
int menu_select(menu_n *menu, int selected_node, int direction) {
  execute_sfx("menu_beep.aiff");
  menu_n *traversal_node = menu;
  int current_node = 1;
  while (current_node != selected_node) {
    traversal_node = traversal_node->next;
    current_node++;
  }
  if (direction == UP) {
    while (traversal_node->prev != NULL) {
      traversal_node = traversal_node->prev;
      current_node--;
      if (traversal_node->selectable == 1) {
        return current_node;
      }
    }
  }
  else {
    while (traversal_node->next != NULL) {
      traversal_node = traversal_node->next;
      current_node++;
      if (traversal_node->selectable == 1) {
        return current_node;
      }
    }
  }
  return selected_node;
} /* menu_select() */

/*
 * Returns the menu node that is currently selected.
 */
menu_n *get_selected(menu_n *menu, int selected) {
  menu_n *traversal_node = menu;
  int current_node = 1;
  while (traversal_node != NULL) {
    if (current_node == selected) {
      return traversal_node;
    }
    traversal_node = traversal_node->next;
    current_node++;
  }
  return NULL;
} /* get_selected() */

/*
 * changes a selected setting to a specified value
 */
void option_select(int option, int new_value) {
  settings[option] = new_value;
  refresh_window_size();
  return;

} /* option_select() */

/*
 * Really stupid way to handle settings, parses setting name and matches it to
 * actual setting.
 */
int setting_finder(char *setting) {
  //For sake of not cluttering menu files, just put your options in here and strcmp
  //Settings are in an array of ints, this function returns the correct int for the setting
  //look in curse_menu.h for defines
  if (strcmp(setting, "Sound:") == 0) {
    return SOUND;
  }
  if (strcmp(setting, "Window size:") == 0) {
    return WINDOW_SIZE;
  }
  if (strcmp(setting, "Difficulty:") == 0) {
    return DIFFICULTY;
  }
  return -1;
} /* setting_finder() */

/*
 * adjusts game window when selecting window size setting
 */
void refresh_window_size() {
  game_w = newwin(settings[0]/2, settings[0], 0, 0);
  wscreen_height = settings[0]/2 - 2;
  wscreen_width = settings[0] - 2;
} /* refresh_window_size() */
