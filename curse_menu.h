extern int wscreen_width;
extern int wscreen_height;
extern int num_directories;
extern char **team_directories;
extern WINDOW *game_w;
extern int settings[3];

#define WINDOW_SIZE (0)
#define SOUND (1)
#define DIFFICULTY (2)


/*
 * types:
 * 1 plain text
 * 2 option 
 * 3 player
 */
typedef struct menu_node {
  struct menu_node *next;
  struct menu_node *prev;
  int (*draw_function)(struct menu_node *, int, WINDOW *);
  void (*select_function)(int, int);
  int type;
  int selectable;
  struct link_node *link;
  char *content;
}menu_n;

typedef struct link_node {
  struct link_node *next;
  struct link_node *prev;
  char *link_c;
}link_n;

void free_game();
void menu();
void init_libs();
void draw_menu(WINDOW *, struct menu_node *, int selected);
int draw_lines(char *, int, WINDOW *);
int draw_option(menu_n *, int, WINDOW *);
int draw_plain_text(menu_n *, int, WINDOW *);
void free_menu(struct menu_node *);
void display_menu(WINDOW *, char **);
void init_config(char *);
menu_n *create_menu(char *);
menu_n *get_selected(struct menu_node *, int);
int menu_select_init(struct menu_node *);
int menu_select(struct menu_node *, int, int);
void option_select(int, int);
int setting_finder(char *);
void refresh_window_size();

