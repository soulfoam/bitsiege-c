#ifndef DELTA_TIME__
#define DELTA_TIME__

#include <inttypes.h>

extern double ACCUMULATOR_RATIO;
extern double PREV_R_PERC;
extern double R_PERC;
extern double DELTA_TIME;
extern uint32_t FPS_FPS;

void
delta_time_calculate(void);

void
fps_calculate(void);

#endif

