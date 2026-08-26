#ifndef GENERATOR_H
#define GENERATOR_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include "roadGraph.h"
#include "../lib/FastNoiseLite.h"

#define WIDTH 800
#define HEIGHT 800
#define SQUARESIZE 1
#define NUMSQUAREWIDTH WIDTH/SQUARESIZE
#define NUMSQUAREHEIGHT HEIGHT/SQUARESIZE

#define CELLCOLORGREEN 0
#define CELLCOLORBLUE 1

typedef struct {
    int type;
} Cell;

typedef struct {
    Cell grid[NUMSQUAREWIDTH][NUMSQUAREHEIGHT];
} MapData;

void generateMap(MapData *mapData, float *scale, float *seaLevel, Vector2 coastOrigin);
Vector2 tensorField(Vector2 worldCoord, Vector2 center);
RoadGraph traceRoads(Vector2 startCoord, Vector2 center);

#endif
