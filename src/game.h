#define BODY_MAX_VERT 32
#define BODY_MAX 32

typedef struct{
	v2 position;
	v2 velocity;
	rect AABB;
	u32 vertCount;
	real inv_mass;
	v2 shape[BODY_MAX_VERT];
	v2 vertices[BODY_MAX_VERT]; //Shape translated to world space
	
	//Debug vars:
	v3 color;
	bool collides;
} Body;

typedef struct{
	Body *body1;
	Body *body2;
	v2 normal;
	real depth;
} Collision;

typedef struct{
	u32 bodyCount;
	Body bodies[BODY_MAX];
} Game;

void InitGame(Game *game);
void Frame(Game *game, real dt, SDL_Renderer *renderer);