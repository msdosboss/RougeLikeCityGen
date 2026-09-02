#include "main.h"

int main(){
    InitWindow(WIDTH, HEIGHT, "City Map");
    MapData mapData;
    float scale = 0.08;
    float seaLevel = 0.4;
    Vector2 landOrigin = {
        .x = NUMSQUAREWIDTH / 2,
        .y = NUMSQUAREHEIGHT / 4
    };
    int toggleMode = TOGGLE_SCALE;
    generateMap(&mapData, &scale, &seaLevel, landOrigin);
    RoadGraph roadGraph = urbanPlanner(&mapData, landOrigin);
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
            roadGraph = urbanPlanner(&mapData, landOrigin);
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
            roadGraph = urbanPlanner(&mapData, landOrigin);
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
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    for(int x = 0; x < NUMSQUAREWIDTH; x += 15){
        for(int y = 0; y < NUMSQUAREHEIGHT; y += 15){
            Vector2 coord = {
                .x = x * SQUARESIZE,
                .y = y * SQUARESIZE
            };
            TensorWeights tensorWeights = {
                .radial = 0.5,
                .grid = 0.5,
                .noise = 0.0
            };
            Vector2 path = tensorField(coord, centerCoord, &tensorWeights, &noise);
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
        DrawLineEx(startPos, endPos, 2.0, BLACK);
    }
}
