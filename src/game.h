#define BODY_MAX_VERT 32
#define BODY_MAX 2048

typedef struct{
	Vector2 position;
	Vector2 velocity;
	Vector2 impulse;
	Rectangle AABB;
	float rotation;
	float inv_mass;
	int vertCount;
	Vector2 shape[BODY_MAX_VERT];
	Vector2 vertices[BODY_MAX_VERT]; //Shape translated to world space
	
	//Debug vars:
	Color color;
} Body;

typedef struct{
	Body *body1;
	Body *body2;
	Vector2 normal;
	float depth;
} Collision;

typedef struct{
	int bodyCount;
	Body bodies[BODY_MAX];
	
	Body *latchedBody;
	bool draggingBody;
} Game;

void InitGame(Game *game);
void UpdateGame(Game *game, float dt);
void DrawGame(Game *game, float dt);