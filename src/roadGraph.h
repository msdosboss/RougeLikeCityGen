#ifndef ROAD_GRAPH_H
#define ROAD_GRAPH_H

#include <stdio.h>

#define MAX_EDGES 2048
#define MAX_VERTICES 2048

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

void addVertex(RoadGraph *roadGraph, float x, float y);
void addEdge(RoadGraph *roadGraph, int srcIndex, int destIndex);

#endif
