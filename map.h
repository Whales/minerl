#ifndef _MAP_H_
#define _MAP_H_

#include "tiledata.h"
#include "window.h"
#include "geometry.h"
#include <vector>

class Player;

struct Tile
{
  Tile_id id;
  glyph sym;
  int hp;
  bool seen;
  bool ready_to_fall;

  Tile();

  void set_id(Tile_id new_id);
  bool dig(int amount);  // Returns true if we were destroyed
};

class Map
{
public:

  Map();
  ~Map();

  void reset_null_tile();
  void generate();

  Tile*       get_tile     (int x, int y);
  Tile_id     get_tile_id  (int x, int y);
  Tile_datum* get_tile_data(int x, int y);
  void draw(Window* w, Player* pl);

  void set_tile(int x, int y, Tile_id tile);
  void add_falling(int x, int y);

  void process_falling(Player& pl);

private:
  std::vector< std::vector<Tile> > tiles;
  std::vector<Point> falling;
  Tile null_tile;
};

  
#endif
