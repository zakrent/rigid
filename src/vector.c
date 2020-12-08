#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "vector.h"

v2 Vec2(real x, real y){
	return (v2){.x = x, .y = y};
}

v2 V2Add(v2 a, v2 b){
	v2 res;
	res.x = a.x+b.x;
	res.y = a.y+b.y;
	return res;
}

v2 V2Sub(v2 a, v2 b){
	v2 res;
	res.x = a.x-b.x;
	res.y = a.y-b.y;
	return res;
}

v2 V2Mul(real a, v2 b){
	v2 res;
	res.x = a*b.x;
	res.y = a*b.y;
	return res;
}

real V2Dot(v2 a, v2 b){
	return a.x*b.x+a.y*b.y;
}

v2 V2Norm(v2 a){
	double len = V2Len(a);
	real invlen = 1.0/len;
	return V2Mul(invlen, a);
}

real V2Len(v2 a){
	return sqrt(a.x*a.x + a.y*a.y);
}

v3 Vec3(real x, real y, real z){
	return (v3){.x = x, .y = y, .z = z};
}

v4 Vec4(real x, real y, real z, real w){
	return (v4){.x = x, .y = y, .z = z, .w = w};
}

rect Rect(real x, real y, real w, real h){
	return (rect){.x = x, .y = y, .w = w, .h = h};
}
