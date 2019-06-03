#ifndef UTIL_FUNCS__
#define UTIL_FUNCS__

#include <inttypes.h>
#include <stdbool.h>

#define clamp(v, min, max) (((v) > (max)) ? (max) : (((v) < (min)) ? (min) : (v)))

int32_t
round_up(int32_t num, int32_t multiple);

char*
trim_white_space(char *str);

void 
strip_file_ext(char *fname);

union color
lerp_rgb(union color a, union color b, float t);

#endif

