#include "main.h"

int main(){
    InitWindow(WIDTH, HEIGHT, "City Map");
    MapData mapData;
    float scale = 0.08;
    float seaLevel = 0.4;
    Vector2 landOrigin = {
        .x = 0,
        .y = 0
    };
    int toggleMode = TOGGLE_SCALE;
    Vector2 center = {
        .x = NUMSQUAREWIDTH / 2,
        .y = NUMSQUAREHEIGHT / 2
    };
    Vector2 roadOrigin = {
        .x = NUMSQUAREWIDTH / 10,
        .y = NUMSQUAREHEIGHT / 8
    };
    generateMap(&mapData, &scale, &seaLevel, landOrigin);
    RoadGraph roadGraph = {0};
    traceRoads(&roadGraph, &mapData, roadOrigin, center);
    roadOrigin.x = roadGraph.vertices[roadGraph.verticesCount - 1].x;
    roadOrigin.y = roadGraph.vertices[roadGraph.verticesCount - 1].y;
    traceRoads(&roadGraph, &mapData, roadOrigin, center);
    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_Z)){
            toggleMode = TOGGLE_SCALE;
        }
        else if(IsKeyPressed(KEY_X)){
            toggleMode = TOGGLE_SEA_LEVEL;
        }
        else if(IsKeyPressed(KEY_UP)){
            switch(toggleMode){
                case TOGGLE_SCALE:
                    scale += 0.01;
                    break;
                case TOGGLE_SEA_LEVEL:
                    if(seaLevel < 1.0){
                        seaLevel += 0.05;
                    }
                    break;
            }
            generateMap(&mapData, &scale, &seaLevel, landOrigin);
        }
        else if(IsKeyPressed(KEY_DOWN)){
            switch(toggleMode){
                case TOGGLE_SCALE:
                    if(scale > 0.0){
                        scale -= 0.01;
                    }
                    break;
                case TOGGLE_SEA_LEVEL:
                    if(seaLevel > -1.0){
                        seaLevel -= 0.05;
                    }
                    break;
            }
            generateMap(&mapData, &scale, &seaLevel, landOrigin);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderMapData(&mapData);
        //renderTensorField(center);
        renderRoadGraph(&roadGraph);

        //UI Overlay
        DrawText(TextFormat("Mode: %s", toggleMode == TOGGLE_SCALE ? "SCALE" : "SEA LEVEL"), 10, 10, 20, WHITE);
        DrawText(TextFormat("Scale: %.3f", scale), 10, 40, 20, WHITE);
        DrawText(TextFormat("Sea Level: %.2f", seaLevel), 10, 70, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void renderMapData(MapData *mapData){
    for(int x = 0; x < NUMSQUAREWIDTH; x++){
        for(int y = 0; y < NUMSQUAREHEIGHT; y++){
            Color color;
            switch(mapData->grid[x][y].type){
                case CELLCOLORGREEN:
                    color = GREEN;
                    break;
                case CELLCOLORBLUE:
                    color = BLUE;
                    break;
            } 
            Rectangle rendRect = {
                .x = x * SQUARESIZE,
                .y = y * SQUARESIZE,
                .width = SQUARESIZE,
                .height = SQUARESIZE
            };
            DrawRectangleRec(rendRect, color);
        }
    }
}

void renderTensorField(Vector2 centerCoord){
    for(int x = 0; x < NUMSQUAREWIDTH; x += 15){
        for(int y = 0; y < NUMSQUAREHEIGHT; y += 15){
            Vector2 coord = {
                .x = x * SQUARESIZE,
                .y = y * SQUARESIZE
            };
            Vector2 path = tensorField(coord, centerCoord);
            path.x = path.x * 15.0;
            path.y = path.y * 15.0;
            path.x = path.x + coord.x;
            path.y = path.y + coord.y;
            DrawLineV(coord, path, RED);
        }
    }
}

void renderRoadGraph(RoadGraph *roadGraph){
    for(int i = 0; i < roadGraph->edgeCount; i++){
        Vector2 startPos = {
            .x = roadGraph->vertices[roadGraph->edges[i].srcIndex].x,
            .y = roadGraph->vertices[roadGraph->edges[i].srcIndex].y,
        };
        Vector2 endPos = {
            .x = roadGraph->vertices[roadGraph->edges[i].destIndex].x,
            .y = roadGraph->vertices[roadGraph->edges[i].destIndex].y,
        };
        DrawLineEx(startPos, endPos, 5.0, BLACK);
    }
}
