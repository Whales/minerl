#include "map.h"
#include "rng.h"
#include "globals.h"
#include "player.h"
#include "geometry.h"

Tile::Tile()
{
  id = T_null;
  sym = glyph('x', c_red, c_white);
  hp = 0;
  seen = false;
  ready_to_fall = false;
}

void Tile::set_id(Tile_id new_id)
{
  id = new_id;
  Tile_datum* data = &(TILES[id]);
  sym.fg = data->fg;
  sym.bg = data->bg;
  ready_to_fall = false;
  if (data->symbols.empty()) {
    sym.symbol = ' ';
  } else {
    int index = rng(0, data->symbols.size() - 1);
    sym.symbol = data->symbols[index];
  }
  if (data->dig > 0) {
    hp = data->dig;
  }
}

bool Tile::dig(int amount)
{
  Tile_datum* data = &(TILES[id]);
  if (data->dig == 0) { // We can't be digged!
    return false;
  }
  hp -= amount;
  if (hp <= 0) {
    set_id(T_empty);
    return true;
  }
// Set symbol to indicate remaining HP; TODO: No?
  if (!data->valuable()) {
    if (hp >= 10) {
      sym.symbol = 'A' + (hp - 10);
    } else {
      sym.symbol = '0' + hp;
    }
  }
  int percent = (100 * hp) / data->dig;
  if (percent >= 80) {
    sym.fg = c_ltgray;
  } else if (percent >= 60) {
    sym.fg = c_green;
  } else if (percent >= 30) {
    sym.fg = c_yellow;
  } else {
    sym.fg = c_red;
  }
  return false;
}


Map::Map()
{
  reset_null_tile();
}

Map::~Map()
{
}

void Map::reset_null_tile()
{
  null_tile.set_id(T_null);
}

void Map::generate()
{
  reset_null_tile();
  std::vector<Tile> tmp;

  for (int i = 0; i < 120; i++) {
    tmp.push_back(null_tile);
  }
  for (int i = 0; i < 120; i++) {
    tiles.push_back(tmp);
  }

// Set the base terrain; sky, grass, and dirt.
  for (int x = 0; x < 120; x++) {
    for (int y = 0; y < 120; y++) {
      if (y < 12) {
        tiles[x][y].set_id(T_sky);
        tiles[x][y].seen = true;
      } else if (y == 12) {
        tiles[x][y].set_id(T_grass);
        tiles[x][y].seen = true;
      } else {
        if (y == 13 || y == 14) {
          tiles[x][y].seen = true;
        }
        if (one_in(10)) {
          tiles[x][y].set_id(T_boulder);
        } else {
          tiles[x][y].set_id(T_dirt);
        }
      }
    }
  }

// We need a shop, of course...
  tiles[65][12].set_id(T_shop);

// Now place some veins.
  int coal_veins = 5 * rng(10, 15);
  for (int i = 0; i < coal_veins; i++) {
    int x = rng(10, 110);
    int y = rng(13, 25);
    add_vein(T_coal, x, y, rng(4, 8));
  }

  int empty_veins = 5 * rng(20, 30);
  int empty_y = 18;
  for (int i = 0; i < empty_veins; i++) {
    int x = rng(10, 110);
    if (one_in(5)) {
      empty_y += rng(1, 3);
    }
    add_vein(T_empty, x, empty_y, rng(3, 12));
  }

}

void Map::add_vein(Tile_id type, int x, int y, int size)
{
  set_tile(x, y, type);
  for (int i = 0; i < size; i++) {
    int move = (y <= 15 ? rng(1, 3) : rng(1, 4));
    switch (move) {
      case 1: x++;  break;
      case 2: x--;  break;
      case 3: y++;  break;
      case 4: y--;  break;
    }
    Tile_id id = get_tile_id(x, y);
    if (id == T_dirt) {
      set_tile(x, y, type);
    } else if (id != T_boulder && !one_in(4)) {
      i--;  // 75% of the time, don't use up part of our size
    }
  }
}

Tile* Map::get_tile(int x, int y)
{
  if (x < 0 || x >= tiles.size()) {
    reset_null_tile();
    return &null_tile;
  }
  if (y < 0 || y >= tiles[x].size()) {
    reset_null_tile();
    return &null_tile;
  }
  return &(tiles[x][y]);
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

Tile_datum* Map::get_tile_data(int x, int y)
{
  return &( TILES[ get_tile_id(x, y) ] );
}

void Map::draw(Window* w, Player* pl)
{
  int xsize = w->sizex(), ysize = w->sizey();

  int x_min = pl->posx - xsize / 2;
  int y_min = pl->posy - ysize / 2;

  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      int x_ter = x + x_min;
      int y_ter = y + y_min;
      if (x_ter == pl->posx && y_ter == pl->posy) {
        w->putglyph(x, y, glyph('@', c_white, c_black));
      } else {
        bool found_monster = false;
        for (int i = 0; i < monsters.size() && !found_monster; i++) {
          if (monsters[i].posx == x_ter && monsters[i].posy == y_ter) {
            found_monster = true;
            w->putglyph(x, y, monsters[i].type->sym);
          }
        }
        if (!found_monster) {
          Tile* tile = get_tile(x_ter, y_ter);
          if (tile->seen) {
            w->putglyph(x, y, tile->sym);
          } else {
            w->putglyph(x, y, glyph('x', c_black, c_black));
          }
        }
      }
    }
  }
}

void Map::set_tile(int x, int y, Tile_id tile)
{
  Tile* tileptr = get_tile(x, y);
  tileptr->set_id(tile);
}

void Map::add_falling(int x, int y)
{
  if (get_tile_id(x, y) != T_boulder) {
debugmsg("Not a boulder");
    return; // Oops
  }
  for (int i = 0; i < falling.size(); i++) {
    if (falling[i].x == x && falling[i].y == y) {
debugmsg("Already falling");
      return; // It's already falling
    }
  }
  //debugmsg("Adding %d:%d", x, y);
  falling.push_back( Point(x, y) );
}

void Map::process_falling(Player& pl)
{
  std::vector<Point> post;
  for (int i = 0; i < falling.size(); i++) {
    Tile* tile = get_tile(falling[i].x, falling[i].y);
    //debugmsg("Processing %d:%d", falling[i].x, falling[i].y);
    if (tile->ready_to_fall) {
      int y = falling[i].y + 1;
      bool player_blocked = false;
      Tile_datum* data = get_tile_data(falling[i].x, y);
      set_tile(falling[i].x, falling[i].y, T_empty);
      if (falling[i].x == pl.posx && y == pl.posy) {
        player_blocked = true;
      }
      while (!data->blocks && !data->support &&
             (falling[i].x != pl.posx || y != pl.posy)) {
        y++;
        data = get_tile_data(falling[i].x, y);
      }
      if (falling[i].x == pl.posx && y == pl.posy) {
        player_blocked = true;
      }
      y--;  // We stop BEFORE the first one that supports a boulder
      set_tile(falling[i].x, y, T_boulder);
      if (player_blocked) {
        post.push_back(Point(falling[i].x, y));
      }
      falling.erase(falling.begin() + i);
      i--;
    } else {
      tile->ready_to_fall = true;
    }
  }

  for (int i = 0; i < post.size(); i++) {
    add_falling(post[i].x, post[i].y);
  }
}

void Map::spawn_monsters(Player &pl)
{
  std::vector<Point> valid_spawns;
  int num_monsters = rng(2, 6);
  for (int x = 0; x < 120; x++) {
    for (int y = 15; y < pl.deepest_point; y++) {
      if (get_tile_id(x, y) == T_empty) {
        valid_spawns.push_back( Point(x, y) );
      }
    }
  }

  for (int i = 0; !valid_spawns.empty() && i < num_monsters; i++) {
    Monster tmp;
    Monster_id mid = Monster_id( rng(M_null + 1, M_max - 1) );
    tmp.set_type( mid, &(MONSTERS[mid]) );
    int index = rng(0, valid_spawns.size() - 1);
    tmp.posx = valid_spawns[index].x;
    tmp.posy = valid_spawns[index].y;
    monsters.push_back(tmp);
  }
}

void Map::move_monsters(Player &pl)
{
  for (int i = 0; i < monsters.size(); i++) {
// If we're adjacent to the player, attack
    Monster* mon = &(monsters[i]);
    if (rl_dist(mon->posx, mon->posy, pl.posx, pl.posy) <= 1) {
// TODO: Display a message.
      pl.take_damage( mon->type->damage );
    }
  }
}
