#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <ncurses.h>

#include "curse_menu.h"

/* 
 * First string is assumed to be malloced second is not
 */
char *combine_string(char **first, char *second) {
  char *combined = malloc(sizeof(char) * (strlen((*first)) + strlen(second) + 1));
  strcpy(combined, (*first));
  strcat(combined, second);
  free((*first));;
  (*first) = combined;
  return (*first);
} /* combine_string() */

/*
 * Combines 2 string arrays, results put into the pointer for the first
 */
void combine_string_arrays(char ***first, char **second, int length) {
  char **first_array = (*first);
  for (int i = 0; i < length; i++) {
    combine_string(((first_array + i)), (*(second + i)));
  }
} /* combine_string_arrays() */

/*
 * Adds spaces to a an array of strings to format them nicley
 */
void space_padding(char ***string, int length, int mod) {
  char **array = (*string);
  for (int i = 0; i < length; i++) {
    int allignment = (strlen((*(array + i)))%mod);
    if (allignment == 0) {
      allignment = 5;
    }
    int spaces_needed = ((mod + 1) - allignment);
    for (int j = 0; j < spaces_needed; j++) {
      combine_string((array + i), " ");
    }
  }
} /* space_padding() */

/*
 * creates a node representing a line of text in a menu and returns it
 * content and links are assumed to be malloc'd though might change.
 */
menu_n *create_menu_node(char *content, char **links, int num_links, int selectable, int type, int spaces_after) {
  menu_n *new_node = malloc(sizeof(menu_n));
  new_node->content = content;
  new_node->next = NULL;
  new_node->prev = NULL;
  new_node->type = type;
  new_node->spaces_after = spaces_after;
  new_node->draw_function = &draw_stats;
  new_node->selectable = selectable;
  new_node->link = malloc(sizeof(link_n));
  new_node->link->link_c = (*links);
  menu_n *first_link = new_node;
  for (int i = 1; i < num_links; i++) {
    new_node->link->next = malloc(sizeof(link_n));
    new_node->link->next->link_c = (*(links + i));
    new_node->link->next->prev = new_node->link;
    new_node->link = new_node->link->next;
  }
  first_link->link->prev = new_node->link;
  new_node->link->next = first_link->link;
  switch (type) {
    case 1:
      new_node->select_function = &change_menu;
      break;
    case 2:
      new_node->select_function = NULL;
      break;
    case 3:
      new_node->select_function = NULL;
      break;
    case 4:
      new_node->select_function = NULL;
      break;
    case 5:
      new_node->select_function = NULL;
      break;
    case 6:
      new_node->select_function = NULL;
      break;
  }
  return new_node;
} /* create_menu_node() */

void append_menu_node(menu_n *original, menu_n *new) {
  while (original->next != NULL) {
    original = original->next;
  }
  original->next = new;
  new->prev = original;
}

void append_menu_text(menu_n *original, char *new) {
  char *link = strdup("exit");
  menu_n *new_node = create_menu_node(new, &link, 1, 0, 1, 1);
  append_menu_node(original, new_node);
}

/*
 * Returns total occurences of a character within provided string
 */
int occurences_of_character(char c, char *string) {
  int total_occurences = 0;
  while ((*string) != '\0') {
    if ((*string) == c) {
      total_occurences++;
    }
    string++;
  }
  return total_occurences;
} /* occurences_of_character() */

/*
 * returns actual integer values, so "10" -> 10
 */
int *char_to_int_array(char **old_array, int size) {
  int *new_results = malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    char *current_string = (*(old_array + i));
    (*(new_results + i)) = atoi(current_string);
  }
  return new_results;
} /* char_to_int_array() */
