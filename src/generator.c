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
