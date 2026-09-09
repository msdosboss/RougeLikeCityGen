#ifndef ROAD_GRAPH_H
#define ROAD_GRAPH_H

#include <stdio.h>

#define MAX_EDGES 64000
#define MAX_VERTICES 64000

typedef struct {
    float x;
    float y;
} Vertex;

typedef struct{
    int srcIndex;
    int destIndex;
} Edge;

typedef struct {
    Vertex vertices[MAX_VERTICES];
    Edge edges[MAX_EDGES];
    int verticesCount;
    int edgeCount;
} RoadGraph;

int addVertex(RoadGraph *roadGraph, float x, float y);
int addEdge(RoadGraph *roadGraph, int srcIndex, int destIndex);
void removeDeadEnds(RoadGraph *roadGraph);

#endif
