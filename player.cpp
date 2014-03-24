#include "player.h"

Player::Player()
{
  posy = 12;
  posx = 60;

  cash = 0;
  deepest_point = 12;

//                                         Lev Price %Icr Amount bought
  equipment[E_stamina]  = Equipment_status(200, 200,  50, 50);
  equipment[E_backpack] = Equipment_status( 15, 100,  50,  3);
  equipment[E_pickaxe]  = Equipment_status(  1, 100,  50,  1);
  equipment[E_sword]    = Equipment_status(  1, 100,  50,  1);
  equipment[E_crossbow] = Equipment_status(  1, 100,  50,  1);
  equipment[E_lamp]     = Equipment_status(  2, 300, 100,  1);
  
  max_stamina = equipment[E_stamina].level;

  for (int i = 0; i < S_max; i++) {
    supplies[i] = 0;
  }
  supplies[S_auger]    =  1;
  supplies[S_ladders]  = 20;
  supplies[S_supports] =  5;
}

Player::~Player()
{
}

void Player::update_hud(cuss::interface &i_hud)
{
  i_hud.set_data("num_stamina",     equipment[E_stamina].current);
  int stam_percent = equipment[E_stamina].get_percent();
  nc_color stam_color;
  if (stam_percent == 100) {
    stam_color = c_white;
  } else if (stam_percent >= 75) {
    stam_color = c_ltgray;
  } else if (stam_percent >= 50) {
    stam_color = c_ltgreen;
  } else if (stam_percent >= 20) {
    stam_color = c_yellow;
  } else {
    stam_color = c_ltred;
  }
  i_hud.set_data("num_stamina", stam_color);

  if (equipment[E_stamina].level > max_stamina) {
    i_hud.set_data("num_stamina_max", max_stamina);
    i_hud.set_data("num_stamina_max", c_ltred);
  } else {
    i_hud.set_data("num_stamina_max", equipment[E_stamina].level);
    i_hud.set_data("num_stamina_max", c_white);
  }

  i_hud.set_data("num_storage",     get_storage());
  if (get_storage() <= 3) {
    i_hud.set_data("num_storage", c_ltred);
  } else {
    i_hud.set_data("num_storage", c_ltgray);
  }

  i_hud.set_data("num_ladders",     supplies[S_ladders]);
  if (supplies[S_ladders] <= 3) {
    i_hud.set_data("num_storage", c_ltred);
  } else {
    i_hud.set_data("num_storage", c_ltgray);
  }

  i_hud.set_data("num_rope",        supplies[S_rope]);
  i_hud.set_data("num_supports",    supplies[S_supports]);
}

int Player::get_storage()
{
  return equipment[E_backpack].level - findings.size();
}

int Player::get_dig_power()
{
  return equipment[E_pickaxe].level;
}

int Player::get_lamp_radius()
{
  return equipment[E_lamp].level;
}

void Player::reset_values()
{
  equipment[E_stamina].reset();
  if (equipment[E_stamina].current > max_stamina) {
    equipment[E_stamina].current = max_stamina;
  }
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
      bool using_auger = false;
      if (posy == 12) { // We need an auger!
        if (supplies[S_auger] <= 0 || map->get_tile_id(posx, posy) == T_shop) {
// TODO: Add a message about needing an auger
          return false; // Can't dig without it
        }
        map->set_tile(posx, posy, T_entrance);
        supplies[S_auger]--;
        using_auger = true;
      }
      Tile* tile = map->get_tile(movex, movey);
      Tile_id finding_id = tile->id;
      if (use_stamina(1)) {
        int dig_power = get_dig_power();
        if (using_auger) {
          dig_power = 500;
        }
        if (tile->dig( dig_power )) {
          if (data->valuable()) {
            add_finding(finding_id);
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
    if (map->get_tile_id(movex, movey) == T_grass) {
      return false; // Can't move up to grass; need a mine entrance.
    }
    if (use_stamina(data_here->climb_cost)) {
      posx = movex;
      posy = movey;
      if (posy > deepest_point) {
        deepest_point = posy;
      }
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
  if (map->get_tile_id(posx, posy) == T_grass) {
    return false;
  }
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
  if (equipment[E_stamina].current >= amount) {
    equipment[E_stamina].current -= amount;
    return true;
  }
  return false;
}

bool Player::take_damage(int amount)
{
  max_stamina -= amount;
  if (equipment[E_stamina].current > max_stamina) {
    equipment[E_stamina].current = max_stamina;
  }
  if (max_stamina <= 0) {
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
