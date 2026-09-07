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

int deadEnd(RoadGraph *roadGraph){
    int vertexDegree[MAX_VERTICES] = {0};
    for(int i = 0; i < roadGraph->edgeCount; i++){
        vertexDegree[roadGraph->edges[i].srcIndex]++;
        vertexDegree[roadGraph->edges[i].destIndex]++;
    }
    int isDeadEnd = 0;
    for(int i = 0; i < roadGraph->edgeCount; i++){
        if(vertexDegree[i] <= 1){
            roadGraph->edgeCount--;
            roadGraph->edges[i] = roadGraph->edges[roadGraph->edgeCount];
            isDeadEnd = 1;
        }
    }
    return isDeadEnd;
}

void removeDeadEnds(RoadGraph *roadGraph){
    while(deadEnd(roadGraph));
}
