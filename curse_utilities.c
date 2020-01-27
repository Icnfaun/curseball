#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <ncurses.h>
#include <ctype.h>

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
 * second is freed, for convienence in regards to how function is used
 * in curse_player_control.c
 *
 */
void combine_string_arrays(char ***first, char **second, int length) {
  char **first_array = (*first);
  for (int i = 0; i < length; i++) {
    combine_string(((first_array + i)), (*(second + i)));
    free(*(second + i));
    (*(second + i)) == NULL;
  }
  free(second);
} /* combine_string_arrays() */

/*
 * Adds spaces to a an array of strings to format them nicley
 * basically like a tab, but varied in size, used for comvenience
 * when making statistic pages for players.
 */
void space_padding(char ***string, int length, int mod) {
  char **array = (*string);
  for (int i = 0; i < length; i++) {
    int spaces_used = (strlen((*(array + i)))%mod);
    if (spaces_used == 0) {
      spaces_used = mod;
    }
    int spaces_needed = ((mod + 1) - spaces_used);
    for (int j = 0; j < spaces_needed; j++) {
      combine_string((array + i), " ");
    }
  }
} /* space_padding() */

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
 * used for getting lots of stats on players and easily converting them all
 * from text.
 */
int *char_to_int_array(char **old_array, int size) {
  int *new_results = malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    char *current_string = (*(old_array + i));
    (*(new_results + i)) = atoi(current_string);
  }
  return new_results;
} /* char_to_int_array() */

/*
 * CHANGES any given string in to a lowercase version.
 */
void string_to_lower(char *string) {
  int string_length = strlen(string);
  for (int i = 0; i < string_length; i++) {
    char current_letter = (*(string + i));
    (*(string + i)) = tolower(current_letter);
  }
} /* string_to_lower */

void free_string_array(char **strings, int num_strings) {
  for(int i = 0; i < num_strings; i++){
    free(*(strings + i));
  }
  free(strings);
}
