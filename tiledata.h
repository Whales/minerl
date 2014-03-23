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
  T_shop,
  T_dirt,
  T_boulder,
  T_ladder,
  T_support,

  T_coal,
  T_max
};

struct Tile_datum
{
  nc_color fg, bg;
  bool blocks;
  bool valuable;
  bool support;
  int dig;
  int climb_cost;
  std::string symbols;

  Tile_datum()
  {
    fg = c_black;
    bg = c_black;
    blocks   = false;
    valuable = false;
    support  = false;
    dig = 0;
    symbols = "";
  }

  Tile_datum(nc_color FG, nc_color BG, bool B, bool V, bool S, int D, int CC,
             std::string Sym) :
    fg (FG), bg (BG),
    blocks (B), valuable (V), support (S),
    dig (D), climb_cost (CC),
    symbols (Sym)
    {}

};

#endif
