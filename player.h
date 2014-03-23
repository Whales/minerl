#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "tiledata.h"
#include "cuss.h"
#include "map.h"
#include <vector>

enum Supplies
{
  S_null = 0,
  S_ladders,
  S_rope,
  S_supports,
  S_max
};

enum Equipment
{
  E_null = 0,
  E_stamina,
  E_backpack,
  E_pickaxe,
  E_sword,
  E_crossbow,
  E_lamp,
  E_max
};

struct Equipment_status
{
  int current;
  int level;
  int price;
  int price_increase;
  int upgrade_amount;

  Equipment_status()
  {
    current = 1;
    level = 1;
    price = 100;
    price_increase = 50;
    upgrade_amount = 1;
  }

  Equipment_status(int L, int P, int I, int U) :
    level (L), price (P), price_increase(I), upgrade_amount (U)
  { current = level; }

  void set_level(int lev)
  {
    level = lev;
    current = lev;
  }

  void reset()
  {
    current = level;
  }
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
  int cash;
// Stats / Equipment (Permanent tools, like your pickaxe)
  Equipment_status equipment[E_max];
// Supplies (tools which are used, like ladders)
  int supplies[S_max];
// Carried mined goods
  std::vector<Tile_id> findings;
};


#endif
