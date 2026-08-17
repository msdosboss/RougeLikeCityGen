#include "generator.h"

void generateMap(MapData *mapData){
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

    float scale = 0.05;

    for(int x = 0; x < NUMSQUAREWIDTH; x++){
        for(int y = 0; y < NUMSQUAREHEIGHT; y++){
            float pixelNoise = fnlGetNoise2D(&noise, x * scale, y * scale);
            if(y % 2 == 0){
                mapData->grid[x][y].type = CELLCOLORGREEN;
            }
            else{
                mapData->grid[x][y].type = CELLCOLORBLUE;
            }
        }
    }
}
