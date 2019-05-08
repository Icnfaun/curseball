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

void draw_menu(WINDOW *, struct menu_node *, int selected);
void free_menu(struct menu_node *);
void display_menu(WINDOW *, char **);
menu_n *create_menu(char *);
char *get_link(struct menu_node *, int);
menu_n *get_selected(struct menu_node *, int);
int menu_select_init(struct menu_node *);
int menu_select(struct menu_node *, int, int);

