#include <SDL2/SDL.h>
#include <math.h>

#include "common.h"
#include "vector.h"
#include "game.h"

#define CIRCLE_VERT_COUNT 16
static Body CreateCircle(v2 pos, v2 vel, real radius, real mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = Vec3(0.0f, 1.0f, 0.0f),
		.vertCount = CIRCLE_VERT_COUNT,
		.inv_mass = 1.0f/mass
	};
	
	for(int i = 0; i < CIRCLE_VERT_COUNT; i++){
		body.shape[i] = Vec2(radius*cosf(i/(real)CIRCLE_VERT_COUNT*2.0f*M_PI), radius*sinf(i/(real)CIRCLE_VERT_COUNT*2.0f*M_PI));
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
	//Todo: check this
	//*game = (Game){0};
	AddBody(CreateCircle(Vec2(100.0f,-100.0f), Vec2(0.1f, 0.1f), 20.0f, 1.0f), game);
	
	//AddBody(CreateCircle(Vec2(300.0f,200.0f), Vec2(0.0f, -0.1f), 20.0f, 1.0f), game);

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

//TODO: remove this
#define MAX_COLLISIONS BODY_MAX
static u32 CheckCollisions(Body *bodies, u32 count, Collision *collisions){	
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		body->collides = false;
	}
	
	u32 numPotentialCollisions = 0;
	Collision *potentialCollisions = AllocArray(Collision, MAX_COLLISIONS);
	for(int i = 0; i < count-1; i++){
		Body *body1 = bodies+i;
		for(int j = i+1; j < count; j++){
			Body *body2 = bodies+j;
			
			rect a = body1->AABB;
			rect b = body2->AABB;
			
			bool xCollides = a.x <= b.x+b.w && a.x +a.w >= b.x;
			bool yCollides = a.y <= b.y+b.h && a.y +a.h >= b.y;
			bool collision = xCollides && yCollides;
			
			/*body1->collides |= collision;
			body2->collides |= collision;*/
			
			if(collision){
				v2 diff = V2Sub(body1->position, body2->position);
				
				potentialCollisions[numPotentialCollisions++] = (Collision){
					.body1 = body1,
					.body2 = body2,					
				};
				
			}
		}
	}
	
	u32 numCollisions = 0;
	
	for(int i = 0; i < numPotentialCollisions; i++){
		Collision *potCollision = potentialCollisions+i;
		Body *body1 = potCollision->body1;
		Body *body2 = potCollision->body2;
		
		real penDist = INFINITY;
		v2 penNorm = Vec2(1.0, 0.0);
		
		bool collides = true;
		for(int b = 0; b < 2; b++){
			for(int i = 0; i < body1->vertCount+1; i++){
				v2 P1 = body1->vertices[i];
				v2 P2 = body1->vertices[(i+1) % body1->vertCount];
				v2 deltaP = V2Norm(V2Sub(P1, P2));
				v2 n = Vec2(-deltaP.y, deltaP.x);
				
				real b1_min = INFINITY;
				real b1_max = -INFINITY;
				real b2_min = INFINITY;
				real b2_max = -INFINITY;
				for(int j = 0; j < body1->vertCount; j++){
					real proj = V2Dot(n, body1->vertices[j]);
					if(proj > b1_max){
						b1_max = proj;
					}
					if(proj < b1_min){
						b1_min = proj;
					}
				}
				
				for(int j = 0; j < body2->vertCount; j++){
					real proj = V2Dot(n, body2->vertices[j]);
					if(proj > b2_max){
						b2_max = proj;
					}
					if(proj < b2_min){
						b2_min = proj;
					}
				}
				
				collides = collides && b1_min <= b2_max && b1_max >= b2_min; 
				
				//TODO: SIGN decides if collides?
				if(collides){
					real curPenDist = MIN(b1_max, b2_max) - MAX(b2_min, b2_max);
					if(curPenDist < penDist){
						penDist = curPenDist;
						if(b == 0)
							penNorm = n;
						else
							penNorm = V2Mul(-1.0, n);
					}
				}
				
			}
			Body *temp = body2;
			body2 = body1;
			body1 = temp;
		}
		
		if(collides){
			collisions[numCollisions] = *potCollision;
			collisions[numCollisions].normal = penNorm;
			potCollision->body1->collides |= collides;
			potCollision->body2->collides |= collides;
			numCollisions++;
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
		SDL_RenderDrawRect(renderer, &aabbRect);
		
		Free(points);
	}

}

static void UpdateBodies(Body *bodies, u32 count, real dt){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		
		if(body->inv_mass != 0){
			body->velocity = V2Add(body->velocity, V2Mul(dt*0.0f, Vec2(0.0f, 0.0001f)));
		}
		
		body->position = V2Add(body->position, V2Mul(dt, body->velocity));
				
		body->impulse = Vec2(0.0f, 0.0f);
	}
}

static void ResolveCollisions(Collision *collisions, u32 count, Body *bodies, u32 bodyCount, real dt){
	for(int i = 0; i < count; i++){
		Collision *collision = collisions+i;
		
		Body *body1 = collision->body1;
		Body *body2 = collision->body2;

		real vRelNorm = V2Dot(V2Sub(body2->velocity, body1->velocity), collision->normal);
		
		if(vRelNorm > 0.0f){
			continue;
		}
		
		//changed to 1.9
		v2 j = V2Mul(-1.9f*vRelNorm/(body1->inv_mass+body2->inv_mass), collision->normal);
						
		body1->impulse = V2Sub(body1->impulse, j);
		body2->impulse = V2Add(body2->impulse, j);
	}
	
	
	for(int i = 0; i < bodyCount; i++){
		Body *body = bodies+i;
		body->velocity = V2Add(body->velocity, V2Mul(body->inv_mass, body->impulse));
	}
	
}

static void ResolveOverlap(Collision *collisions, u32 count, Body *bodies, u32 bodyCount, real dt){
	for(int i = 0; i < count; i++){
		Collision *collision = collisions+i;
		
		Body *body1 = collision->body1;
		Body *body2 = collision->body2;

		v2 d = V2Mul(collision->depth/(body1->inv_mass+body2->inv_mass),collision->normal);
		body1->position = V2Sub(body1->position, V2Mul(body1->inv_mass, d));
		body2->position = V2Add(body2->position, V2Mul(body2->inv_mass, d));
	}
}

void Frame(Game *game, real dt, SDL_Renderer *renderer){
	//TODO: move to init?
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	
	u64 start = SDL_GetPerformanceCounter();
	
	UpdateBodies(game->bodies, game->bodyCount, dt);
	
	u64 updateTick = SDL_GetPerformanceCounter();
	
	TranslateBodies(game->bodies, game->bodyCount);
	UpdateAABB(game->bodies, game->bodyCount);
	
	u64 translateTick = SDL_GetPerformanceCounter();
			
	Collision *collisions = AllocArray(Collision, MAX_COLLISIONS);
	u32 numCollisions = CheckCollisions(game->bodies, game->bodyCount, collisions);	
		
	ResolveCollisions(collisions, numCollisions, game->bodies, game->bodyCount, dt);	
	Free(collisions);
	
	u64 collisionTick = SDL_GetPerformanceCounter();
	
	DrawBodies(renderer, game->bodies, game->bodyCount);
	
	u64 drawTick = SDL_GetPerformanceCounter();
	
	u64 end = SDL_GetPerformanceCounter();
	float frameTime = (end - start)*1000000.0f/SDL_GetPerformanceFrequency();
	float updateTime = (updateTick - start)*1000000.0f/SDL_GetPerformanceFrequency();
	float translateTime = (translateTick - updateTick)*1000000.0f/SDL_GetPerformanceFrequency();
	float collisionTime = (collisionTick - translateTick)*1000000.0f/SDL_GetPerformanceFrequency();
	float drawTime = (drawTick - collisionTick)*1000000.0f/SDL_GetPerformanceFrequency();
	printf("Frame time: %9.5f us Upd: %9.5f us Trans: %9.5f us Col: %9.5f us Draw %9.5f us\n", frameTime, updateTime, translateTime, collisionTime, drawTime);
		
	SDL_RenderPresent(renderer);
}