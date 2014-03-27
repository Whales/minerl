#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "tiledata.h"
#include "glyph.h"
#include "geometry.h"
#include <vector>
#include <cstdarg>  // For variadic constructor

class Map;

enum Monster_id
{
  M_null = 0,
  M_worm,
  M_cockroach,
  M_termite,
  M_max
};

struct Monster_type
{
  int hp;
  int dig;
  int dig_delay;
  int damage;
  int awareness;
  bool tools;
  bool climb;
  bool cling;
  bool fly;
  glyph sym;
  std::vector<Tile_id> targets;

  Monster_type()
  {
    hp = 1;
    dig = 1;
    dig_delay = 1;
    damage = 1;
  }

  Monster_type(int H, int D, int DD, int DAM, int A,
               bool T, bool C, bool G, bool F,
               char S, nc_color FG, ...) :
    hp (H), dig (D), dig_delay (DD), damage (DAM), awareness (A),
    tools (T), climb (C), cling (G), fly (F)
  {
    sym = glyph(S, FG, c_black);
    va_list ap;
    va_start(ap, FG);
    Tile_id tmp;
    while ((tmp = (Tile_id)(va_arg(ap, int)))) {
      targets.push_back(tmp);
    }
    va_end(ap);
  }

};

struct Monster
{
  Monster();

  Monster_id id;
  Monster_type* type;
  int hp;
  int rest;
  int posx;
  int posy;

  std::vector<Point> path;

  void follow_path(Map* map);
  void move(Map* map, int movex, int movey);
// Returns true if we fell
  bool fall_if_needed(Map* map);
  void path_to_target(Map* map);

  void set_type(Monster_id mid, Monster_type* mtype = NULL);
private:
  void path_to_random_target(Map* map);
};

#endif
