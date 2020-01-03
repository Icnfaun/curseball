/*
 *  Curseball - Menu Maker
 *
 *  Author - Mason Snyder
 *  Version - 0.0
 *  OUTDATED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>

#include "menu_maker.h"

#define PATH_SIZE (9)

int main() {
  choose_mode();
}

void choose_mode() {
  printf("\n\n---Curseball Menu Maker---\n");
  printf("1.) Make New Menu\n2.) Edit Existing Menu\n3.) Quit\n");
  char c = get_valid_number("", 1, 3);
  switch(c) {
    case 1:
      make_menu();
      break;
    case 2:
      edit_menu();
      break;
    case 3:
      break;
  }
}

bool valid_input(char *input, bool ints, bool letters, bool spaces) {
  if (input == NULL) {
    return false;
  }
  if (input[0] == '\n') {
    return false;
  }
  for (int c = 0; c < strlen(input); c++) {
    if ((!ints) && (isdigit(input[c]))) {
      return false;
    }
    if ((!letters) && (isalpha(input[c]))) {
      return false;
    }
    if ((!spaces) && (input[c] == ' ')) {
      return false;
    }
  }
  return true;
}

int yes_or_no(char *prompt) {
  printf("%s (y/n)\n", prompt);
  char answer = ' ';
  while ((answer != 'y') && (answer != 'n')) {
    answer = getchar();
  }
  if (answer == 'y') {
    return 1;
  }
  return 0;
}

int get_valid_number(char *prompt, int min, int max) {
  printf("%s", prompt);
  printf("Enter a number between (inclusive) %i and %i\n", min, max);
  int valid_int = -1;
  while((valid_int < min) || (valid_int > max)) {
    scanf("%d", &valid_int);
  }
}

char *get_valid_input(char *prompt, bool ints, bool letters, bool spaces, int max_size) {
  printf("%s", prompt);
  printf("Restrictions: ");
  if (!ints) {
    printf("no ints, ");
  }
  if (!letters) {
    printf("no letters, ");
  }
  if (!spaces) {
    printf("no spaces, ");
  }
  printf("%i character max\n", max_size);
  char *input = malloc(sizeof(char) * (max_size + 1));
  input[0] = '\0';
  while ((input[0] == '\0') || (strlen(input) > max_size) || (!valid_input(input, ints, letters, spaces))) {
    fgets(input, max_size + 1, stdin);
    strtok(input, "\n");
  }
  char *confirmed_input = malloc(sizeof(char) * (strlen(input) + 1));
  strncpy(confirmed_input, input, strlen(input) + 1);
  free(input);
  return confirmed_input;
}

void make_menu() {
  char *title = get_valid_input("Enter menu name:\n", true, true, false, 20);
  char *full_path_title = malloc(sizeof(char) * (1 + PATH_SIZE + strlen(title)));
  strcpy(full_path_title, "../menus/");
  strcpy(full_path_title + PATH_SIZE, title);
  FILE *new_menu = fopen(full_path_title, "w");
  free(full_path_title);
  if (new_menu == NULL) {
    return;
  }
  int node = 1;
  bool add_node = true;
  while (add_node) {
    printf("\n\n--Node %i--\n", node);
    write_node(new_menu);
    add_node = yes_or_no("Node sucessfully written, add more?\n");
    if(add_node){
      fprintf(new_menu, "\n");
    }
    node++;
  }
  fclose(new_menu);
  new_menu = NULL;
  choose_mode();
}

void edit_menu() {
  char *menu_name = NULL;
  FILE *menu_file = NULL;
  bool menu_valid = false;
  while (!menu_valid) {
    menu_name = get_valid_input("Enter menu name, or \"list\" to view menus\n", true, true, false, 20);
    if (strcmp(menu_name, "list") == 0) {
      printf("\n");
      system("ls -1 ../menus"); 
      printf("\n");
    }
    else {
      char *full_menu_path = malloc(sizeof(char) * (1 + PATH_SIZE + strlen(menu_name)));
      strcpy(full_menu_path, "../menus/");
      strcpy(full_menu_path + PATH_SIZE, menu_name);
      menu_name = malloc((sizeof(char) + 1) * strlen(full_menu_path));
      strcpy(menu_name, full_menu_path);
      menu_file = fopen(full_menu_path, "r");
      if(menu_file != NULL) {
        menu_valid = true;
      }
    }
  }
  bool still_editing = true;
  while (still_editing) {
    if (menu_file == NULL) {
      menu_file = fopen(menu_name, "r");
    }
    int total_nodes = lines_in_file(menu_file);
    print_contents(menu_file);
    int node_to_edit = get_valid_number("Enter node to edit:\n", 1, total_nodes);
    rewrite_menu(menu_file, node_to_edit, menu_name);
    still_editing = yes_or_no("Edit more nodes?");
  }
  choose_mode();
}

void write_node(FILE *write) {
  int selectable = yes_or_no("Is the node selectable\n");
  int type = get_valid_number("Enter node type:\n", 1, 3);
  char *content = get_valid_input("Enter node text:\n", true, true, true, 1000);
  int number_links = 1;
  if (type == 2) {
    number_links = get_valid_number("How many options are there:\n", 2, 100);
  }
  char *default_link = get_valid_input("Enter the default link:\n", true, true, false, 1000);
  fprintf(write, "%i,%i,%s,%i,%s", type, selectable, content, number_links, default_link);
  if (type == 2) {
    for (int i = 0; i < number_links - 1; i++) {
      char *current_link = get_valid_input("Enter next option:\n", true, true, false, 1000);
      fprintf(write, " %s", current_link);
      free(current_link);
    }
  }
  free(content);
  free(default_link);
}

void rewrite_menu(FILE *read, int edit_line, char *menu_name) {
  fseek(read, 0, SEEK_SET);
  FILE *rewrite_copy = NULL;
  rewrite_copy = fopen("../menus/copy_menu", "w");
  char c = getc(read);
  int current_line = 1;
  bool new_line_written = false;
  while (c != EOF) {
    if (current_line != edit_line) {
      putc(c, rewrite_copy);
    }
    else {
      if(!new_line_written) {
        printf("Enter new content for line %i\n", current_line);
        write_node(rewrite_copy);
        new_line_written = true;
        if (c != EOF) {
          putc('\n', rewrite_copy);
        }
      }
    }
    if (c == '\n') {
      current_line++;
    }
    c = getc(read);
  }
  fclose(rewrite_copy);
  rewrite_copy = NULL;
  fclose(read);
  read = NULL;
  remove(menu_name);
  rename("../menus/copy_menu", menu_name);
}

void print_contents(FILE *read) {
  printf("\n\n");
  fseek(read, 0, SEEK_SET);
  char content[1000];
  int current_node = 1;
  while (fscanf(read, "%*i,%*i,%[^,],%*[^\n]\n", content) == 1) {
    printf("%i.) %s\n", current_node, content);
    current_node++;
  }
}

int lines_in_file(FILE *read) {
  fseek(read, 0, SEEK_SET);
  int lines = 0;
  while(!feof(read)) {
    char current_char = fgetc(read);
    if (current_char == '\n') {
      lines++;
    }
  }
  return lines;
}
