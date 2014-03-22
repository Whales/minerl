#include "globals.h"

Tile_datum TILES[T_max];

void init_tiles()
{
  TILES[T_null]     = Tile_datum(c_black,   c_black,  false,  0,  "");
  TILES[T_empty]    = Tile_datum(c_black,   c_black,  false,  0,  "");
  TILES[T_sky]      = Tile_datum(c_cyan,    c_cyan,   false,  0,  "");
  TILES[T_grass]    = Tile_datum(c_green,   c_cyan,   false,  0,  "_");
  TILES[T_dirt]     = Tile_datum(c_ltgray,  c_brown,  true,   5,  "   `~,.'^");
  TILES[T_boulder]  = Tile_datum(c_ltgray,  c_black,  true,   0,  "0O");
  TILES[T_ladder]   = Tile_datum(c_brown,   c_black,  false,  0,  "H");
}
