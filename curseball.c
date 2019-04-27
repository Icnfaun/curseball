/*
 * Curseball - A curses based baseball game/simulation.
 * 
 * Creator   - Mason Snyder
 * Version   - 0.0
 */



#include <ncurses.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "curseball.h"

#define FIELD (0)
#define BAT   (1)
#define PITCH (2)
#define UP    (0)
#define DOWN  (1)
#define MAX_LINE_LEN (1000)

int main() {
  
  //initializing screen
  initscr();
  noecho();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  keypad(stdscr, TRUE);
  WINDOW *game_w = newwin(25, 50, 0, 0);
  
  //menus
  char *current_menu = malloc(sizeof(char) * 25);
  strcpy(current_menu, "menus/main_menu");
  while (strcmp(current_menu, "menus/exit") != 0) {
    printw("%s", current_menu);
    refresh();
    sleep(2);
    display_menu(game_w, &current_menu);
    werase(game_w);
  }
  //main game loop
  //draw_scene(FIELD, game_w);
  
  //closing screen
  free(current_menu);
  endwin();
}

void display_menu (WINDOW *window ,char **filename_p) {
  char *filename = (*filename_p);
  menu_n *menu = create_menu(filename);
  if (menu == NULL) {
    strcpy((*filename_p), "menus/exit");
    return;
  }
  int selected = menu_select_init(menu);
  draw_menu(window, menu, selected);
  int key = getch();
  while (key != KEY_RIGHT) {
    if (key == KEY_UP) {
      selected = menu_select(menu, selected, UP);
    }
    if (key == KEY_DOWN) {
      selected = menu_select(menu, selected, DOWN);
    }
    draw_menu(window, menu, selected);
    key = getch();
  }
  char new_menu[25] = "menus/"; 
  strcpy((*filename_p),strcat(new_menu,get_link(menu, selected)));
  free_menu(menu);
}

menu_n *create_menu(char *filename) {
  FILE *read_file = NULL;
  read_file = fopen(filename, "r");
  if (read_file == NULL) {
    return NULL;
  }
  menu_n *head = NULL;
  menu_n *traversal_node = NULL;
  char current_line[MAX_LINE_LEN];
  int current_color = 0;
  int current_select = 0;
  int current_node = 0;
  while(fgets(current_line
        , sizeof(current_line), read_file) != NULL) {
    char node_contents[MAX_LINE_LEN];
    char node_link[MAX_LINE_LEN];
    if (current_node == 0) {
      traversal_node = malloc(sizeof(menu_n));
      sscanf(current_line, "%d,%d,%[^,],%[^\n]\n"
             ,&traversal_node->color
             ,&traversal_node->selectable
             ,node_contents
             ,node_link);
      traversal_node->content 
        = malloc(sizeof(char) * (strlen(node_contents) + 1));
      traversal_node->link 
        = malloc(sizeof(char) * (strlen(node_link) + 1));
      strcpy(traversal_node->content, node_contents);
      strcpy(traversal_node->link, node_link);
      traversal_node->next = NULL;
      traversal_node->prev = NULL;
      head = traversal_node; 
    }
    else {
      traversal_node->next = malloc(sizeof(menu_n)); 
      sscanf(current_line, "%d,%d,%[^,],%[^\n]\n"
             ,&traversal_node->next->color
             ,&traversal_node->next->selectable
             ,node_contents
             ,node_link);
      traversal_node->next->content 
        = malloc(sizeof(char) * (strlen(node_contents) + 1));
      traversal_node->next->link 
        = malloc(sizeof(char) * (strlen(node_link) + 1));
      strcpy(traversal_node->next->content, node_contents);
      strcpy(traversal_node->next->link, node_link);
      traversal_node->next->next = NULL;
      traversal_node->next->prev = traversal_node;
      traversal_node = traversal_node->next;
    }
    current_node++;
  }
  fclose(read_file);
  return head;
}

void free_menu(menu_n *menu) {
  menu_n *traversal_node = menu;
  while (menu != NULL) {
    traversal_node = menu->next;
    free(menu->content);
    menu->content = NULL;
    free(menu->link);
    menu->link = NULL;
    free(menu);
    menu->next = NULL;
    menu->prev = NULL;
    menu = traversal_node;
  }
}

void draw_menu(WINDOW *game_w, menu_n *menu, int selected) {
  menu_n *traversal_node = menu;
  int col = 1;
  int total_multilines = 0;
  while (traversal_node != NULL) {
    char *current_segment = malloc(sizeof(char) * 48);
    int num_segments = strlen(traversal_node->content)/48;
    if (strlen(traversal_node->content) % 48 != 0) {
      num_segments += 1;
    }
    for (int i = 0; i < num_segments; i++) {
      strncpy(current_segment
           , traversal_node->content + (i * 48), 48);
      if ((col - total_multilines) == (2 * selected) - 1) {
        wattron(game_w, COLOR_PAIR(2));
      }
      mvwprintw(game_w, col, 1, current_segment);
      if ((col - total_multilines) == (2 * selected) - 1) {
        wattroff(game_w, COLOR_PAIR(2));
      }
      if (i != num_segments - 1) {
        col++;
      }
      if (i > 0) {
        total_multilines++;
      }
    }
    traversal_node = traversal_node->next;
    free(current_segment);
    current_segment = NULL;
    col += 2;
  }
  move(((selected*2)-1) + total_multilines, 1);
  box(game_w, 0,0);
  wrefresh(game_w);
}

int menu_select_init(menu_n *menu) {
  menu_n *traversal_node = menu;
  int current_node = 1;
  while(traversal_node->selectable != 1) {
    traversal_node = traversal_node->next;
    current_node++;
  }
  return current_node;
}

int menu_select(menu_n *menu, int selected_node, int direction) {
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
}

char *get_link(menu_n *menu, int selected) {
  menu_n *traversal_node = menu;
  int current_node = 1;
  while (traversal_node != NULL) {
    if (current_node == selected) {
      return traversal_node->link;
    }
    traversal_node = traversal_node->next;
    current_node++;
  }
  return menu->link;
}

void draw_scene(int scene, WINDOW *game_w) {
  switch(scene) {
    case FIELD:
      return;
    case BAT:
      return;
    case PITCH:
      return;
  }
}

