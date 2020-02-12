#pragma once

#define TOTAL_MAP_STYLE		5

#define MAP_X				5
#define MAP_Y				5
#define BUILD_X				3
#define BUILD_Y				3
#define SIZE				1.0f



#define MIN_FLOAT_X			(-2.5f)
#define MAX_FLOAT_X			(2.5f)

#define MIN_FLOAT_Y			(-0.5f)
#define MAX_FLOAT_Y		 	(TOTAL_BUILDS*MAP_Y+MIN_FLOAT_Y)		//(TOTAL_BUILD*MAP_Y+MIN_FLOAT_Y)


static unsigned char Build[BUILD_Y][BUILD_X] = {
	{2,5,4},
	{1,3,1},
	{4,5,2},
};

static unsigned char map[TOTAL_MAP_STYLE][MAP_Y][MAP_X] = {
	{{1,1,2,1,1},
	{1,1,2,1,1},
	{1,1,2,1,1},
	{1,1,2,1,1},
	{1,1,2,1,1},},

	{{1,1,1,1,1},
	{1,2,1,2,1},
	{1,1,1,1,1},
	{1,2,1,2,1},
	{1,1,1,1,1},},

	{{2,1,2,1,2},
	{2,1,2,1,2},
	{1,1,1,1,1},
	{2,1,2,1,2},
	{2,1,2,1,2},},

	{{2,1,1,1,2},
	{1,1,2,1,1},
	{1,2,2,2,1},
	{1,1,2,1,1},
	{2,1,1,1,2},},

	{{3,1,1,1,3},
	{1,1,3,1,1},
	{1,3,3,3,1},
	{1,1,3,1,1},
	{3,1,1,1,3},},


};