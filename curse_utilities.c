#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <ncurses.h>

#include "curse_menu.h"

/* 
 * First string is assumed to be malloced second is not
 */
void combine_string(char **first, char *second) {
  char *combined = malloc(sizeof(char) * (strlen((*first)) + strlen(second) + 1));
  strcpy(combined, (*first));
  strcat(combined, second);
  free((*first));;
  (*first) = combined;
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
    int spaces_needed = (mod - (strlen((*(array + i)))%mod)) + 1;
    for (int j = 0; j < spaces_needed; j++) {
      combine_string((array + i), " ");
    }
  }
} /* space_padding() */

/*
 * creates a node representing a line of text in a menu and returns it
 */
menu_n *create_menu_node(char *content, char **links, int num_links, int selectable, int type) {
  menu_n *new_node = malloc(sizeof(menu_n));
  new_node->content = content;
  new_node->next = NULL;
  new_node->prev = NULL;
  new_node->type = type;
  new_node->draw_function = &draw_plain_text;
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
  return new_node;
} /* create_menu_node() */

/*
 * NOT DONE
 * creates a menu without a file, just with strings
 */
menu_n *menu_from_strings(char **strings, char **links, int strings_length, int  selectable, int type) {
  menu_n *first_node = create_menu_node((*strings), links, 1, selectable, type);
  first_node->prev = NULL;
  menu_n *traversal_node = first_node;
  for (int i = 1; i < strings_length; i++) {
    traversal_node->next = create_menu_node((*(strings + i)), links, 1, selectable, type);
    traversal_node->next->prev = traversal_node;
    traversal_node = traversal_node->next;
  }
  return first_node;
} /* menu_from_strings() */

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
int * char_to_int_array(char **old_array, int size) {
  int *new_results = malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    char *current_string = (*(old_array + i));
    (*(new_results + i)) = atoi(current_string);
  }
  return new_results;
} /* char_to_int_array() */
