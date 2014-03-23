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
  int get_dig_power();
  int get_lamp_radius();

  void reset_values();
// Returns true if we actually used our turn
  bool move(Map* map, int movex, int movey);
// Returns true if we fell
  bool fall_if_needed(Map* map);
// Returns true if we had enough
  bool use_stamina(int amount);
  void add_finding(Tile_id id);

  int posx, posy;
  int stamina;

// "Permanent" values
// Stats
  int stamina_max;

// Equipment
  int backpack_size;
  int pickaxe_level;
  int sword_level;
  int crossbow_level;
  int lamp_level;

// Carried equipment
  int equipment[E_max];
// Carried mined goods
  std::vector<Tile_id> findings;
};


#endif
