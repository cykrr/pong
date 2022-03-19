#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

float dt = 0, last_frame = 0, current_frame = 0;

SDL_Rect leftBar = {0, 480 - 50 , 10, 100};
SDL_Rect rightBar = {640 - 10, 480 - 50 , 10, 100};
SDL_Rect ball = {(640 - 10)/2, (480 - 10)/2, 10, 10};

float ballSpeed = 0.1;
float ballDirection = 0;


void updateTime() {
	current_frame = SDL_GetTicks();
	dt = current_frame - last_frame;
	last_frame = current_frame;
}

int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (ren == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	int close = 0;
	while (!close){
		SDL_SetRenderDrawColor(ren, 0x20, 0x20, 0x20, 0xff);
		SDL_RenderClear(ren);

		updateTime();

		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case SDL_QUIT:
					close = 1;
					break;
			}
		}
		SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);

		SDL_RenderFillRect(ren, &leftBar);
		SDL_RenderFillRect(ren, &rightBar);
		SDL_RenderFillRect(ren, &ball);

		SDL_RenderPresent(ren);
	}
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
