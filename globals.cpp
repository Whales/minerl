#include "globals.h"

Tile_datum TILES[T_max];

void init_tiles()
{
  TILES[T_null]     = Tile_datum(c_black,   c_black,  false, false, false, 0, 0,
                                 "");
  TILES[T_empty]    = Tile_datum(c_black,   c_black,  false, false, false, 0, 0,
                                 "");
  TILES[T_sky]      = Tile_datum(c_cyan,    c_cyan,   false, false, false, 0, 0,
                                 "");
  TILES[T_grass]    = Tile_datum(c_green,   c_cyan,   false, false, false, 0, 0,
                                 "_");
  TILES[T_shop]     = Tile_datum(c_green,   c_black,  false, false, false, 0, 0,
                                 "$");
  TILES[T_dirt]     = Tile_datum(c_ltgray,  c_brown,  true,  false, false, 5, 0,
                                 "   `~,.'^");
  TILES[T_boulder]  = Tile_datum(c_ltgray,  c_black,  true,  false, false, 0, 0,
                                 "0O");
  TILES[T_ladder]   = Tile_datum(c_ltred,   c_black,  false, false, false, 0, 3,
                                 "H");
  TILES[T_support]  = Tile_datum(c_ltred,   c_black,  false, false, true,  0, 0,
                                 "|");
  TILES[T_coal]     = Tile_datum(c_black,   c_brown,  true,  true,  false, 8, 0,
                                 "#");
}
