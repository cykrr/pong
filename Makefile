all: build

build: main.c
	gcc main.c -o pong -lSDL2 -lm


