# Makefile
LDFLAGS=-lncurses

all: curseball.c curseball.h curse_menu.c curse_menu.h curse_sound.c curse_sound.h curse_player_control.c curse_player_control.h curse_utilities.c curse_utilities.h 
	gcc curseball.c curse_menu.c curse_sound.c curse_player_control.c curse_utilities.c -g -o cball -lncurses -lSDL -lSDL_mixer
