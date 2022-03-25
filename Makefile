NAME=pong
all: $(NAME)

$(NAME): main.c
	gcc main.c -o $(NAME) -lSDL2 -lm -g


