#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "tiledata.h"
#include "cuss.h"
#include "map.h"
#include <vector>

enum Equipment
{
  E_null = 0,
  E_ladders,
  E_rope,
  E_supports,
  E_max
};

class Player
{
public:
  Player();
  ~Player();

  void update_hud(cuss::interface &i_hud);
  int get_storage();

  void move(Map* map, int movex, int movey);

  int posx, posy;
  int stamina;
  std::vector<Tile_id> findings;

// "Permanent" values
  int stamina_max;
  int backpack_size;

  int equipment[E_max];
};


#endif
