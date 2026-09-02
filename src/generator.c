#define FNL_IMPL
#define _XOPEN_SOURCE 500
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


Vector2 tensorField(Vector2 worldCoord, Vector2 center, TensorWeights *tensorWeights, fnl_state *noise){
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
    Vector2 radialVector = dCoord;
    Vector2 gridVector;
    if(fabs(radialVector.x) > fabs(radialVector.y)){
        gridVector.x = radialVector.x > 0 ? 1 : -1;
        gridVector.y = 0;
    }
    else{
        gridVector.y = radialVector.y > 0 ? 1 : -1;
        gridVector.x = 0;
    }
    float pixelNoise = fnlGetNoise2D(noise, worldCoord.x, worldCoord.y);
    pixelNoise *= 2 * M_PI;
    Vector2 noiseVector = {
        .x = cos(pixelNoise),
        .y = sin(pixelNoise)
    };

    dCoord.x = radialVector.x * tensorWeights->radial + gridVector.x * tensorWeights->grid + noiseVector.x * tensorWeights->noise;
    dCoord.y = radialVector.y * tensorWeights->radial + gridVector.y * tensorWeights->grid + noiseVector.y * tensorWeights->noise;

    //Normilize vector
    vecMagnitude = sqrt(dCoord.x * dCoord.x + dCoord.y * dCoord.y);
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


int isIntersect(
    RoadGraph *roadGraph, 
    Vector2 canidateStart, 
    Vector2 canidateEnd, 
    Edge oldEdge, 
    Vector2 *intersectionPoint
){
    Vertex v1 = {
        .x = canidateStart.x,
        .y = canidateStart.y
    };
    Vertex v2 = {
        .x = canidateEnd.x,
        .y = canidateEnd.y
    };
    Vertex v3 = roadGraph->vertices[oldEdge.srcIndex];
    Vertex v4 = roadGraph->vertices[oldEdge.destIndex];
    //D = (x1 - x2)(y3 - y4) - (y1 - y2)(x3 - x4)
    float denominator = (v1.x - v2.x) * (v3.y - v4.y) - (v1.y - v2.y) * (v3.x - v4.x);
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
    else if(t < 0.0 || t > 1.0){
        return 0;
    }

    intersectionPoint->x = v1.x + t * (v2.x - v1.x);
    intersectionPoint->y = v1.y + t * (v2.y - v1.y);

    return 1;
}


RoadGraph urbanPlanner(MapData *mapData, Vector2 center){
    RoadGraph roadGraph = {0};
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    //Primary roads
    for(int x = -10; x <= 4; x += 10){
        for(int y = -10; y <= 4; y += 10){
            //Don't want to run it with roadOrigin=center
            if(y == 0 && x == 0){
                continue;
            }
            Vector2 roadOrigin = {
                .x = center.x + x,
                .y = center.y + y
            }; 
            TensorWeights tensorWeights = {
                .radial = 0.3,
                .grid = 0.65,
                .noise = 0.05
            };
            traceRoads(&roadGraph, mapData, roadOrigin, center, &tensorWeights, &noise);
        }
    }
    //Branching from primary roads
    int originalVerticesCount = roadGraph.verticesCount;
    for(int i = 0; i < originalVerticesCount; i += 20){
        //Can't use exact vertex because it will determinstically return the same edge it already made
        Vector2 roadOrigin = {
            .x = roadGraph.vertices[i].x + 5,
            .y = roadGraph.vertices[i].y - 5
        };
        if(roadOrigin.x > NUMSQUAREWIDTH || roadOrigin.y < 0){
            continue;
        }
        TensorWeights tensorWeights = {
            .radial = 0.1,
            .grid = 0.8,
            .noise = 0.1
        };
        traceRoads(&roadGraph, mapData, roadOrigin, center, &tensorWeights, &noise);
    }
    return roadGraph;
}


void traceRoads(
    RoadGraph *roadGraph,
    MapData *mapData,
    Vector2 startCoord,
    Vector2 center,
    TensorWeights *tensorWeights,
    fnl_state *noise
){
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
        Vector2 roadVec = tensorField(currentCoord, center, tensorWeights, noise);
        Vector2 oldCoord = {
            .x = currentCoord.x,
            .y = currentCoord.y
        };
        currentCoord.x += roadVec.x * stepSize;
        currentCoord.y += roadVec.y * stepSize;
        if(coordIsInWater(mapData, currentCoord)){
            break;
        }
        if(currentCoord.y < 0 || currentCoord.y >= HEIGHT || currentCoord.x < 0 || currentCoord.x >= WIDTH){
            break;
        }
        int didIntersect = 0;
        //#TODO Going to iterate through each edge for now future improvement is a spatial hash grid
        for(int i = 0; i < roadGraph->edgeCount - 1; i++){
            Vector2 intersectionPoint;
            if(isIntersect(
                roadGraph,
                oldCoord, 
                currentCoord,
                roadGraph->edges[i],
                &intersectionPoint)
            ){
                currentCoord = intersectionPoint;
                didIntersect = 1;
                break;
            } 
        }
        addVertex(roadGraph, currentCoord.x, currentCoord.y);
        addEdge(roadGraph, prevRoadIndex, currentRoadIndex);
        if(didIntersect){
            break;
        }
        prevRoadIndex = currentRoadIndex;
        currentRoadIndex++;
        stepCount++; 
    }
}
