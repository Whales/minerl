#include "window.h"
#include "globals.h"
#include "map.h"
#include "player.h"

int main()
{
  init_display();

  init_tiles();

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
      if (ch == 'w') {
        if (player.equipment[E_ladders] > 0) {
          player.equipment[E_ladders]--;
          map.set_tile(player.posx, player.posy, T_ladder);
          player_took_turn = true;
        }
      } else if (ch == 's') {
        if (player.equipment[E_supports] > 0) {
          player.equipment[E_supports]--;
          map.set_tile(player.posx, player.posy, T_support);
          player_took_turn = true;
        }
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
