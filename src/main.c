#include <stdlib.h>
#include <stdint.h>

#include <raylib.h>
#define RAYMATH_IMPLEMENTATION
#include <raymath.h>

#include "game.h"

int main(int argc, char *argv[]){
    InitWindow(800, 600, "Rigid bodies");
	
	SetTargetFPS(60);
		
	Game *game = malloc(sizeof(Game));
	InitGame(game);
	
	while (!WindowShouldClose()){
		UpdateGame(game, GetFrameTime());
		
        BeginDrawing();
		DrawGame(game, GetFrameTime());
        EndDrawing();
    }

    CloseWindow();

    return 0;
}