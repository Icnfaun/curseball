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
#include "curse_player_control.h"
#include "curse_utilities.h"

#define UP    (0)
#define DOWN  (1)
#define START (1)
#define WHITESPACE (1)
#define MAX_LINE_LEN (1000)

menu_n *current_menu_g = NULL;
bool menu_changed_g = false;

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
  current_menu_g = create_menu("main_menu");
  while (current_menu_g != NULL) {
    display_menu(game_w);
  }
  free(current_menu_g);
} /* menu() */

/*
 * This function initializes ncurses and sdl so certain settings like
 * color and hiding the cursor work.
 */
void init_libs() {
  initscr();
  noecho();
  cbreak();
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
      if (strncmp("start_menu", current_setting, 10) == 0) {
          start_menu = malloc(sizeof(char) * 25);
          sscanf(current_line, "%*[^=]=%[^\n]", start_menu);
          printf("%s", start_menu);
      }
    }
  }
  fclose(config_file);
  config_file = NULL;
} /* init_config() */

/*
 * This function displays a menu file onto an ncurses window, and also parses keypresses
 */
void display_menu (WINDOW *window) {
  menu_changed_g = false;
  int selected = menu_select_init(current_menu_g);
  int key = -1;
  while (!menu_changed_g) {
    werase(window);
    refresh();
    draw_menu(window, current_menu_g, selected);
    key = getch();
    menu_n *current_node = get_selected(current_menu_g, selected);
    if (current_node->type == 3 && ((key > 31 && key < 127) || (key == 263))) {
      edit_link(current_node, key);
    }
    if (key == KEY_UP) {
      selected = menu_select(current_menu_g, selected, UP);
    }
    if (key == KEY_DOWN) {
      selected = menu_select(current_menu_g, selected, DOWN);
    }
    if (key == KEY_RIGHT) {
      current_node->link = current_node->link->next;
    }
    if (key == KEY_LEFT) {
      current_node->link = current_node->link->prev;
    }
    if (key == 10) {
      if (current_node->select_function != NULL) {
        current_node->select_function(current_node);
      }
      erase();
      return;
    }
  }
  free_menu(current_menu_g);
} /* display_menu() */

/*
 * This function takes in a filename for a menu file and creates an allocated doubly linked list
 * (that can be seen in the header file) for easy displaying and use in other functions
 */
menu_n *create_menu(char *filename) {
  char full_path[25] = "menus/";
  strcat(full_path, filename);
  FILE *read_file = fopen(full_path, "r");
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
    sscanf(current_line, "%d,%d,%d,%[^,],%i,%n"
           , &traversal_node->type
           , &traversal_node->selectable
           , &traversal_node->spaces_after
           , node_contents
           , &num_links
           , &bytes_read);
    switch(traversal_node->type) {
      case 1:
        traversal_node->draw_function = &draw_plain_text;
        traversal_node->select_function = &change_menu;
        break;
      case 2:
        traversal_node->draw_function = &draw_option;
        traversal_node->select_function = &option_select;
        break;
      case 3:
        traversal_node->draw_function = &draw_search_box;
        traversal_node->select_function = NULL;
        break;
      case 4:
        traversal_node->draw_function = &draw_plain_text;
        traversal_node->select_function = &search_player;
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
  execute_sfx("resources/menu_select.wav");
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

void free_menu_node(menu_n *menu) {
  free(menu->content);
  menu->content = NULL;
  menu_n *traversal_link = menu;
  while (traversal_link->link != NULL) {
    free(traversal_link->link->link_c);
    if ((traversal_link->link->next == menu->link) || (traversal_link->link->next = NULL)) {
      free(traversal_link->link);
      traversal_link->link->next = NULL;
      traversal_link->link->prev = NULL;
      traversal_link->link = NULL;
    }
    else {
      traversal_link->link = traversal_link->link->next;
      free(traversal_link->link->prev);
      traversal_link->link->prev->next = NULL;
      traversal_link->link->prev->prev = NULL;
    }
    
  }
  free(menu);
  menu->next = NULL;
  menu->prev = NULL;

}

/*
 * This function draws contents of menu files to the window specified
 * it returns the number of lines that it took to write the contents
 *
 * tabs and newlines within a lines content will break this currently
 * (anything taking up more than one space on the screen)
 */
int draw_lines(char *node_contents, int col, WINDOW *game_w) {
  int extra_cols = 0;
  int current_line = START;
  char *current_word = malloc(sizeof(char) * (strlen(node_contents) + 1));
  char *current_whitespace = malloc(sizeof(char) * (strlen(node_contents) + 1));
  int total_chars_read = 0;
  int cur_chars_read = 0;
  while (total_chars_read < strlen(node_contents)) {
    strcpy(current_word, "");
    strcpy(current_whitespace, "");
    sscanf(node_contents + total_chars_read , "%1000s%n%1000[ ]", current_word, &cur_chars_read, current_whitespace);
    if (strcmp(current_whitespace, "") != 0) {
     strcat(current_word, current_whitespace);
    }
    while (strlen(current_word) >= wscreen_width) {
      char current_chunk[MAX_LINE_LEN] = "";
      strncpy(current_chunk, current_word, wscreen_width - current_line);
      mvwprintw(game_w, col, current_line, current_chunk); 
      current_line = START;
      extra_cols++;
      col++;
      current_word = current_word + strlen(current_chunk);   
    }
    if ((strlen(current_word) + current_line) > wscreen_width) {
      current_line = START;
      extra_cols++;
      col++;
    }
    total_chars_read += cur_chars_read;
    mvwprintw(game_w, col, current_line, current_word);
    current_line += strlen(current_word);
  }
  return extra_cols;
} /* draw_lines() */

/*
 * This function is the "draw function" of search box menu nodes, think of these draw functions
 * more of a format to print with draw_lines()
 */
int draw_search_box(menu_n *node, int col, WINDOW *game_w) {
  char full_line[MAX_LINE_LEN] = "";
  strcpy(full_line, node->content);
  if (node->link->link_c != NULL) {
    strcat(full_line, " ");
    strcat(full_line, node->link->link_c);
  }
  return draw_lines(full_line, col, game_w);
}

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
  char full_line[MAX_LINE_LEN] = "";
  strcpy(full_line, node->content);
  strcat(full_line, " <");
  strcat(full_line, node->link->link_c);
  strcat(full_line, ">");
  return draw_lines(full_line, col, game_w);
} /* draw_option() */

int draw_stats(menu_n *node, int col, WINDOW *game_w) {
  int shift = atoi(node->link->link_c);  
  char visible_text[MAX_LINE_LEN] = "";
  strncpy(visible_text, node->content + shift, wscreen_width-1);
  return draw_lines(visible_text, col, game_w);
}

/*
 * This function goes through each node in a menu list and draws it to the
 * given window, it also highlights the current selected node
 */
void draw_menu(WINDOW *game_w, menu_n *menu, int selected) {
  menu_n *traversal_node = menu; 
  int col = 1;
  int total_multilines = 0;
  int current_multilines = 0;
  int current_node = 1;
  bool selected_on_screen = false;
  while ((col <= wscreen_height-1) && (traversal_node != NULL)) {
    if (current_node == selected) {
      wattron(game_w, COLOR_PAIR(2));
      selected_on_screen = true;
    }
    current_multilines = traversal_node->draw_function(traversal_node, col, game_w);
    if (current_node == selected) {
      wattroff(game_w, COLOR_PAIR(2));
    }
    total_multilines += current_multilines;
    col += current_multilines + traversal_node->spaces_after + 1;
    traversal_node = traversal_node->next;
    current_node++;
  }
  if (!selected_on_screen) {
    werase(game_w);
    draw_menu(game_w, menu->next, selected - 1);
    return;
  }
  box(game_w, 0, 0);
  wrefresh(game_w);
} /* draw_menu() */

/*
 * finds the first selectable node and returns how many nodes deep it is.
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
  execute_sfx("resources/menu_beep.aiff");
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
void option_select(menu_n *current_node) {
  int option_to_change = setting_finder(current_node->content);
  int new_value = atoi(current_node->link->link_c);
  settings[option_to_change] = new_value;
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
 * This edits a seach box's contents based on what you type, not sure if deletion works
 * on different computers yet, might be funky
 */
void edit_link(menu_n *current_node, int typed) {
  char typed_contents[MAX_LINE_LEN];
  int string_length = 0;
  if (current_node->link->link_c != NULL) {
    string_length = strlen(current_node->link->link_c);
    strcpy(typed_contents, current_node->link->link_c);
    free(current_node->link->link_c);
    if (typed == 263) {
      typed_contents[string_length - 1] = '\0';
    }
  }
  if ((typed != 263) && (string_length + 2 < MAX_LINE_LEN)) {
    typed_contents[string_length] = typed;
    typed_contents[string_length + 1] = '\0';
    string_length++;
  }
  if (string_length > 0) {
    current_node->link->link_c = strdup(typed_contents);
  }
  else {
    current_node->link->link_c = strdup("");
  }
}/*  edit_link() */

/*
 * adjusts game window when selecting window size setting
 */
void refresh_window_size() {
  game_w = newwin(settings[0]/2, settings[0], 0, 0);
  wscreen_height = settings[0]/2 - 2;
  wscreen_width = settings[0] - 2;
} /* refresh_window_size() */

/*
 * changes the active menu
 */
void change_menu(menu_n *old_menu) {
  current_menu_g = create_menu(old_menu->link->link_c);
  menu_changed_g = true;
} /* change_menu() */
