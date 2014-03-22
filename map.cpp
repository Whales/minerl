#include "map.h"
#include "rng.h"
#include "globals.h"

void Tile::set_id(Tile_id new_id)
{
  id = new_id;
  Tile_datum* data = &(TILES[id]);
  sym.fg = data->fg;
  sym.bg = data->bg;
  if (data->symbols.empty()) {
    sym.symbol = ' ';
  } else {
    int index = rng(0, data->symbols.size() - 1);
    sym.symbol = data->symbols[index];
  }
}

Map::Map()
{
  null_tile.set_id(T_null);
}

Map::~Map()
{
}

void Map::generate()
{
  std::vector<Tile> tmp;

  for (int i = 0; i < 120; i++) {
    tmp.push_back(null_tile);
  }
  for (int i = 0; i < 120; i++) {
    tiles.push_back(tmp);
  }

  for (int x = 0; x < 120; x++) {
    for (int y = 0; y < 120; y++) {
      if (y < 12) {
        tiles[x][y].set_id(T_sky);
      } else if (y == 12) {
        tiles[x][y].set_id(T_grass);
      } else {
        tiles[x][y].set_id(T_dirt);
      }
    }
  }
}

Tile_id Map::get_tile_id(int x, int y)
{
  if (x < 0 || x >= tiles.size()) {
    return T_null;
  }
  if (y < 0 || y >= tiles[x].size()) {
    return T_null;
  }
  return tiles[x][y].id;
}

Tile Map::get_tile(int x, int y)
{
  if (x < 0 || x >= tiles.size()) {
    return null_tile;
  }
  if (y < 0 || y >= tiles[x].size()) {
    return null_tile;
  }
  return tiles[x][y];
}

void Map::draw(Window* w, int cx, int cy)
{
  int xsize = w->sizex(), ysize = w->sizey();

  int x_min = cx - xsize / 2;
  int y_min = cy - ysize / 2;

  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      int x_ter = x + x_min;
      int y_ter = y + y_min;
      Tile tile = get_tile(x_ter, y_ter);
      w->putglyph(x, y, tile.sym);
    }
  }
}
