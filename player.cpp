#include "player.h"

Player::Player()
{
  posy = 12;
  posx = 60;
  backpack_size = 15;
  stamina_max = 500;
  stamina = 500;

  for (int i = 0; i < E_max; i++) {
    equipment[i] = 0;
  }
}

Player::~Player()
{
}

void Player::update_hud(cuss::interface &i_hud)
{
  i_hud.set_data("num_stamina",   stamina);
  i_hud.set_data("num_storage",   get_storage());
  i_hud.set_data("num_ladders",   equipment[E_ladders]);
  i_hud.set_data("num_rope",      equipment[E_rope]);
  i_hud.set_data("num_supports",  equipment[E_supports]);
}

int Player::get_storage()
{
  return backpack_size - findings.size();
}

void Player::move(Map* map, int movex, int movey)
{
}
