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

void removeDeadEnds(RoadGraph *roadGraph){
    int verticesDegree[MAX_VERTICES] = {0};

    for(int i = 0; i < roadGraph->edgeCount; i++){
        verticesDegree[roadGraph->edges[i].destIndex]++;
        verticesDegree[roadGraph->edges[i].srcIndex]++;
    }

    int passMadeChanges;
    do{
        passMadeChanges = 0;

        for(int i = 0; i < roadGraph->edgeCount;){
            int u = roadGraph->edges[i].srcIndex;
            int v = roadGraph->edges[i].destIndex;

            if(verticesDegree[u] <= 1 || verticesDegree[v] <= 1){
                verticesDegree[u]--;
                verticesDegree[v]--; 

                roadGraph->edgeCount--;
                roadGraph->edges[i] = roadGraph->edges[roadGraph->edgeCount];
                passMadeChanges = 1;
            }
            else{
                i++;
            }
        }
    
    }
    while(passMadeChanges);
}
