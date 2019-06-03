#include <math.h>
#include "vec2.h"
#include "log.h"

bool
vec2_eq_vec2i(vec2 v1, vec2i v2)
{
	int32_t x = (int32_t)floor(v1.x);
	int32_t y = (int32_t)floor(v1.y);

	if (x == v2.x && y == v2.y) return true;

	return false;
}

vec2i
vec2i_init(int32_t x, int32_t y)
{
	vec2i r = {x, y};
	return r;
}

vec2
vec2_init(int32_t x, int32_t y)
{
	vec2 r = {(float)x, (float)y};
	return r;
}

vec2
vec2_init_f(float x, float y)
{
	vec2 r = {x, y};
	return r;
}

vec2 
vec2_add(vec2 v1, vec2 v2)
{
	vec2 value = {v1.x + v2.x, v1.y + v2.y};
	return value;
}

vec2
vec2_add_f(vec2 v, float f)
{
	vec2 value = {v.x + f, v.y + f};
	return value;
}

vec2 
vec2_sub(vec2 v1, vec2 v2)
{
	vec2 value = {v1.x - v2.x, v1.y - v2.y};
	return value;
}

vec2
vec2_sub_f(vec2 v, float f)
{
	vec2 value = {v.x - f, v.y - f};
	return value;
}

vec2 
vec2_mul(vec2 v, double d)
{
	vec2 value = {v.x * d, v.y * d};
	return value;
}

vec2 
vec2_norm(vec2 v)
{
	return vec2_mul(v, 1 / vec2_mag(v));
}

vec2
vec2_floor(vec2 v)
{
	vec2 value = {floor(v.x), floor(v.y)};
	return value;
}

vec2
vec2_lerp(vec2 pos, vec2 dst, float f) 
{
	float x = (pos.x * (1.0 - f)) + (dst.x * f);	
	float y = (pos.y * (1.0 - f)) + (dst.y * f);	
	vec2 r = {x, y};
	return r;
}

bool
vec2_equal(vec2 v1, vec2 v2)
{
	v1 = vec2_floor(v1);
	v2 = vec2_floor(v2);
	return v1.x == v2.x && v1.y == v2.y;
}

float 
vec2_mag(vec2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

float 
vec2_dot(vec2 v1, vec2 v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float 
vec2_dist(vec2 v1, vec2 v2)
{
	return vec2_mag(vec2_sub(v1, v2));
}

