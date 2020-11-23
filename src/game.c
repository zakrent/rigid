#include <SDL2/SDL.h>

#include "common.h"
#include "vector.h"
#include "game.h"

static Body CreateCircle(v2 pos, v2 vel, real radius, real mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = Vec3(0.0f, 1.0f, 0.0f),
		.vertCount = 16,
		.inv_mass = 1.0f/mass
	};
	
	for(int i = 0; i < 16; i++){
		body.shape[i] = Vec2(radius*cosf(i/16.0f*2.0f*M_PI), radius*sinf(i/16.0f*2.0f*M_PI));
	}
	
	return body;
}

static Body CreateRectangle(v2 pos, real w, real h, v2 vel, real mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = Vec3(0.0f, 1.0f, 0.0f),
		.vertCount = 4,
		.inv_mass = 1.0f/mass
	};
	
	body.shape[0] = Vec2(w/2.0f, h/2.0f);
	body.shape[1] = Vec2(w/2.0f, -h/2.0f);
	body.shape[2] = Vec2(-w/2.0f, -h/2.0f);
	body.shape[3] = Vec2(-w/2.0f, h/2.0f);
	
	return body;
}


static void AddBody(Body body, Game *game){
	game->bodies[game->bodyCount++] = body;
}

void InitGame(Game *game){
	*game = (Game){0};
	AddBody(CreateRectangle(Vec2(500.0f,400.0f), 1000.0f, 200.0f, Vec2(0.0f, 0.0f), INFINITY), game);
	AddBody(CreateCircle(Vec2(300.0f,150.0f), Vec2(0.0f, 0.0f), 20.0f, 1.0f), game);
	AddBody(CreateCircle(Vec2(300.0f,100.0f), Vec2(0.0f, 0.0f), 20.0f, 1.0f), game);
	AddBody(CreateCircle(Vec2(300.0f,50.0f), Vec2(0.0f, 0.0f), 20.0f, 1.0f), game);
	for(int i = 0; i < 10; i++){
		AddBody(CreateCircle(Vec2(300.0f+40*i+i,100.0f), Vec2(0.0f, 0.0f), 20.0f, 1.0f), game);
	}

	return;
}

static void TranslateBodies(Body *bodies, u32 count){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		for(int j = 0; j < body->vertCount; j++){
			v2 vert = V2Add(body->shape[j], body->position);
			body->vertices[j] = vert;
		}
	}
}

static void UpdateAABB(Body *bodies, u32 count){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		v2 max = body->vertices[0];
		v2 min = body->vertices[0];
		for(int j = 0; j < body->vertCount; j++){
			v2 vert = body->vertices[j];
			
			//TODO: problem with negative coords?
			if(vert.x > max.x){
				max.x = vert.x;
			}
			if(vert.x < min.x){
				min.x = vert.x;
			}
			
			if(vert.y > max.y){
				max.y = vert.y;
			}
			if(vert.y < min.y){
				min.y = vert.y;
			}
		}
		body->AABB = Rect(min.x, min.y, max.x-min.x, max.y-min.y);
	}
}

//TODO: remove?
static bool AABBCollides(rect a, rect b){
	return (a.x < b.x+b.w && a.x +a.w > b.x &&
			a.y < b.y+b.h && a.y +a.h > b.y);
}

//TODO: remove this
#define MAX_COLLISIONS BODY_MAX
static u32 CheckAABBCollisions(Body *bodies, u32 count, Collision *collisions){
	u32 numCollisions = 0;
	
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		body->collides = false;
	}
	
	for(int i = 0; i < count-1; i++){
		Body *body1 = bodies+i;
		for(int j = i+1; j < count; j++){
			Body *body2 = bodies+j;
			
			rect a = body1->AABB;
			rect b = body2->AABB;
			
			bool xCollides = a.x <= b.x+b.w && a.x +a.w >= b.x;
			bool yCollides = a.y <= b.y+b.h && a.y +a.h >= b.y;
			bool collision = xCollides && yCollides;
			
			body1->collides = collision;
			body2->collides = collision;
			
			if(collision){
				v2 diff = V2Sub(body1->position, body2->position);
				
				v2 normal = Vec2(0.0f, 0.0f);
				
				if(ABS(diff.x) > ABS(diff.y)){
					normal.x = 1.0f;
					if(diff.x < 0.0f){
						normal.x *= -1;
					}
				}
				else{
					normal.y = 1.0f;
					if(diff.y < 0.0f){
						normal.y *= -1;
					}
				}	
				
				collisions[numCollisions++] = (Collision){
					.body1 = body1,
					.body2 = body2,
					//TODO: hack
					.normal = Vec2(0.0f, -1.0f),
					//TODO: hack
					.depth = ABS(b.y + b.h - a.y)
				};
				
				printf("DEPTH: %f, %f, %f %f\n", ABS(b.y - 260), a.h, b.h, b.h - a.y + b.y);
			}
		}
	}
	
	return numCollisions;
}

static void DrawBodies(SDL_Renderer *renderer, Body *bodies, u32 count){
	
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		SDL_Point *points = AllocArray(SDL_Point, body->vertCount+1);
		
		for(int j = 0; j < body->vertCount; j++){
			points[j].x = (i32)body->vertices[j].x;
			points[j].y = (i32)body->vertices[j].y;
		}
		
		points[body->vertCount].x = (i32)body->vertices[0].x;
		points[body->vertCount].y = (i32)body->vertices[0].y;
		
		SDL_SetRenderDrawColor(renderer, body->color.r*255, body->color.g*255, body->color.b*255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLines(renderer, points, body->vertCount+1);
		
		if(body->collides){
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
		else{
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 100);
		}
		
		const SDL_Rect aabbRect = {
			.x = body->AABB.x,
			.y = body->AABB.y,
			.w = body->AABB.w,
			.h = body->AABB.h,
		};
		//SDL_RenderDrawRect(renderer, &aabbRect);
		
		Free(points);
	}

}

static void UpdateBodies(Body *bodies, u32 count, real dt){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		
		if(body->inv_mass != 0){
			body->velocity = V2Add(body->velocity, V2Mul(dt, Vec2(0.0f, 0.001f)));
		}
		
		body->position = V2Add(body->position, V2Mul(dt, body->velocity));
	}
}

//TODO: mass
static void ResolveCollisions(Collision *collisions, u32 count, real dt){
	for(int i = 0; i < count; i++){
		Collision *collision = collisions+i;
		
		Body *body1 = collision->body1;
		Body *body2 = collision->body2;

		real vRelNorm = V2Dot(V2Sub(body2->velocity, body1->velocity), collision->normal);
		
		//TODO: changed to 1.5
		v2 j = V2Mul(-1.5f*vRelNorm/(body1->inv_mass+body2->inv_mass), collision->normal);
				
		body1->velocity = V2Sub(body1->velocity, V2Mul(body1->inv_mass, j));
		body2->velocity = V2Add(body2->velocity, V2Mul(body2->inv_mass, j));
		
		v2 d = V2Mul(collision->depth/(body1->inv_mass+body2->inv_mass),collision->normal);
		body1->position = V2Sub(body1->position, V2Mul(body1->inv_mass, d));
		body2->position = V2Add(body2->position, V2Mul(body2->inv_mass, d));
		
		//ASSERT(body2->position.y <= 280.0f);
		
		body2->collides = 0;
	}
}

void Frame(Game *game, real dt, SDL_Renderer *renderer){
	//TODO: move to init?
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	
	UpdateBodies(game->bodies, game->bodyCount, dt);
	
	TranslateBodies(game->bodies, game->bodyCount);
	UpdateAABB(game->bodies, game->bodyCount);
			
	Collision *collisions = AllocArray(Collision, MAX_COLLISIONS);
	u32 numCollisions = CheckAABBCollisions(game->bodies, game->bodyCount, collisions);	
	
	ResolveCollisions(collisions, numCollisions, dt);
	
	TranslateBodies(game->bodies, game->bodyCount);
	
	Free(collisions);
	
	DrawBodies(renderer, game->bodies, game->bodyCount);
		
	SDL_RenderPresent(renderer);
}