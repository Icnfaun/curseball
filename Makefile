# Makefile
LDFLAGS=-lncurses

all: curseball.c curseball.h 
	gcc curseball.c -g -o cball -lncurses
