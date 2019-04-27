# Makefile
LDFLAGS=-lncurses

all: curseball.c curseball.h 
	gcc curseball.c -o cball -lncurses
