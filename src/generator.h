#ifndef GENERATOR_H
#define GENERATOR_H
#include <raylib.h>
#define FNL_IMPL
#include "../lib/FastNoiseLite.h"

#define WIDTH 800
#define HEIGHT 800
#define SQUARESIZE 10
#define NUMSQUAREWIDTH WIDTH/SQUARESIZE
#define NUMSQUAREHEIGHT HEIGHT/SQUARESIZE

#define CELLCOLORGREEN 0
#define CELLCOLORBLUE 1

typedef struct {
    float type;
} Cell;

typedef struct {
    Cell grid[NUMSQUAREWIDTH][NUMSQUAREHEIGHT];
} MapData;

void generateMap(MapData *mapData);

#endif
