#define FNL_IMPL
#include "generator.h"


void generateMap(MapData *mapData, float *scale, float *seaLevel, Vector2 landOrigin){
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    //Longest distance between to points
    double maxDist = sqrt(NUMSQUAREWIDTH * NUMSQUAREWIDTH + NUMSQUAREHEIGHT * NUMSQUAREHEIGHT);

    for(int x = 0; x < NUMSQUAREWIDTH; x++){
        for(int y = 0; y < NUMSQUAREHEIGHT; y++){
            float pixelNoise = fnlGetNoise2D(&noise, x * *scale, y * *scale);
            double distFromCoast = sqrt(((x - landOrigin.x) * (x - landOrigin.x)) + 
                                        ((y - landOrigin.y) * (y - landOrigin.y)));
            //Distance from coast as a percentage compaired to the maxDist
            double percFromCoast = distFromCoast / maxDist;
            pixelNoise = (pixelNoise + 1) / 2;
            pixelNoise -= percFromCoast;
            if(pixelNoise > *seaLevel){
                mapData->grid[x][y].type = CELLCOLORGREEN;
            }
            else{
                mapData->grid[x][y].type = CELLCOLORBLUE;
            }
        }
    }
}


Vector2 tensorField(Vector2 worldCoord, Vector2 center){
    Vector2 dCoord = {
        .x = worldCoord.x - center.x,
        .y = worldCoord.y - center.y
    };
    //Rotate 90 degrees
    float tmp = -dCoord.y;
    dCoord.y = dCoord.x;
    dCoord.x = tmp;
    //Normilize vector
    double vecMagnitude = sqrt(dCoord.x * dCoord.x + dCoord.y * dCoord.y);
    if(vecMagnitude != 0.0){
        dCoord.x = dCoord.x / vecMagnitude;
        dCoord.y = dCoord.y / vecMagnitude;
    }

    return dCoord;
}


int coordIsInWater(MapData *mapData, Vector2 coord){
    int xGridCoord = (int)floor(coord.x / SQUARESIZE);
    int yGridCoord = (int)floor(coord.y / SQUARESIZE);
    if(mapData->grid[xGridCoord][yGridCoord].type == CELLCOLORBLUE){
        return 1;
    }
    else{
        return 0;
    }
}


VectorEquation findVectorEquation(Vector2 startCoord, Vector2 endCoord){
    VectorEquation vectorEquation = {0};
    vectorEquation.slope = (endCoord.y - startCoord.y) / (endCoord.x - startCoord.x);
    vectorEquation.yIntercept = startCoord.y - vectorEquation.slope * startCoord.x;
    return vectorEquation;
}


int isIntersect(RoadGraph *roadGraph, Edge canidateEdge, Edge oldEdge, Vector2 *intersectionPoint){
    Vertex v1 = roadGraph->vertices[canidateEdge.srcIndex];
    Vertex v2 = roadGraph->vertices[canidateEdge.destIndex];
    Vertex v3 = roadGraph->vertices[oldEdge.srcIndex];
    Vertex v4 = roadGraph->vertices[oldEdge.destIndex];
    //D = (x1 - x2)(y3 - y4) - (y1 - y2)(x3 - x4)
    float denominator = (v1.x - v2.x) * (v3.y - v4.y) - (v1.y - v2.y) * (v3.x = v4.x);
    if(denominator == 0){
        return 0;
    }
    //X(t) = x1 + t(x2 - x1)
    //Y(t) = y1 + t(y2 - y1)
    //X(u) = x3 + u(x4 - x3)
    //Y(u) = y3 + u(y4 - y3)
    //Where X(t) and Y(t) represent the x and y coords t percent through the vector
    //This allows us to set X(t) = X(u) and Y(t) = Y(u) because they will be equal when they intersect
    //We then us systems of equations to solve for t and u. if t and u are inclusivly between 0 and 1
    //we know intersect in the existing line
    float t = ((v1.x - v3.x) * (v3.y - v4.y) - (v1.y - v3.y) * (v3.x - v4.x)) / denominator;
    float u = ((v1.x - v3.x) * (v1.y - v2.y) - (v1.y - v3.y) * (v1.x - v2.x)) / denominator;
    if(u < 0.0 || u > 1.0){
        return 0;
    }
    else if(u < 0.0 || u > 1.0){
        return 0;
    }

    intersectionPoint->x = v1.x + t * (v2.x - v1.x);
    intersectionPoint->y = v1.y + t * (v2.y - v2.x);

    return 1;
}


void traceRoads(RoadGraph *roadGraph, MapData *mapData, Vector2 startCoord, Vector2 center){
    int prevRoadIndex = roadGraph->verticesCount;
    int currentRoadIndex = roadGraph->verticesCount + 1;
    const int stepSize = 10;
    const int maxStepCount = 400;
    int stepCount = 0;
    Vector2 currentCoord = {
        .x = startCoord.x,
        .y = startCoord.y
    };
    addVertex(roadGraph, currentCoord.x, currentCoord.y);

    while(stepCount < maxStepCount){
        Vector2 roadVec = tensorField(currentCoord, center);
        currentCoord.x += roadVec.x * stepSize;
        currentCoord.y += roadVec.y * stepSize;
        if(coordIsInWater(mapData, currentCoord)){
            break;
        }
        if(currentCoord.y < 0 || currentCoord.y >= HEIGHT || currentCoord.x < 0 || currentCoord.x >= WIDTH){
            break;
        }
        addVertex(roadGraph, currentCoord.x, currentCoord.y);
        addEdge(roadGraph, prevRoadIndex, currentRoadIndex);
        prevRoadIndex = currentRoadIndex;
        currentRoadIndex++;
        stepCount++; 
    }
}
