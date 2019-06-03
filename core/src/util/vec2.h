#ifndef VEC2__
#define VEC2__

#include <inttypes.h>
#include <stdbool.h>

typedef 
struct 
{ 
	int32_t x; 
	int32_t y;  
} vec2i;

vec2i
vec2i_init(int32_t x, int32_t y);

typedef 
struct 
{ 
	float x; 
	float y;  
} vec2;


bool
vec2_eq_vec2i(vec2 v1, vec2i v2);

vec2
vec2_init(int32_t x, int32_t y);

vec2
vec2_init_f(float x, float y);

vec2 
vec2_add(vec2 v1, vec2 v2);

vec2
vec2_add_f(vec2 v, float f);

vec2 
vec2_sub(vec2 v1, vec2 v2);

vec2 
vec2_sub_f(vec2 v, float f);

vec2 
vec2_mul(vec2 v, double d);

vec2 
vec2_norm(vec2 v);

vec2
vec2_floor(vec2 v);

vec2
vec2_lerp(vec2 pos, vec2 dst, float f);

bool
vec2_equal(vec2 v1, vec2 v2);

float 
vec2_mag(vec2 v);

float 
vec2_dot(vec2 v1, vec2 v2);

float 
vec2_dist(vec2 v1, vec2 v2);

#endif

