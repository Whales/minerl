#include "map.h"
#include "rng.h"
#include "globals.h"
#include "player.h"
#include "geometry.h"
#include <vector>
#include <fstream>  // For Map::path() debug output

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
  int x_max = x_min + xsize - 1;
  int y_max = y_min + ysize - 1;

  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      int x_ter = x + x_min;
      int y_ter = y + y_min;
      if (x_ter == pl->posx && y_ter == pl->posy) {
        w->putglyph(x, y, glyph('@', c_white, c_black));
      } else {
        Tile* tile = get_tile(x_ter, y_ter);
        if (tile->seen) {
          w->putglyph(x, y, tile->sym);
        } else {
          w->putglyph(x, y, glyph('x', c_black, c_black));
        }
      }
    }
  }
// Now, draw monsters.
  for (int i = 0; i < monsters.size(); i++) {
    if (monsters[i].posx >= x_min && monsters[i].posx <= x_max &&
        monsters[i].posy >= y_min && monsters[i].posy <= y_max   ) {
      int x = monsters[i].posx - x_min, y = monsters[i].posy - y_min;
      w->putglyph(x, y, monsters[i].type->sym);
    }
  }
// Finally, draw the message, if any...
  while (!messages.empty()) {
    w->putstr(1, 0, c_yellow, c_black, messages[0]);
    messages.erase(messages.begin());
    if (!messages.empty()) {
      w->putstr(1, 1, c_black, c_red, "More... (Press Spacebar)");
    }
    long ch;
    while (ch != ' ') {
      ch = input();
    }
  }
}

void Map::set_tile(int x, int y, Tile_id tile)
{
  Tile* tileptr = get_tile(x, y);
  tileptr->set_id(tile);
}

void Map::add_ladder(int x, int y)
{
  if (get_tile_id(x, y) == T_support) {
    set_tile(x, y, T_ladder_and_support);
  } else {
    set_tile(x, y, T_ladder);
  }
}

void Map::add_support(int x, int y)
{
  if (get_tile_id(x, y) == T_ladder) {
    set_tile(x, y, T_ladder_and_support);
  } else {
    set_tile(x, y, T_support);
  }
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

void Map::add_msg(std::string message)
{
  messages.push_back(message);
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
  int num_monsters = rng(1, 2);
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
    if (mon->rest > 0) {
      //debugmsg("Rest");
      mon->rest--;
    } else {
      int dist = rl_dist(mon->posx, mon->posy, pl.posx, pl.posy);
      if (pl.posy > 12 && dist <= 1) {
// TODO: Display a message.
        pl.take_damage( mon->type->damage );
      } else if (pl.posy > 12 && dist <= mon->type->awareness) {
        mon->path = path(mon->id, mon->posx, mon->posy, pl.posx, pl.posy);
        if (mon->path.empty()) {  // Couldn't find a route to player
          mon->path_to_target(this);  // ... so path to something else.
        }
        mon->follow_path(this);
      } else if (!mon->path.empty()) {
        mon->follow_path(this);
      } else {  // Nowhere near player; find something to eat?
        mon->path_to_target(this);
        mon->follow_path(this);
      }
    }
  }
}

enum A_star_status
{
  AS_none,
  AS_open,
  AS_closed
};
  

std::vector<Point> Map::path(Monster_id type,
                             int origx, int origy, int destx, int desty)
{
  if (origx == destx && origy == desty) {
    return std::vector<Point>();  // No pathing to do!
  }
  //debugmsg("Pathing [%d:%d] => [%d:%d]", origx, origy, destx, desty);
  bool tools = false, climb = false, cling = false, fly = false;
  if (type > M_null) {
    Monster_type* mtype = &(MONSTERS[type]);
    tools = mtype->tools;
    climb = mtype->climb;
    cling = mtype->cling;
    fly   = mtype->fly;
  }

  int min_x = (origx < destx ? origx - 3 : destx - 3);
  int max_x = (origx > destx ? origx + 3 : destx + 3);
  int min_y = (origy < desty ? origy - 3 : desty - 3);
  int max_y = (origy > desty ? origy + 3 : desty + 3);

  if (min_x < 0) {
    min_x = 0;
  }
  if (max_x > 119) {
    max_x = 119;
  }
  if (min_y < 13) {
    min_y = 13;
  }
  if (max_y > 119) {
    max_y = 119;
  }

  int x_size = max_x - min_x + 1;
  int y_size = max_y - min_y + 1;

  origx -= min_x;
  origy -= min_y;
  destx -= min_x;
  desty -= min_y;

/*
  debugmsg("Prepped: bounds [%d:%d]/[%d:%d], path [%d:%d] => [%d:%d]",
           min_x, min_y, max_x, max_y, origx, origy, destx, desty);
*/

  std::vector<Point> open_points;
  A_star_status status[x_size][y_size];
  int           gscore[x_size][y_size];
  int           hscore[x_size][y_size];
  Point         parent[x_size][y_size];

// Init everything to 0
  for (int x = 0; x < x_size; x++) {
    for (int y = 0; y < y_size; y++) {
      status[x][y] = AS_none;
      gscore[x][y] = 0;
      hscore[x][y] = 0;
      parent[x][y] = Point(-1, -1);
    }
  }

  status[origx][origy] = AS_open;
  open_points.push_back( Point(origx, origy) );
  bool done = false;

  while (!done && !open_points.empty()) {
// 1) Find the lowest cost in open_points, and set (current) to that point
    int lowest_cost = -1, point_index = -1;
    Point current;
    int current_g = 0;
    for (int i = 0; i < open_points.size(); i++) {
      Point p = open_points[i];
      int score = gscore[p.x][p.y] + hscore[p.x][p.y];
      if (i == 0 || score < lowest_cost) {
        lowest_cost = score;
        current = p;
        current_g = gscore[p.x][p.y];
        point_index = i;
      }
    }
// 2) Check if (current) is the endpoint
    if (current.x == destx && current.y == desty) {
      done = true;
    } else {
// 3) Set (current) to be closed
      open_points.erase(open_points.begin() + point_index);
      status[current.x][current.y] = AS_closed;
// 4) Examine all adjacent points
//  4a) We can go up if we fly, we use tools, or we climb & there's something to
//      climb on.
      Tile_datum* here    = get_tile_data(current.x    , current.y    );
      //Tile_datum* U_data  = get_tile_data(current.x    , current.y - 1);
      Tile_datum* UL_data = get_tile_data(current.x - 1, current.y - 1);
      Tile_datum* UR_data = get_tile_data(current.x + 1, current.y - 1);
      bool go_up = (fly || tools);
      if (!go_up) {
        if (climb && here->climb_cost > 0) {
          go_up = true;
        } else if (cling && (UL_data->blocks || UR_data->blocks)) {
          go_up = true;
        }
      }
// Cycle through all four adjacent points.
      for (int i = 1; i <= 4; i++) {
        int nx = current.x, ny = current.y;
        switch (i) {
          case 1: nx--; break;
          case 2: nx++; break;
          case 3: ny--; break;
          case 4: ny++; break;
        }
// Ensure we're inbounds
        if (nx >= 0 && ny >= 0 && nx < x_size && ny < y_size) {
          int cost = pathing_cost(type, current.x, current.y, nx, ny);
// Only check the spot if we can go up, or if it's not an upwards move.
          if (cost > 0 && (ny >= current.y || go_up)) {
            int ng = current_g + cost;
// If it's unexamined, make it open and set its values.
            if (status[nx][ny] == AS_none) {
              status[nx][ny] = AS_open;
              gscore[nx][ny] = ng;
              hscore[nx][ny] = manhattan_dist(nx, ny, destx, desty);
              parent[nx][ny] = current;
              open_points.push_back( Point(nx, ny) );
// Otherwise, if it's open and we're a better parent, make us the parent
            } else if (status[nx][ny] == AS_open && ng < gscore[nx][ny]) {
              gscore[nx][ny] = ng;
              parent[nx][ny] = current;
            }
          }
        } // end of "nx & ny are inbounds" if block
      } // end of for loop over adjacent tiles
    } // end of "this isn't the end tile" block
  } // while (!done && !open_points.empty())

  std::vector<Point> ret;
  if (!done) {
    return ret; // Couldn't find a path!
  }

// Above, we subtraced min_x from origx, etc.  So we have to add it back in now.
  Point cur(destx, desty);
  ret.push_back( Point(cur.x + min_x, cur.y + min_y) );
  while (parent[cur.x][cur.y] != Point(origx, origy)) {
    cur = parent[cur.x][cur.y];
    if (cur.x < 0 || cur.y < 0 || cur.x >= x_size || cur.y >= y_size) {
      std::ofstream fout;
      fout.open("debug.txt", std::fstream::trunc);
      for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
          fout << parent[x][y].str() << "   ";
        }
        fout << std::endl;
      }
      fout.close();
      debugmsg("Cur %s", cur.str().c_str());
    }
    ret.push_back( Point(cur.x + min_x, cur.y + min_y) );
  }
// Now, reverse the list (oy vey)
  std::vector<Point> real_ret;
  for (int i = ret.size() - 1; i >= 0; i--) {
    real_ret.push_back(ret[i]);
  }

  return real_ret;
}

int Map::pathing_cost(Monster_id type, int curx, int cury, int x, int y)
{
  int dig = 1, dig_delay = 1;
  bool tools = false, climb = false, cling = false, fly = false;
  if (type > M_null) {
    Monster_type* mtype = &(MONSTERS[type]);
    dig       = mtype->dig;
    dig_delay = mtype->dig_delay;
    tools     = mtype->tools;
    climb     = mtype->climb;
    cling     = mtype->cling;
    fly       = mtype->fly;
  }
  Tile_datum* data = get_tile_data(x, y);
  Tile* tile = get_tile(x, y);
  int ret = 0;
  if (data->blocks) { // Start with time to dig
    if (dig > 0) {
      ret = (tile->hp / dig) * dig_delay;
    } else {
      return 0; // Can't get through it!
    }
  }
  if (y < cury) { // Gotta move up
    if (fly || climb || cling) {
// Don't check climbability here; pathfinder does that!
      ret++;  // Just takes a turn to fly up
    } else if (tools) {
      if (get_tile_data(curx, cury)->climb_cost > 0) {
        ret++;
      } else {
        ret += 2; // One extra turn to place a ladder!
      }
    } else {  // No way to move up!
      return 0;
    }
  } else {
    ret++;  // Non-vertical moves always take one turn.
  }
  return ret;
}
