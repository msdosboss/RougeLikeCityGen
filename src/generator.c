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


Vector2 getRandomCoord(){
    return (Vector2){
        .x = GetRandomValue(0, NUMSQUAREWIDTH),
        .y = GetRandomValue(0, NUMSQUAREHEIGHT)
    };
}


Vector2 getRandomLandCoord(MapData *mapData){
    Vector2 landCoord;
    do{
        landCoord = getRandomCoord();
    }
    while(coordIsInWater(mapData, landCoord));
    return landCoord;
}


float distBetweenTwoPoints(Vector2 a, Vector2 b){
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}


void createDistrictsCoords(MapData *mapData, Vector2 *districts, const int districtCount){
    for(int i = 1; i < districtCount;){
        districts[i] = getRandomLandCoord(mapData);
        int isToClose = 0;
        //Check over all the previous points to make sure they are not to close
        for(int j = i - 1; j >= 0; j--){
            if(distBetweenTwoPoints(districts[i], districts[j]) < NUMSQUAREWIDTH/ 20){
                isToClose = 1;
                break;
            } 
        }
        if(!isToClose){
            i++;
        }
    } 
}


RoadGraph urbanPlanner(MapData *mapData, Vector2 center, Vector2 *districts, const int districtCount){
    RoadGraph roadGraph = {0};
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    int currentVertexIndex = addVertex(&roadGraph, center.x, center.y);
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
                .radial = 0.0,
                .grid = 0.95,
                .noise = 0.05
            };
            traceRoads(&roadGraph, mapData, currentVertexIndex, roadOrigin, center, &tensorWeights, &noise);
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
        traceRoads(&roadGraph, mapData, i, roadOrigin, districts[i % districtCount], &tensorWeights, &noise);
    }

    removeDeadEnds(&roadGraph);

    return roadGraph;
}

//Returns 0 if a is closer and 1 if b is closer 
int whichVertexIsCloser(Vertex origin, Vertex a, Vertex b){
    float x = origin.x - a.x;
    float y = origin.y - a.y;
    float distFromA = sqrt(x * x + y * y);
    x = origin.x - b.x;
    y = origin.y - b.y;
    float distFromB = sqrt(x * x + y * y);
    if(distFromA > distFromB){
        return 1;
    }
    else{
        return 0;
    }
}


void traceRoads(
    RoadGraph *roadGraph,
    MapData *mapData,
    int startVertex,
    Vector2 startCoord,
    Vector2 center,
    TensorWeights *tensorWeights,
    fnl_state *noise
){
    int currentRoadIndex;
    int prevRoadIndex = startVertex;
    const int stepSize = 10;
    const int maxStepCount = 400;
    int stepCount = 0;
    Vector2 currentCoord = {
        .x = startCoord.x,
        .y = startCoord.y
    };
    //addVertex(roadGraph, currentCoord.x, currentCoord.y);

    int lastEdgeDrawnbyThisTrace = -1;
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
                if(i == lastEdgeDrawnbyThisTrace){
                    continue;
                }
                currentCoord = intersectionPoint;
                currentRoadIndex = addVertex(roadGraph, currentCoord.x, currentCoord.y);
                addEdge(roadGraph, currentRoadIndex, prevRoadIndex);
                //Snapping to the closest vertex
                int bIndex = roadGraph->edges[i].destIndex;
                roadGraph->edges[i].destIndex = currentRoadIndex;
                addEdge(roadGraph, currentRoadIndex, bIndex);
                didIntersect = 1;
                break;
            } 
        }
        if(didIntersect){
            break;
        }
        currentRoadIndex = addVertex(roadGraph, currentCoord.x, currentCoord.y);
        addEdge(roadGraph, prevRoadIndex, currentRoadIndex);
        lastEdgeDrawnbyThisTrace = roadGraph->edgeCount - 1;
        prevRoadIndex = currentRoadIndex;
        currentRoadIndex++;
        stepCount++; 
    }
}
