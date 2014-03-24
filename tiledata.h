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
  T_ladder_and_support,
  T_entrance,

  T_coal,
  T_max
};

struct Tile_datum
{
  nc_color fg, bg;
  bool blocks;
  bool support;
  int dig;
  int climb_cost;
  int value;
  std::string symbols;
  std::string name;

  bool valuable() { return value > 0; }

  Tile_datum()
  {
    fg = c_black;
    bg = c_black;
    blocks   = false;
    support  = false;
    dig = 0;
    climb_cost = 0;
    value = 0;
    symbols = "";
  }

  Tile_datum(nc_color FG, nc_color BG, bool B, bool S, int D, int CC, int V,
             std::string Sym, std::string Name) :
    fg (FG), bg (BG),
    blocks (B), support (S),
    dig (D), climb_cost (CC), value (V),
    symbols (Sym), name (Name)
    {}

};

#endif
