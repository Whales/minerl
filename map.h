#ifndef _MAP_H_
#define _MAP_H_

#include "tiledata.h"
#include "window.h"
#include <vector>

struct Tile
{
  Tile_id id;
  glyph sym;

  void set_id(Tile_id new_id);
};

class Map
{
public:

  Map();
  ~Map();

  void generate();

  Tile get_tile(int x, int y);
  Tile_id get_tile_id(int x, int y);
  void draw(Window* w, int cx, int cy);

  void set_tile(int x, int y, Tile_id tile);

private:
  std::vector< std::vector<Tile> > tiles;
  Tile null_tile;
};

  
#endif
