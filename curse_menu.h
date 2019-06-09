extern int window_size;
extern int sound;
extern int difficulty;
extern int wscreen_width;
extern int wscreen_height;
extern int num_directories;
extern char **team_directories;
extern WINDOW *game_w;

typedef struct menu_node {
  struct menu_node *next;
  struct menu_node *prev;
  int setting;
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
int draw_menu(WINDOW *, struct menu_node *, int selected, int s_multilines);
void free_menu(struct menu_node *);
void display_menu(WINDOW *, char **);
void init_config(char *);
menu_n *create_menu(char *);
menu_n *get_selected(struct menu_node *, int);
int menu_select_init(struct menu_node *);
int menu_select(struct menu_node *, int, int);
