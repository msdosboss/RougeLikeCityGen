#include "main.h"

int main(){
    InitWindow(WIDTH, HEIGHT, "City Map");
    MapData mapData;
    generateMap(&mapData);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderMapData(&mapData);

        EndDrawing();
    }
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
