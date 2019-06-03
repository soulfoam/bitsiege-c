#include <ctype.h>
#include <string.h>
//#include <regex.h>
#include <stdlib.h>
#include "util_funcs.h"
#include "log.h"
#include "vec2.h"
#include "util_funcs.h"
#include "../game/direction.h"
#include "game_util.h"

char*
trim_white_space(char *str)
{
	while (isspace(*str))
	{
		str++;
	}

	if (*str == 0)
	{
		return str;
	}

	char *e = str + strlen(str) - 1;
	while (e > str && isspace(*e))
	{
		e--;
	}

	*(e + 1) = '\0';

	return str;
}

int32_t
round_up(int32_t num, int32_t multiple)
{
    if (multiple == 0) return num;

    int remainder = num % multiple;
    if (remainder == 0) return num;

    return num + multiple - remainder;
}

int32_t
round_down(int32_t num, int32_t multiple)
{
	if (multiple == 0) return num;

	return num - (num % multiple);
}

void 
strip_file_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }

    if (end > fname && *end == '.') {
        *end = '\0';
    }
}

union color
lerp_rgb(union color a, union color b, float t)
{
	union color c;
	c.val.r = a.val.r + (b.val.r - a.val.r) * t;
	c.val.g = a.val.g + (b.val.g - a.val.g) * t;
	c.val.b = a.val.b + (b.val.b - a.val.b) * t;
	c.val.a = a.val.a + (b.val.a - a.val.a) * t;

	return c;
}


