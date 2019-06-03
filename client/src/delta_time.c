#include <SDL2/SDL.h>
#include "delta_time.h"

double ACCUMULATOR_RATIO = 0;
double PREV_R_PERC = 0;
double R_PERC = 0;
double DELTA_TIME = 0;
double LAST_TIME = 0;
uint32_t FPS_FPS = 0;
uint32_t FPS_TIMER = 0;
uint32_t FPS_COUNT = 0;

void
delta_time_calculate(void)
{
	float current_time = SDL_GetTicks();
	DELTA_TIME = current_time - LAST_TIME;
	LAST_TIME = current_time;
}

void
fps_calculate(void)
{		
	FPS_TIMER += DELTA_TIME;
	FPS_COUNT++;
	if (FPS_TIMER >= 1000)
	{
		FPS_FPS = FPS_COUNT;
		FPS_COUNT = 0;
		FPS_TIMER = 0;
	}
}


