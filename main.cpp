#include "window.h"
#include "globals.h"
#include "map.h"
#include "player.h"

void sell_findings(Player &pl);

void shop_upgrades(Player &pl);
void update_upgrade_ui(cuss::interface &i_shop, Player &pl);

void shop_supplies(Player &pl);
void update_supply_ui(cuss::interface &i_shop, Player &pl);

int main()
{
  init_display();

  init_data();

  Map map;
  map.generate();
  Player player;

  Window w_main(0, 0, 60, 24);
  Window w_hud(60, 0, 20, 24);

  cuss::interface i_hud;
  if (!i_hud.load_from_file("cuss/i_hud.cuss")) {
    return 1;
  }

  char ch;
  while (ch != 'q') {
    map.draw(&w_main, &player);
    player.update_hud(i_hud);
    i_hud.draw(&w_hud);
    w_main.refresh();
    w_hud.refresh();
    ch = input();
    int newx = player.posx, newy = player.posy;
    bool player_took_turn = false;
    if (ch == KEY_LEFT  || ch == 'h')  newx--;
    if (ch == KEY_RIGHT || ch == 'l')  newx++;
    if (ch == KEY_UP    || ch == 'k')  newy--;
    if (ch == KEY_DOWN  || ch == 'j')  newy++;

    if (player.posy > 12) { // Can only use items underground
      if (ch == 'w' && map.get_tile_id(player.posx, player.posy) != T_ladder) {
        if (player.supplies[S_ladders] > 0) {
          player.supplies[S_ladders]--;
          map.set_tile(player.posx, player.posy, T_ladder);
          player_took_turn = true;
        }
      } else if (ch == 's' &&
                 map.get_tile_id(player.posx, player.posy) != T_support) {
        if (player.supplies[S_supports] > 0) {
          player.supplies[S_supports]--;
          map.set_tile(player.posx, player.posy, T_support);
          player_took_turn = true;
        }
      }
    }

    if (ch == 'a') {
      if (player.posx == 65 && player.posy == 12) {
        sell_findings(player);
        shop_upgrades(player);
      }
    }

    if (newx != player.posx || newy != player.posy) {
      if (player.move(&map, newx, newy)) {
        player_took_turn = true;
        int rad = player.get_lamp_radius();
        for (int x = player.posx - rad; x <= player.posx + rad; x++) {
          for (int y = player.posy - rad; y <= player.posy + rad; y++) {
            map.get_tile(x, y)->seen = true;
          }
        }
      }
    } else if (ch == '.') {
      player_took_turn = true;
    }

    if (player.posy <= 12) {
      player.reset_values();
    }

    if (player_took_turn) {
// TODO: Move monsters, boulders, etc
      map.process_falling(player);
    }
  }

  end_display();
  return 0;
}

void sell_findings(Player &pl)
{
  for (int i = 0; i < pl.findings.size(); i++) {
    pl.cash += TILES[pl.findings[i]].value;
  }
  pl.findings.clear();
}

void shop_upgrades(Player &pl)
{
  Window w_shop(0, 0, 80, 24);
  cuss::interface i_shop;
  i_shop.load_from_file("cuss/i_upgrade.cuss");

  update_upgrade_ui(i_shop, pl);

  while (true) {
    i_shop.draw(&w_shop);
    w_shop.refresh();
    long ch = input();

    if ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
      Equipment bought;
      if (ch >= 'a' && ch <= 'f') {
        bought = Equipment( ch - 'a' + 1 );
      }
      if (ch >= 'A' && ch <= 'F') {
        bought = Equipment( ch - 'A' + 1 );
      }
      Equipment_status* stat = &(pl.equipment[bought]);
      if (pl.cash >= stat->price) {
        pl.cash -= stat->price;
        stat->set_level( stat->level + stat->upgrade_amount );
        stat->price += (stat->price * stat->price_increase) / 100;
        update_upgrade_ui(i_shop, pl);
      }
    } else if (ch == 'q' || ch == 'Q') {
      return;
    } else if (ch == 's' || ch == 'S') {
      shop_supplies(pl);
      return;
    }
  }
}

void update_upgrade_ui(cuss::interface &i_shop, Player &pl)
{
  i_shop.set_data("num_cash",         pl.cash);

  i_shop.set_data("num_cur_stamina",  pl.equipment[E_stamina].level );
  i_shop.set_data("num_cur_backpack", pl.equipment[E_backpack].level);
  i_shop.set_data("num_cur_pickaxe",  pl.equipment[E_pickaxe].level );
  i_shop.set_data("num_cur_sword",    pl.equipment[E_sword].level   );
  i_shop.set_data("num_cur_crossbow", pl.equipment[E_crossbow].level);
  i_shop.set_data("num_cur_lamp",     pl.equipment[E_lamp].level    );

  i_shop.set_data("num_post_stamina",
                  pl.equipment[E_stamina].level +
                  pl.equipment[E_stamina].upgrade_amount);
  i_shop.set_data("num_post_backpack",
                  pl.equipment[E_backpack].level +
                  pl.equipment[E_backpack].upgrade_amount);
  i_shop.set_data("num_post_pickaxe",
                  pl.equipment[E_pickaxe].level +
                  pl.equipment[E_pickaxe].upgrade_amount);
  i_shop.set_data("num_post_sword",
                  pl.equipment[E_sword].level +
                  pl.equipment[E_sword].upgrade_amount);
  i_shop.set_data("num_post_crossbow",
                  pl.equipment[E_crossbow].level +
                  pl.equipment[E_crossbow].upgrade_amount);
  i_shop.set_data("num_post_lamp",
                  pl.equipment[E_lamp].level +
                  pl.equipment[E_lamp].upgrade_amount);

  i_shop.set_data("num_cost_stamina",  pl.equipment[E_stamina].price );
  i_shop.set_data("num_cost_backpack", pl.equipment[E_backpack].price);
  i_shop.set_data("num_cost_pickaxe",  pl.equipment[E_pickaxe].price );
  i_shop.set_data("num_cost_sword",    pl.equipment[E_sword].price   );
  i_shop.set_data("num_cost_crossbow", pl.equipment[E_crossbow].price);
  i_shop.set_data("num_cost_lamp",     pl.equipment[E_lamp].price    );
}

void shop_supplies(Player &pl)
{
  Window w_shop(0, 0, 80, 24);
  cuss::interface i_shop;
  i_shop.load_from_file("cuss/i_supplies.cuss");

  update_supply_ui(i_shop, pl);

  while (true) {
    i_shop.draw(&w_shop);
    w_shop.refresh();
    long ch = input();

    if ((ch == 'a' || ch == 'A') && pl.cash >= COST[S_ladders]) {
      pl.cash -= COST[S_ladders];
      pl.supplies[S_ladders] += AMT[S_ladders];
      update_supply_ui(i_shop, pl);
    }

    if ((ch == 'b' || ch == 'B') && pl.cash >= COST[S_rope]) {
      pl.cash -= COST[S_rope];
      pl.supplies[S_rope] += AMT[S_rope];
      update_supply_ui(i_shop, pl);
    }

    if ((ch == 'c' || ch == 'C') && pl.cash >= COST[S_supports]) {
      pl.cash -= COST[S_supports];
      pl.supplies[S_supports] += AMT[S_supports];
      update_supply_ui(i_shop, pl);
    }

    if (ch == 'q' || ch == 'Q') {
      return;
    }

    if (ch == 'u' || ch == 'U') {
      shop_upgrades(pl);
      return;
    }
  }
}

void update_supply_ui(cuss::interface &i_shop, Player &pl)
{
  i_shop.set_data("num_cash", pl.cash);

  i_shop.set_data("num_ladders",  pl.supplies[S_ladders] );
  i_shop.set_data("num_rope",     pl.supplies[S_rope]    );
  i_shop.set_data("num_supports", pl.supplies[S_supports]);

  i_shop.set_data("num_post_ladders",
                  pl.supplies[S_ladders]  + AMT[S_ladders] );
  i_shop.set_data("num_post_rope",
                  pl.supplies[S_rope]     + AMT[S_rope]    );
  i_shop.set_data("num_post_supports",
                  pl.supplies[S_supports] + AMT[S_supports]);

  i_shop.set_data("num_cost_ladders",  COST[S_ladders] );
  i_shop.set_data("num_cost_rope",     COST[S_rope]    );
  i_shop.set_data("num_cost_supports", COST[S_supports]);
}
