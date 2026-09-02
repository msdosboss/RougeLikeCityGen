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

typedef struct {
    float slope;
    float yIntercept;
} VectorEquation;

typedef struct {
    float radial;
    float grid;
    float noise;
} TensorWeights;

void generateMap(MapData *mapData, float *scale, float *seaLevel, Vector2 coastOrigin);
Vector2 tensorField(Vector2 worldCoord, Vector2 center, TensorWeights *tensorWeights, fnl_state *noise);
void traceRoads(
    RoadGraph *roadGraph,
    MapData *mapData,
    Vector2 startCoord,
    Vector2 center,
    TensorWeights *tensorWeights,
    fnl_state *noise
);
int coordIsInWater(MapData *mapData, Vector2 coord);
int isIntersect(
    RoadGraph *roadGraph, 
    Vector2 canidateStart, 
    Vector2 canidateEnd, 
    Edge oldEdge, 
    Vector2 *intersectionPoint
);
RoadGraph urbanPlanner(MapData *mapData, Vector2 center);

#endif
