#include "player.h"

Player::Player()
{
  posy = 12;
  posx = 60;
  backpack_size = 15;
  stamina_max = 500;
  stamina = 500;

  pickaxe_level   = 1;
  sword_level     = 1;
  crossbow_level  = 1;
  lamp_level      = 2;

  for (int i = 0; i < E_max; i++) {
    equipment[i] = 0;
  }
  equipment[E_ladders]  = 20;
  equipment[E_supports] = 5;
}

Player::~Player()
{
}

void Player::update_hud(cuss::interface &i_hud)
{
  i_hud.set_data("num_stamina",     stamina);
  i_hud.set_data("num_stamina_max", stamina_max);
  i_hud.set_data("num_storage",     get_storage());
  i_hud.set_data("num_ladders",     equipment[E_ladders]);
  i_hud.set_data("num_rope",        equipment[E_rope]);
  i_hud.set_data("num_supports",    equipment[E_supports]);
}

int Player::get_storage()
{
  return backpack_size - findings.size();
}

int Player::get_dig_power()
{
  return pickaxe_level;
}

int Player::get_lamp_radius()
{
  return lamp_level;
}

void Player::reset_values()
{
  stamina = stamina_max;
}

bool Player::move(Map* map, int movex, int movey)
{
  if (movex == posx && movey == posy) {
    return true;  // We paused.
  }
  if (!map) {
    return false;
  }
  Tile_datum* data      = map->get_tile_data(movex, movey);
  Tile_datum* data_here = map->get_tile_data(posx, posy);
// Can't move there; maybe dig?
  if (data->blocks) {
    if (data->dig > 0) {
      Tile* tile = map->get_tile(movex, movey);
      if (use_stamina(1)) {
        if (tile->dig( get_dig_power() )) {
          if (data->valuable) {
            add_finding(tile->id);
          }
          int bouldery = movey - 1;
          while (map->get_tile_id(movex, bouldery) == T_boulder) {
            map->add_falling(movex, bouldery);
            bouldery--;
          }
        }
        fall_if_needed(map);
        return true;
      }
    }
    return false;
  }

  if (movey < posy) {
    if (data_here->climb_cost == 0) {
      return false; // We can't climb
    }
    if (use_stamina(data_here->climb_cost)) {
      posx = movex;
      posy = movey;
      return true;
    }
    return false;
  }

// Otherwise - it's just a normal move!
  if (use_stamina(1)) {
    posx = movex;
    posy = movey;
    fall_if_needed(map);
    return true;
  }
  return false;
}

bool Player::fall_if_needed(Map* map)
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

bool Player::use_stamina(int amount)
{
  if (stamina >= amount) {
    stamina -= amount;
    return true;
  }
  return false;
}

void Player::add_finding(Tile_id id)
{
  if (get_storage() <= 0) {
    return; // No space for it
  }
  findings.push_back(id);
}
