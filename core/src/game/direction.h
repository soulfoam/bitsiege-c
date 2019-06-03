#ifndef DIRECTION__
#define DIRECTION__

//this must remain in this order because we export things clockwise
//things will break if this is in a different order
//
enum
direction
{
	DIR_N,
	DIR_NE,
	DIR_E,
	DIR_SE,
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW,

	DIR_COUNT,

	DIR_NONE
};

#endif
