extern menu_n *current_menu_g;
extern bool menu_changed_g;

struct player_struct {
  char first_name[30];
  char last_name[30];
  int bats;
  int throws;
  int defense;  //based on average errors commited, around 25 errors is a 50
  int main_position; //position that will appear when assembling teams, a number 1-9 
  int power;      //based on slugging percentage, a 100 is around a .650 
  int contact;    //based on average, a 100 is around a .350 batting average
  int patience;   //based on OBP, a 100 is around a .450
  int speed;      //based on stolen bases only, were 100 is around 60 SB
  int defensive_flexibility; //based on ammount of appearances at different positions, a 100 is like 6 or 7 positions
  int experience; //based on years played, a 100 is greater than 15 years
  int durablility; //based on games played a year on average, 100 is 162
  int pitching_power;
  int pitching_control;
}player;

struct team_struct {
  char *name;
  struct player *players[26];
}team;

void search_player(menu_n *);
char *get_player_id_lines(char *, char *);
void player_info_menu(menu_n *);
int evaluate_main_position(char *);
char **get_spreadsheet_info(int *, char *, char *, int);
menu_n *create_statline(char **, int, char *, int);
char *position_to_text(int);
void import_player(char *);
void import_pitcher(char *);
