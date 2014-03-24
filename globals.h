#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "tiledata.h"
#include "player.h"
#include "monster.h"

extern Tile_datum TILES[T_max];
extern int COST[S_max];
extern int AMT[S_max];
extern Monster_type MONSTERS[M_max];

void init_data();

#endif
