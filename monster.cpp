#include "monster.h"
#include "map.h"
#include "geometry.h"
#include "globals.h"
#include "rng.h"
#include <vector>

Monster::Monster()
{
  hp = 1;
  rest = 0;
  posx = 0;
  posy = 0;
  type = NULL;
  id = M_null;
}

void Monster::set_type(Monster_id mid, Monster_type* mtype)
{
  id = mid;
  if (mtype) {
    type = mtype;
  } else {
    type = &(MONSTERS[id]);
  }
  hp = type->hp;
}

void Monster::follow_path(Map* map)
{
  if (path.empty()) {
    return;
  }

// Make sure the next step in the path is adjacent to us.
  if (rl_dist(posx, posy, path[0].x, path[0].y) > 1) {
    std::vector<Point> tmp = map->path(type, posx, posy, path[0].x, path[0].y);
    for (int i = tmp.size() - 1; i >= 0; i--) {
      path.insert( path.begin(), tmp[i] );
    }
  }

// Make sure the next step in the path is adjacent to us... again.
  if (rl_dist(posx, posy, path[0].x, path[0].y) > 1) {
    debugmsg("Monster really couldn't path!");
    path.clear();
    return;
  }

  move(map, path[0].x, path[0].y);
  if (posx == path[0].x && posy == path[0].y) {
    path.erase( path.begin() );
  }
}

void Monster::move(Map* map, int movex, int movey)
{
  if (posx == movex && posy == movey) {
    return;
  }
  if (!map) {
    return;
  }

  Tile_datum* data      = map->get_tile_data(movex, movey);
  Tile_datum* data_here = map->get_tile_data(posx, posy);
// Can't move there; maybe dig?
  if (data->blocks) {
    if (data->dig > 0) {
      Tile* tile = map->get_tile(movex, movey);
      int dig_power = type->dig;
      if (tile->dig( dig_power )) {
        int bouldery = movey - 1;
        while (map->get_tile_id(movex, bouldery) == T_boulder) {
          map->add_falling(movex, bouldery);
          bouldery--;
        }
      }
      fall_if_needed(map);
      rest = type->dig_delay - 1; // Some monsters take several turns to dig
      return;
    }
    return;
  }
  if (movey < posy) { // Moving upwards!
    if (type->climb || type->fly || data_here->climb_cost > 0) {
      posx = movex;
      posy = movey;
    } else if (type->tools) {
      map->set_tile(posx, posy, T_ladder);
    }
  } else {
    posx = movex;
    posy = movey;
  }
  fall_if_needed(map);
}

bool Monster::fall_if_needed(Map* map)
{
  Tile_datum* below = map->get_tile_data(posx, posy + 1);
  if (below->blocks || below->climb_cost > 0) {
    return false;
  }
// TODO: Falling damage
  move(map, posx, posy + 1);
  fall_if_needed(map);
  return true;
}

void Monster::path_to_target(Map* map)
{
  if (!map) {
    return;
  }
  if (type->targets.empty()) {  // We don't like to gather anything!
    path_to_random_target(map);
    return;
  }
  std::vector<Point> targets;
  for (int x = posx - type->awareness; x <= posx + type->awareness; x++) {
    for (int y = posy - type->awareness; y <= posy + type->awareness; y++) {
      bool found = false;
      Tile_id tile = map->get_tile_id(x, y);
      for (int i = 0; !found && i < type->targets.size(); i++) {
        if (tile == type->targets[i]) {
          targets.push_back( Point(x, y) );
          found = true;
        }
      }
    }
  }
  if (targets.empty()) {  // Couldn't find anything to path to!
    path_to_random_target(map);
    return;
  }
  Point tar = targets[ rng(0, targets.size() - 1) ];
  path = map->path(type, posx, posy, tar.x, tar.y);
}

void Monster::path_to_random_target(Map* map)
{
  if (!map) {
    return;
  }
  int minx = posx - 5;
  if (minx < 0) {
    minx = 0;
  }
  int maxx = posx + 5;
  if (maxx > 119) {
    maxx = 119;
  }
  int miny = posy - 5;
  if (miny < 13) {
    miny = 13;
  }
  int maxy = posy + 5;
  if (maxy > 119) {
    maxy = 119;
  }
  path = map->path(type, posx, posy, rng(minx, maxx), rng(miny, maxy));
}