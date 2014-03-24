#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "tiledata.h"
#include "glyph.h"
#include <vector>

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
  glyph sym;
  std::vector<Tile_id> targets;

  Monster_type()
  {
    hp = 1;
    dig = 1;
    dig_delay = 1;
    damage = 1;
  }

  Monster_type(int H, int D, int DD, int DAM, char S, nc_color FG, ...) :
    hp (H), dig (D), dig_delay (DD), damage (DAM)
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
  Monster_id id;
  Monster_type* type;
  int hp;
  int posx;
  int posy;

  void set_type(Monster_id mid, Monster_type* mtype)
  {
    id = mid;
    type = mtype;
    hp = type->hp;
  }
};

#endif
