#include "globals.h"

Tile_datum TILES[T_max];
int COST[S_max];
int AMT[S_max];

void init_data()
{
  TILES[T_null]     = Tile_datum(c_black,   c_black,  false, false, 0, 0, 0,
                                 "", "NULL");
  TILES[T_empty]    = Tile_datum(c_dkgray,  c_black,  false, false, 0, 0, 0,
                                 ".", "empty");
  TILES[T_sky]      = Tile_datum(c_cyan,    c_cyan,   false, false, 0, 0, 0,
                                 "", "sky");
  TILES[T_grass]    = Tile_datum(c_green,   c_cyan,   false, false, 0, 0, 0,
                                 "_", "grass");
  TILES[T_shop]     = Tile_datum(c_green,   c_black,  false, false, 0, 0, 0,
                                 "$", "shop");
  TILES[T_dirt]     = Tile_datum(c_ltgray,  c_brown,  true,  false, 5, 0, 0,
                                 "   `~,.'^", "dirt");
  TILES[T_boulder]  = Tile_datum(c_ltgray,  c_black,  true,  false, 0, 0, 0,
                                 "0", "boulder");
  TILES[T_ladder]   = Tile_datum(c_ltred,   c_black,  false, false, 0, 1, 0,
                                 "H", "ladder");
  TILES[T_support]  = Tile_datum(c_ltred,   c_black,  false, true,  0, 0, 0,
                                 "|", "support");
  TILES[T_ladder_and_support] =
                      Tile_datum(c_ltred,   c_black,  false, true,  0, 1, 0,
                                 "#", "support");
  TILES[T_entrance] = Tile_datum(c_black,   c_cyan,   false, false, 0, 0, 0,
                                 "M", "entrance");

  TILES[T_coal]     = Tile_datum(c_black,   c_brown,  true,  true,  8, 0, 5,
                                 "#", "coal");

  COST[S_auger]     = 500;
  AMT [S_auger]     =   1;

  COST[S_ladders]   =   5;
  AMT [S_ladders]   =   1;

  COST[S_rope]      =   5;
  AMT [S_rope]      =   1;

  COST[S_supports]  =   7;
  AMT [S_supports]  =   1;

}
