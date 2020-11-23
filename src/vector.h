
typedef struct{
	real x;
	real y;
} v2;

typedef struct{
	union{
		struct{
			real x;
			real y;
			real z;
		};
		
		struct{
			real r;
			real g;
			real b;
		};
	};
} v3;

typedef struct{
	union{
		struct{
			real x;
			real y;
			real z;
			real w;
		};
	};
} v4;

typedef struct{
	real x;
	real y;
	real w;
	real h;
} rect;

v2 Vec2(real x, real y);
v2 V2Add(v2 a, v2 b);
v2 V2Sub(v2 a, v2 b);
v2 V2Mul(real a, v2 b);
real V2Dot(v2 a, v2 b);

v3 Vec3(real x, real y, real z);

v4 Vec4(real x, real y, real z, real w);

rect Rect(real x, real y, real w, real h);
