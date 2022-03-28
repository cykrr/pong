#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define DEGREES M_PI/180

SDL_Rect tmp;

float dt = 0, last_frame = 0, current_frame = 0;

int start = 0;

SDL_Rect bottomBar = {(500 - 100)/2, 500 - 10 , 100, 10};
SDL_Rect ball = {(500 - 10)/2, (500 - 20), 10, 10};
SDL_Rect screen = {0, 0, 500, 500};

float ballSpeed = 200;
float ballDirection = 45 * DEGREES;

typedef struct List {
    SDL_Rect *p;
    size_t length, free;
}List;

typedef struct Node {
    SDL_Rect rect;
    struct Node *next, *prev;
}Node;

typedef struct LinkedList {
    Node *head;
    Node *current;
}LinkedList;

float model(float percent) {
    return -2 * percent + 200; 
          
}

int calculateAngle()
{
    return (int)model((double)100*(bottomBar.x
        + bottomBar.w /2 ) /
    (screen.w-(bottomBar.w / 2.5)));
}

void bounceTop(){
    ballDirection *= -1;
}

void bounceSides(){
    ballDirection = M_PI - ballDirection;
}

void bounceBottom(){
    ballDirection = calculateAngle()*DEGREES;
}

void linkedListRemoveNode(LinkedList *list, Node *node){
    if (!(list && list->head && node)){
        return;
    }

    if (node == list->head){
        list->head = node->next;
        if(node->next)
            node->next->prev = NULL;
    } else {
        if(node->next){
            node->next->prev = node->prev;
        } if (node->prev) {
            node->prev->next = node->next;
        }
    }
    free(node);
}


void renderLinkedBricks(LinkedList *list, SDL_Renderer *renderer )
{
    for(Node *i = list->head; i != NULL; i = i->next){
        SDL_RenderDrawRect(renderer, &i->rect);
        if(SDL_IntersectRect(&ball, &i->rect, &tmp)){
            linkedListRemoveNode(list, i);
            bounceTop();
        }
    }
}

Node *nodeCreate(){
    Node *node = (Node*)malloc(sizeof(Node));
    if(!node){
        printf("Error creating Node, Out of mem \n");
        exit(1);
    }
    node->next = NULL, node->prev = NULL;

    return node;
}

void linkedListInit(LinkedList *list){
    list->current = NULL, list->head = NULL;
}

void linkedListAddElement(LinkedList* list, SDL_Rect *rect){
    Node *node = nodeCreate();
    node->rect = *rect;
    if(!list->head) list->head = node;
    else {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    if(!list->current) list->current = node;
}

void linkedListNext(LinkedList *list){
    if(!list->current) return;
    list->current = list->current->next;
}

void linkedListPrev(LinkedList *list){
    if(!list->current) return;
    list->current = list->current->prev;
}

void renderBricks(List *list, SDL_Renderer *renderer){
    for(size_t i = 0; i < list->length; i++){
        if (SDL_IntersectRect(&ball, &(list->p[i]), &tmp)) {
        }
        SDL_RenderDrawRect(renderer, &(list->p[i]));
    }
}


void listInit(List *list, size_t typeSize){
    list->p = malloc(5 * typeSize);
    if(!list->p){
        printf("Error creating list\n");
        exit(1);
    }

    list->length = 0;
    list->free = 5;
}

void listResize(List *list){
    void *tmp = realloc(list->p, sizeof(SDL_Rect)*(list->length + list->free + (int)list->length/2 ));
    if(!tmp){
        printf("Error resizing list\n");
        exit(1);
    }
    list->p = tmp;
    list->free += (int)list->length/2;
}

void listAddElement(List *list, void *element){
    if(list->free <= (int)list->length/2){
        listResize(list);
    }
    list->p[list->length] = *(SDL_Rect *)element;
    list->length++;
    list->free--;
}

void listAddBlock(List *list, SDL_Rect rect){
    listAddElement(list, &rect);
}

int close = 0;
const Uint8 *kb = NULL;

SDL_Rect *bricks = NULL;
size_t bricksLength, bricksFree;






void updatePosition() 
{
    ball.x += ballSpeed * dt * cos(ballDirection);
    ball.y -= ballSpeed * dt * sin(ballDirection);
}


LinkedList createLinkedBlocks(SDL_Renderer *renderer){
    LinkedList list;
    linkedListInit(&list);
    for(int i = 0; i*50 + 10 * i + 50 <= screen.w; i++)
        for(int j = 0; j*10 + 5 * j + 10 <= screen.h / 4; j++){
            SDL_Rect block = { 10 + 50 * i + 10*i, j*10 + j*5 + 10, 50, 10};
            linkedListAddElement(&list, &block);
        }
    return list;
}

List createBlocks(SDL_Renderer *renderer){
    List list;
    listInit(&list, sizeof(SDL_Rect));

    SDL_SetRenderDrawColor(renderer, 0x30, 0x30, 0x30, 0xff);
    for(int i = 0; i*50 + 10 * i + 50 <= screen.w; i++)
        for(int j = 0; j*10 + 5 * j + 10 <= screen.h / 4; j++){
            SDL_Rect block = { 10 + 50 * i + 10*i, j*10 + j*5 + 10, 50, 10};
            listAddBlock(&list, block);
            SDL_RenderDrawRect(renderer, &block);
         }
    return list;
}



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
        if ((kb[SDL_SCANCODE_H] || kb[SDL_SCANCODE_LEFT]) &&
                bottomBar.x >= 0){
            bottomBar.x -= 300 * dt;
        } if ((kb[SDL_SCANCODE_L] || kb[SDL_SCANCODE_RIGHT]) && 
            bottomBar.x + bottomBar.w <= 500){
            bottomBar.x += 300 * dt;
        } if (kb[SDL_SCANCODE_SPACE] || kb[SDL_SCANCODE_KP_ENTER]) {
            start = 1;
        }
    }

    if(kb[SDL_SCANCODE_J] || kb[SDL_SCANCODE_DOWN] ){ 
        ballDirection -= M_PI/90;
    } else if(kb[SDL_SCANCODE_K] || kb[SDL_SCANCODE_UP]){ 
        ballDirection += M_PI/90;
    }
}


void updateTime() {
	current_frame = SDL_GetTicks();
	dt = (current_frame - last_frame) / 1000;
	last_frame = current_frame;
}

int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, 
                        "SDL_Init Error: %s\n",
                        SDL_GetError());

		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Window* win = SDL_CreateWindow(
                "Pong!", 100, 100, 500, 500,
                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );

	if (win == NULL) {
		fprintf(stderr, 
                        "SDL_CreateWindow Error: %s\n",
                        SDL_GetError());

		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | 
                        SDL_RENDERER_PRESENTVSYNC);

	if (ren == NULL) {
		fprintf(stderr,
                        "SDL_CreateRenderer Error: %s\n",
                        SDL_GetError());

		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_SetRenderDrawColor(ren, 0x20, 0x20, 0x20, 0xff);

        kb = SDL_GetKeyboardState(NULL);
        LinkedList llist = createLinkedBlocks(ren);

        while (!start){
            updateTime();
            processInput(win);
            SDL_RenderPresent(ren);
        }
        
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

                printf("bar: %d %d %d %d\n", 
                        bottomBar.x, bottomBar.y, 
                        bottomBar.w, bottomBar.h);

                printf("%% away: %f%%\n", 
                        ((double)100*(bottomBar.x
                            + bottomBar.w /2 ) /
                        (screen.w-(bottomBar.w / 2))
                        ));
                printf("Angle: %d°\n", 
                        calculateAngle()
                        );
                putchar('\n');
//                printf("Array length: %zu\n", list.length);
//                printf("Array free: %zu\n", list.free);


                for(int i = 0; i < 8+8; i++) putchar('\n');

		SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);

                if(ball.x + ball.w >= screen.w || (ball.x <= 0)){
                    bounceSides();
                } 

                if ( ball.y <= 0){
                    bounceTop();
                }

                if ( ball.y + ball.h >= 500 - 10 &&
                         SDL_IntersectRect(&ball,
                             &bottomBar, &tmp)
                ){
                    bounceBottom();
                } 
                updatePosition();

                renderLinkedBricks(&llist, ren);
                SDL_RenderFillRect(ren, &ball);
                SDL_RenderFillRect(ren, &bottomBar);

		SDL_RenderPresent(ren);
	}
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
