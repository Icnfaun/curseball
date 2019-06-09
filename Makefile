# Makefile
LDFLAGS=-lncurses

all: curseball.c curseball.h curse_menu.c curse_menu.h curse_sound.c curse_sound.h 
	gcc curseball.c curse_menu.c curse_sound.c -g -o cball -lncurses -lSDL -lSDL_mixer
