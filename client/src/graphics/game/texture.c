#include <sys/stat.h>
#include "texture.h"
#include "util/log.h"
#include "util/util_funcs.h"
#include "../../paths.h"

static struct texture_cache tex_cache;

void
texture_init(SDL_Renderer *renderer)
{
	tex_cache.count = 0;

	DIR *dir;
	struct dirent *ent;
	
	char **dir_names = malloc(TEX_MAX_DIR_COUNT * sizeof(char*));
	for (int i = 0; i < TEX_MAX_DIR_COUNT; i++)
	{
		dir_names[i] = malloc(TEX_MAX_PATH_LEN);
	}

	uint16_t dir_count = 0;

	if ((dir = opendir(PATH_ART)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (!strcmp(ent->d_name, ".")) continue;
			if (!strcmp(ent->d_name, "..")) continue;
			strcpy(dir_names[dir_count], ent->d_name);

			dir_count++;
		}

		closedir(dir);
	}
	else
	{
		log_fatal("Failed to find art directory!");
		return;
	}
	
	struct texture_set sets[1024];

	for (int i = 0; i < dir_count; i++)
	{	

		DIR *t_dir;
		struct dirent *t_ent;

		char *dir_path_full = malloc(TEX_MAX_PATH_LEN);
		strcpy(dir_path_full, PATH_ART);
		strcat(dir_path_full, dir_names[i]);
		strcat(dir_path_full, "/");

		if ((t_dir = opendir(dir_path_full)) != NULL)
		{
			while ((t_ent = readdir(t_dir)) != NULL)
			{
				if (!strcmp(t_ent->d_name, ".")) continue;
				if (!strcmp(t_ent->d_name, "..")) continue;
				
				char *file_path = malloc(TEX_MAX_PATH_LEN);

				strcpy(file_path, dir_path_full);
				strcat(file_path, t_ent->d_name);

				log_info("%d: Loading Texture: %s", tex_cache.count+1, file_path);

				struct texture_set t_s;
				strcpy(t_s.tex_path, file_path);

				struct SDL_Texture *tex = IMG_LoadTexture(renderer, file_path);

				if (tex == NULL)
				{
					log_fatal("Failed to load texture: %s", file_path);
					return;
				}

				t_s.tex = tex;

				sets[tex_cache.count] = t_s;

				tex_cache.count++;
				
				free(file_path);

			}
			
			closedir(t_dir);
		}
		else
		{
			log_fatal("Failed to loop texture directories %s", dir_path_full);
			return;
		}

		free(dir_path_full);
	}
	

	for (int i = 0; i < TEX_MAX_DIR_COUNT; i++)
	{
		free(dir_names[i]);
	}
	free(dir_names);

	tex_cache.cache = malloc(sizeof(struct texture_set) * tex_cache.count);
	for (int i = 0; i < tex_cache.count; i++)
	{
		tex_cache.cache[i] = sets[i];	
	}

}

void
texture_destroy(void)
{
	for (int i = 0; i < tex_cache.count; i++)
	{
		SDL_DestroyTexture(tex_cache.cache[i].tex);
	}

	free(tex_cache.cache);
}

const struct texture_set*
texture_get_tex_set(const char *path)
{
	for (int i = 0; i < tex_cache.count; i++)
	{
		if (strcmp(tex_cache.cache[i].tex_path, path) == 0)
		{
			return &tex_cache.cache[i];
		}
	}

	log_fatal("Failed to get texture: %s", path);

	return NULL;
}

SDL_Texture*
texture_get(const char *path)
{
	for (int i = 0; i < tex_cache.count; i++)
	{
		if (strcmp(tex_cache.cache[i].tex_path, path) == 0)
		{
			return tex_cache.cache[i].tex;
		}
	}

	log_fatal("Failed to get texture: %s", path);

	return NULL;
}


