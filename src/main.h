#ifndef MAIN_H
#define MAIN_H

#include "generator.h"

#define TOGGLE_SCALE 0
#define TOGGLE_SEA_LEVEL 1

void renderMapData(MapData *mapData);
void renderTensorField(Vector2 centerCoord);
void renderRoadGraph(RoadGraph *roadGraph);

#endif
