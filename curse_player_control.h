extern menu_n *current_menu_g;
extern bool menu_changed_g;

struct position_player_struct {
  int main_position; //position that will appear when assembling teams, a number 1-9 
  int power;      //based on slugging percentage, a 100 is around a .650 
  int contact;    //based on average, a 100 is around a .350 batting average
  int patience;   //based on OBP, a 100 is around a .450
  int speed;      //based on stolen bases only, were 100 is around 60 SB
  int defensive_flexibility; //based on ammount of appearances at different positions, a 100 is like 6 or 7 positions
  int experience; //based on years played, a 100 is greater than 15 years
}position;

struct pitcher_struct {
  int power;  //based on strikeouts, over 300 per is a 100
  int control; //based on walks per 9 innings less than 2 is a 100
  int durability; //based on innings pitched, more than 250 is a 100
  struct pitch_info_struct *pitches;
}pitcher;

struct pitch_info_struct {
  int speed;
  int horizontal_movement;
  int downwards_movement;
}pitch;

struct player_struct {
  int bats;
  int throws;
  int defense;  //based on errors commited, around 25 errors is a 50
  struct position *position_player_info;
  struct pitcher *pitcher_info;
}player;

struct team_struct {
  char *name;
  struct player *players;
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
