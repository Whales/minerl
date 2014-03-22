#ifndef _TILEDATA_H_
#define _TILEDATA_H_

#include "color.h"
#include <string>

enum Tile_id
{
  T_null = 0,
  T_empty,
  T_sky,
  T_grass,
  T_dirt,
  T_boulder,
  T_ladder,
  T_max
};

struct Tile_datum
{
  nc_color fg, bg;
  bool blocks;
  int dig;
  std::string symbols;

  Tile_datum()
  {
    fg = c_black;
    bg = c_black;
    blocks = false;
    dig = 0;
    symbols = "";
  }

  Tile_datum(nc_color FG, nc_color BG, bool B, int D, std::string S) :
    fg (FG), bg (BG), blocks (B), dig (D), symbols (S) {}

};

#endif
