#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"

#define MAX(x, y) ((x>y)?x:y)
#define MIN(x, y) ((x<y)?x:y)

/*
#define CIRCLE_VERT_COUNT 32
static Body CreateCircle(Vector2 pos, Vector2 vel, float radius, float mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = Vec3(0.0f, 1.0f, 0.0f),
		.vertCount = CIRCLE_VERT_COUNT,
		.inv_mass = 1.0f/mass
	};
	
	for(int i = 0; i < CIRCLE_VERT_COUNT; i++){
		body.shape[i] = Vector2(radius*cosf(i/(float)CIRCLE_VERT_COUNT*2.0f*M_PI), radius*sinf(i/(float)CIRCLE_VERT_COUNT*2.0f*M_PI));
	}
	
	return body;
}

static Body CreateRectangleangle(Vector2 pos, Vector2 vel, float w, float h, float mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = Vec3(0.0f, 1.0f, 0.0f),
		.vertCount = 4,
		.inv_mass = 1.0f/mass
	};
	
	body.shape[0] = Vector2(w/2.0f, h/2.0f);
	body.shape[1] = Vector2(w/2.0f, -h/2.0f);
	body.shape[2] = Vector2(-w/2.0f, -h/2.0f);
	body.shape[3] = Vector2(-w/2.0f, h/2.0f);
	
	return body;
}


static void AddBody(Body body, Game *game){
	game->bodies[game->bodyCount++] = body;
}


static void ResolveOverlap(Collision *collisions, int count, Body *bodies, int bodyCount, float dt){
	for(int i = 0; i < count; i++){
		Collision *collision = collisions+i;
		
		Body *body1 = collision->body1;
		Body *body2 = collision->body2;

		Vector2 d = Vector2Mul(collision->depth/(body1->inv_mass+body2->inv_mass),collision->normal);
		body1->position = Vector2Subtract(body1->position, Vector2Mul(body1->inv_mass, d));
		body2->position = Vector2Add(body2->position, Vector2Mul(body2->inv_mass, d));
	}
}*/

#define CIRCLE_VERT_COUNT 32
static Body CreateCircle(Vector2 pos, Vector2 vel, float radius, float mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = RED,
		.vertCount = CIRCLE_VERT_COUNT,
		.inv_mass = 1.0f/mass
	};
	
	for(int i = 0; i < CIRCLE_VERT_COUNT; i++){
		body.shape[i] = (Vector2){radius*cosf(i/(float)CIRCLE_VERT_COUNT*2.0f*PI), radius*sinf(i/(float)CIRCLE_VERT_COUNT*2.0f*PI)};
	}
	
	return body;
}

static Body CreateRectangleangle(Vector2 pos, Vector2 vel, float w, float h, float mass){
	Body body = (Body){
		.position = pos,
		.velocity = vel,
		.color = RED,
		.vertCount = 4,
		.inv_mass = 1.0f/mass
	};
	
	body.shape[0] = (Vector2){w/2.0f, h/2.0f};
	body.shape[1] = (Vector2){w/2.0f, -h/2.0f};
	body.shape[2] = (Vector2){-w/2.0f, -h/2.0f};
	body.shape[3] = (Vector2){-w/2.0f, h/2.0f};
	
	return body;
}

static void AddBody(Body body, Game *game){
	game->bodies[game->bodyCount++] = body;
}

void InitGame(Game *game){	
	AddBody(CreateCircle((Vector2){100.0f,100.0f}, (Vector2){50.0f, 0.0f}, 20.0f, 1.0f), game);

	AddBody(CreateRectangleangle((Vector2){320.0f,150.0f}, (Vector2){0.0f, 0.0f}, 20.0f, 20.0f, 1.0f), game);

	for(int i = 0; i < 10; i++){
		AddBody(CreateCircle((Vector2){300.0f+40*i+i,100.0f}, (Vector2){0.0f, 0.0f}, 20.0f, 1.0f), game);
	}

	return;
}


//TODO: remove this
#define MAX_COLLISIONS BODY_MAX
static int CheckCollisions(Body *bodies, int count, Collision *collisions){	
	int numPotentialCollisions = 0;
	Collision *potentialCollisions = malloc(sizeof(Collision)*MAX_COLLISIONS);
	for(int i = 0; i < count-1; i++){
		Body *body1 = bodies+i;
		for(int j = i+1; j < count; j++){
			Body *body2 = bodies+j;

			if(CheckCollisionRecs(body1->AABB, body2->AABB)){
				Vector2 diff = Vector2Subtract(body1->position, body2->position);
				
				potentialCollisions[numPotentialCollisions++] = (Collision){
					.body1 = body1,
					.body2 = body2,					
				};
				
			}
		}
	}
	
	int numCollisions = 0;
	
	for(int i = 0; i < numPotentialCollisions; i++){
		Collision *potCollision = potentialCollisions+i;
		Body *body1 = potCollision->body1;
		Body *body2 = potCollision->body2;
		
		float penDist = INFINITY;
		Vector2 penNorm = (Vector2){1.0, 0.0};
		
		bool collides = true;
		for(int b = 0; b < 2; b++){
			for(int i = 0; i < body1->vertCount-1; i++){
				Vector2 P1 = body1->vertices[i];
				Vector2 P2 = body1->vertices[(i+1) % body1->vertCount];
				Vector2 deltaP = Vector2Normalize(Vector2Subtract(P1, P2));
				Vector2 n = (Vector2){-deltaP.y, deltaP.x};
				
				float b1_min = INFINITY;
				float b1_max = -INFINITY;
				float b2_min = INFINITY;
				float b2_max = -INFINITY;
				for(int j = 0; j < body1->vertCount; j++){
					float proj = Vector2DotProduct(n, body1->vertices[j]);
					if(proj > b1_max){
						b1_max = proj;
					}
					if(proj < b1_min){
						b1_min = proj;
					}
				}
				
				for(int j = 0; j < body2->vertCount; j++){
					float proj = Vector2DotProduct(n, body2->vertices[j]);
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
					float curPenDist = MIN(b1_max, b2_max) - MAX(b2_min, b2_max);
					if(curPenDist < penDist){
						penDist = curPenDist;
						if(b == 0)
							penNorm = n;
						else
							penNorm = Vector2Scale(n, -1.0f);
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
			numCollisions++;
		}
	}
	
	free(potentialCollisions);
	
	return numCollisions;
}

static void ResolveCollisions(Collision *collisions, int count, Body *bodies, int bodyCount, float dt){
	for(int i = 0; i < count; i++){
		Collision *collision = collisions+i;
		
		Body *body1 = collision->body1;
		Body *body2 = collision->body2;

		float vRelNorm = Vector2DotProduct(Vector2Subtract(body2->velocity, body1->velocity), collision->normal);
		
		if(vRelNorm > 0.0f){
			continue;
		}
		
		//changed to 1.9
		Vector2 j = Vector2Scale(collision->normal, -1.9f*vRelNorm/(body1->inv_mass+body2->inv_mass));
						
		body1->impulse = Vector2Subtract(body1->impulse, j);
		body2->impulse = Vector2Add(body2->impulse, j);
	}
	
	
	for(int i = 0; i < bodyCount; i++){
		Body *body = bodies+i;
		body->velocity = Vector2Add(body->velocity, Vector2Scale(body->impulse, body->inv_mass));
	}
	
}

static void UpdateBodies(Body *bodies, int count, float dt){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		
		body->position = Vector2Add(body->position, Vector2Scale(body->velocity, dt));
				
		body->impulse = (Vector2){0.0f, 0.0f};
	}
}

static void UpdateAABB(Body *bodies, int count){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		Vector2 max = body->vertices[0];
		Vector2 min = body->vertices[0];
		for(int j = 0; j < body->vertCount; j++){
			Vector2 vert = body->vertices[j];
			
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
		body->AABB = (Rectangle){min.x, min.y, max.x-min.x, max.y-min.y};
	}
}

static void TranslateBodies(Body *bodies, int count){
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		body->rotation += 0.001;
		for(int j = 0; j < body->vertCount; j++){
			float angle = body->rotation;
			Vector2 rotated = Vector2Rotate(body->shape[j], angle*RAD2DEG);
			Vector2 vertex = Vector2Add(rotated, body->position);
			body->vertices[j] = vertex;
		}
	}
}

static void DrawBodies(Body *bodies, int count){
	
	for(int i = 0; i < count; i++){
		Body *body = bodies+i;
		DrawLineStrip(body->vertices, body->vertCount, body->color);
		DrawLineV(body->vertices[0], body->vertices[body->vertCount-1], body->color);
		DrawLineV(body->position, body->vertices[0], body->color);

	}

}

static void MouseInputUpdate(Game *game, float dt){
	Vector2 mousePos = GetMousePosition();
	
	if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
		AddBody(CreateCircle(mousePos, (Vector2){0.0f, 0.0f}, 20.0f, 1.0f), game);
	}
	
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){		
		for(int i = 0; i < game->bodyCount; i++){
			Body *body = game->bodies+i;
			if(CheckCollisionPointRec(mousePos, body->AABB)){
				game->draggingBody = 1;
				game->latchedBody = body;
			}
		}
	}
	
	if(game->draggingBody){
		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			game->draggingBody = 0;
			game->latchedBody = 0;
			return;
		}
		
		Vector2 displacement = Vector2Subtract(mousePos, game->latchedBody->position);
		
		game->latchedBody->velocity = Vector2Add(game->latchedBody->velocity, Vector2Scale(displacement, dt*3.0f));
		game->latchedBody->velocity = Vector2Scale(game->latchedBody->velocity, 1.0f-dt);
	}
}

void UpdateGame(Game *game, float dt){
	MouseInputUpdate(game, dt);
	
	UpdateBodies(game->bodies, game->bodyCount, dt);
		
	TranslateBodies(game->bodies, game->bodyCount);
	UpdateAABB(game->bodies, game->bodyCount);
		
	Collision *collisions = malloc(sizeof(Collision)*MAX_COLLISIONS);
	int numCollisions = CheckCollisions(game->bodies, game->bodyCount, collisions);	
		
	ResolveCollisions(collisions, numCollisions, game->bodies, game->bodyCount, dt);	
	free(collisions);
	
	return;
}

void DrawGame(Game *game, float dt){
	ClearBackground(RAYWHITE);
	DrawFPS(10,10);
	
	char bcountString[64];
	snprintf(bcountString, 64, "Body count: %i", game->bodyCount);
	DrawText(bcountString, 10, 30, 20, GREEN);
	
	if(game->draggingBody){
		DrawLineV(game->latchedBody->position, GetMousePosition(), VIOLET);
	}
	
	DrawBodies(game->bodies, game->bodyCount);
}