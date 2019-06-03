#ifndef TEXTURE__
#define TEXTURE__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <dirent.h>

#define TEX_MAX_PATH_LEN 512
#define TEX_MAX_DIR_COUNT 20

struct
texture_set
{
	char tex_path[TEX_MAX_PATH_LEN];
	struct SDL_Texture *tex;
};

struct
texture_cache
{
	struct texture_set *cache;
	uint16_t count;
};

void
texture_init(SDL_Renderer *renderer);

void
texture_destroy(void);

const struct texture_set*
texture_get_tex_set(const char *path);

SDL_Texture*
texture_get(const char *path);


#endif

