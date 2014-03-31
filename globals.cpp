#include "globals.h"

Tile_datum TILES[T_max];
int COST[S_max];
int AMT[S_max];
Monster_type MONSTERS[M_max];

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
  TILES[T_shop]     = Tile_datum(c_black,   c_blue,   false, false, 0, 0, 0,
                                 "=", "shop");
  TILES[T_dirt]     = Tile_datum(c_ltgray,  c_brown,  true,  false, 3, 0, 0,
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

  TILES[T_coal]     = Tile_datum(c_black,   c_brown,  true,  true,  5, 0,  5,
                                 "#", "coal");
  TILES[T_iron]     = Tile_datum(c_red,     c_brown,  true,  true,  8, 0, 10,
                                 "#", "iron");
  TILES[T_copper]   = Tile_datum(c_ltred,   c_brown,  true,  true, 10, 0, 14,
                                 "#", "copper");

  COST[S_auger]     = 500;
  AMT [S_auger]     =   1;

  COST[S_ladders]   =   5;
  AMT [S_ladders]   =   1;

  COST[S_rope]      =   5;
  AMT [S_rope]      =   1;

  COST[S_supports]  =   7;
  AMT [S_supports]  =   1;

  MONSTERS[M_worm]        =
    Monster_type(8, 1, 2, 1, 3, false, false, true, false, 'w', c_ltred,
                 0);

  MONSTERS[M_cockroach]   =
    Monster_type(3, 1, 5, 1, 6, false, true, true, false, 'c', c_brown,
                 0);

  MONSTERS[M_termite]     =
    Monster_type(3, 1, 5, 1, 6, false, true, true, false, 'c', c_red,
                 T_ladder, T_support, T_ladder_and_support, 0);

}
