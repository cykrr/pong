#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define DEGREES M_PI/180

int close = 0;
const Uint8 *kb = NULL;


float dt = 0, last_frame = 0, current_frame = 0;

SDL_Rect bottomBar = {(500 - 50)/2, 500 - 10 , 100, 10};
SDL_Rect ball = {(500 - 10)/2, (500 - 10)/2, 10, 10};
SDL_Rect screen = {0, 0, 500, 500};
SDL_Rect tmp;

float ballSpeed = 200;
float ballDirection = 25 * DEGREES;

void processInput(SDL_Window *window){
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
            switch (ev.type) {
                    case SDL_QUIT:
                            close = 1;
                            break;
            }
    }
    if (kb != NULL) {
        if (kb[SDL_SCANCODE_H]){
            bottomBar.x -= 300 * dt;
        } if (kb[SDL_SCANCODE_L]){
            bottomBar.x += 300 * dt;
        }
    }

    const Uint8 *kb = SDL_GetKeyboardState(0);
    if(kb[SDL_SCANCODE_J]){ 
        ballDirection -= M_PI/180;
    } else if(kb[SDL_SCANCODE_K]){ 
        ballDirection += M_PI/180;
    }
}

void updateBallPos() {
}

void updateTime() {
	current_frame = SDL_GetTicks();
	dt = (current_frame - last_frame) / 1000;
	last_frame = current_frame;
}

int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100, 500, 500,
                SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL );
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

	SDL_SetRenderDrawColor(ren, 0x20, 0x20, 0x20, 0xff);

        kb = SDL_GetKeyboardState(NULL);
	while (!close){
		SDL_SetRenderDrawColor(ren, 0x20, 0x20, 0x20, 0xff);
		SDL_RenderClear(ren);

		updateTime();
                processInput(win);

                printf("Pong\n");
                printf("dt: %.3f\n", dt);
                printf("ball: %d %d %d %d\n", ball.x, ball.y, ball.w, ball.h);
                printf("ballSpeed: %f\n", ballSpeed);
                printf("ballDirection: %f\n", ballDirection * 180 / M_PI);
                printf("Screen: %d %d %d %d\n", screen.x, screen.y, screen.w, screen.h);

                for(int i = 0; i < 23; i++) putchar('\n');

		SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
                ball.x += ballSpeed * dt * cos(ballDirection);
                ball.y -= ballSpeed * dt * sin(ballDirection);

                if(ball.x + ball.w >= screen.w || (ball.x <= 10)){
                    printf("Here\n");
                    ballDirection = M_PI - ballDirection;
                }
                if ( ball.y <= 0 || 
                        ( ball.y + ball.h >= 500 - 10 &&
                         SDL_IntersectRect(&ball, &bottomBar, &tmp)
                        )
                    ) {
                    ballDirection *= -1;
                }

                SDL_RenderFillRect(ren, &ball);

                SDL_RenderFillRect(ren, &bottomBar);

		SDL_RenderPresent(ren);
	}
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
