#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "tiledata.h"
#include "player.h"

extern Tile_datum TILES[T_max];
extern int COST[S_max];
extern int AMT[S_max];

void init_data();

#endif
