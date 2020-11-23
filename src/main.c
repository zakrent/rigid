#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdint.h>

#include "common.h"
#include "vector.h"
#include "game.h"

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Rigid",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1920/2, 1080/2,
		NULL);
		
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	
	Game *game = malloc(sizeof(Game));
	InitGame(game);
		
	bool running = true;
	real dt = 1.0f/60.0f*1000.0f;
	real slowdown = 1.0f;
	u32 target_trame_time = (u32)(dt*slowdown);
	u32 frame_begin = 0; 
	SDL_Event e;
	while(running){
		frame_begin = SDL_GetTicks();
		
		Frame(game, dt, renderer);
		
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:
				{
					running = false;
				}
				break;
				default:
				break;
			}
		}
	
		u32 frame_time = SDL_GetTicks() - frame_begin;
		if(frame_time < target_trame_time){
			SDL_Delay(target_trame_time-frame_time);
		}

	}
	
	return 0;
}