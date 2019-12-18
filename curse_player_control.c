#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "curse_menu.h"
#include "curse_player_control.h"
#include "curse_utilities.h"

#define MAX_LINE_LEN (1000)

/*
 * Code for the import player page, mainly used to get the correct id to later create player files with
 * edits the current menu to display results, won't work for any other page really unless you format it exactly
 * TODO change the format of the function, so you dont need to work with adjacent nodes so much
 * TODO make search case insensitive
 */
void search_player(menu_n *current_node) {
  menu_n *traversal_node = current_node;
  while(traversal_node->next != NULL) {
    traversal_node = traversal_node->next;    
  }
  menu_n *last_node = traversal_node;
  int previous_results_cleared = 0;
  while (traversal_node->prev != NULL) {
    if ((!previous_results_cleared) && (traversal_node->next != last_node) && (traversal_node->next != NULL)) {
      free_menu_node(traversal_node->next);
    }    
    if (strncmp(traversal_node->content, "------", 6) == 0) {
      previous_results_cleared = 1;
    }
    traversal_node = traversal_node->prev;

  }
  char *first_name = traversal_node->next->link->link_c;
  char *last_name = traversal_node->next->next->link->link_c;
  menu_n *search_results = NULL;
  int number_of_results = 0;
  FILE *database = fopen("teams/players/data/People.csv", "r");
  if (database == NULL) {
    return;
  }
  char current_line[MAX_LINE_LEN] = "";
  Match:
  while (fgets(current_line, MAX_LINE_LEN, database) != NULL) {
    char *current_line_d = strdup(current_line);
    char cur_first_name[100] = "";
    char cur_last_name[100] = "";
    char first_played[100] = "";
    char last_played[100] = "";
    char unique_id[100] = "";
    char *current_line_t = strsep(&current_line_d, ",");
    int current_category = 1;
    while (current_line_t != NULL) {
      switch (current_category){
        case 1:
          strcpy(unique_id, current_line_t);
          break;
        case 14:
          strcpy(cur_first_name, current_line_t);
          if(strncmp(cur_first_name, first_name, strlen(first_name)) != 0) {
            goto Match;
          }
          break;
        case 15:
          strcpy(cur_last_name, current_line_t);
          if(strncmp(cur_last_name, last_name, strlen(last_name)) != 0) {
            goto Match;
          }
          break;
        case 21:
          strcpy(first_played, current_line_t);
          break;
        case 22:
          strcpy(last_played, current_line_t);
          break;
      }
      current_line_t = strsep(&current_line_d, ",");
      current_category++;
    }
    free(current_line_d);
    char *result_content = malloc(sizeof(char) * (strlen(first_name) + strlen(last_name) + strlen(first_played) + strlen(last_played) + 8));
    char *result_id = malloc(sizeof(char) * (strlen(unique_id) + 1));
    strcpy(result_id, unique_id);
    strcpy(result_content, first_name);
    strcat(result_content, " ");
    strcat(result_content, last_name);
    strcat(result_content, " (");
    strcat(result_content, first_played);
    strcat(result_content, " - ");
    strcat(result_content, last_played);
    strcat(result_content, ")");
    if (number_of_results == 0) {
      search_results = malloc(sizeof(menu_n));
      search_results->link = malloc(sizeof(link_n));
      search_results->prev = NULL;
    }
    else {
      search_results->next = malloc(sizeof(menu_n));
      search_results->next->link = malloc(sizeof(link_n));
      search_results->next->prev = search_results;
      search_results = search_results->next;
    }
    search_results->next = NULL;
    search_results->type = 5;
    search_results->selectable = 1;
    search_results->content = result_content;
    search_results->link->link_c = strdup(result_id);
    search_results->link->next = search_results->link;
    search_results->link->prev = search_results->link;
    search_results->draw_function = &draw_plain_text;
    search_results->select_function = &player_info_menu;
    number_of_results++;
  }
  fclose(database);
  database = NULL;
  if (search_results == NULL) {
    return;
  }
  menu_n *last_result = search_results; 
  while (search_results->prev != NULL) {
    search_results = search_results->prev;
  }
  traversal_node = traversal_node->next->next->next->next;
  last_node->prev = last_result;
  last_result->next = last_node;
  traversal_node->next = search_results;
  search_results->prev = traversal_node;
} /*  search_player() */

/*
 * Gets lines with a matching player id form a specified file residing in
 * players/teams/data, returns in string form
 */
char *get_player_id_lines(char *filename, char *player_id) {
  char *full_filename = malloc(sizeof(char) * (strlen(filename) + 20));
  strcpy(full_filename, "teams/players/data/");
  strcat(full_filename, filename);
  FILE *data = fopen(full_filename, "r");
  char current_line[MAX_LINE_LEN] = "";
  char current_id[20] = "";
  char *results = NULL;
  while(fgets(current_line, MAX_LINE_LEN, data) != NULL) {
    sscanf(current_line, "%[^,]", current_id);
    if (strncmp(current_id, player_id, strlen(player_id)) == 0) {
      if(results == NULL) {
        results = strdup(current_line);
      }
      else {
        combine_string(&results, current_line);
      }
    }
  }
  fclose(data);
  data = NULL;
  return results;
} /* get_player_id_lines() */

/*
 * changes the active menu to a sceen displaying stats for a given player
 */
void player_info_menu(menu_n *player) {
  int total_lines = 0;
  char *link = strdup("0");
  char **player_menu_lines_final = get_spreadsheet_info(&total_lines, "Batting.csv", player->link->link_c, 1);
  int position = evaluate_main_position(player->link->link_c);
  char *header = strdup("Yr    G    AB   R    H    2B   HR   RBI  SB   BB   SO");
  int wanted_columns[] = {5,6,7,8,9,11,12,13,15,16};
  for (int i = 0; i < 10; i++) {
    space_padding(&player_menu_lines_final, total_lines, 5);
    combine_string_arrays(&player_menu_lines_final, get_spreadsheet_info(NULL, "Batting.csv", player->link->link_c, wanted_columns[i]), total_lines);
    current_menu_g = menu_from_strings(player_menu_lines_final, &link, total_lines, 1, 6);
  }
} /* player_info_menu() */

/*
 * Evaluates the most frequently played position of a given player
 */
int evaluate_main_position(char *player_id) {
  int number_of_results = -1;
  char **positions = get_spreadsheet_info(&number_of_results, "Fielding.csv", player_id, 5);
  char **frequency = get_spreadsheet_info(NULL, "Fielding.csv", player_id, 6);
  int *frequency_i = char_to_int_array(frequency, number_of_results);
  int positional_results[7] = {0};
  for (int i = 0; i < number_of_results; i++) {
    switch(*(*(positions + i))) {
      case 'P':
        positional_results[0] += (*(frequency_i + i));
        break;
      case 'C':
        positional_results[1] += (*(frequency_i + i));
        break;
      case '1':
        positional_results[2] += (*(frequency_i + i));
        break;
      case '2':
        positional_results[3] += (*(frequency_i + i));
        break;
      case '3':
        positional_results[4] += (*(frequency_i + i));
        break;
      case 'S':
        positional_results[5] += (*(frequency_i + i));
        break;
      case 'O':
        positional_results[6] += (*(frequency_i + i));
        break;
    } 
  }
  int most_played_position = -1;
  int greatest = -1;
  for (int i = 0; i < 7; i++) {
    if (positional_results[i] > greatest) {
      greatest = positional_results[i];
      most_played_position = i+1;
    }
  }
  return most_played_position;
} /* evaluate_position */

/*
 * returns every string/number from a given column in a csv file
 * used to find matches for a player, and return the results
 */
char **get_spreadsheet_info(int *size, char *filename, char *player_id, int column) {
  char *player_data = get_player_id_lines(filename, player_id);
  int number_of_lines = occurences_of_character('\n', player_data);
  char **results = malloc(sizeof(char *) * number_of_lines);
  int current_result = 0;
  char current_line[MAX_LINE_LEN] = "";
  int bytes_read = 0;
  while (sscanf(player_data, "%[^\n]\n%n", current_line, &bytes_read) == 1) {
    player_data += bytes_read;
    char *current_line_m = strdup(current_line);
    char *current_data = strsep(&current_line_m, ",");
    int current_column = 0;
    while (current_data != NULL) {
      if (current_column == column) {
        (*(results + current_result)) = strdup(current_data);
        current_result++;
        current_data = NULL;
      }
      else {
        current_data = strsep(&current_line_m, ",");
      }
      current_column++;
    } 
  }
  if (size != NULL) {
    (*size) = number_of_lines;
  }
  return results;
} /* get_spreadsheet_info() */
