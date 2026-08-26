#include "roadGraph.h"

void addEdge(RoadGraph *roadGraph, int srcIndex, int destIndex){
    if(roadGraph->edgeCount >= MAX_EDGES){
        fprintf(stderr, "Tried to add an edge to a full edgeList\n");
        return;
    }
    roadGraph->edges[roadGraph->edgeCount].srcIndex = srcIndex;
    roadGraph->edges[roadGraph->edgeCount].destIndex = destIndex;
    roadGraph->edgeCount++;
}

void addVertex(RoadGraph *roadGraph, float x, float y){
    if(roadGraph->verticesCount >= MAX_VERTICES){
        fprintf(stderr, "Tried to add an vertex to a full verticesList\n");
        return;
    }
    roadGraph->vertices[roadGraph->verticesCount].x = x;
    roadGraph->vertices[roadGraph->verticesCount].y = y;
    roadGraph->verticesCount++;
}
