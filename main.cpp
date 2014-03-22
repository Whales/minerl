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
    map.draw(&w_main, player.posx, player.posy);
    player.update_hud(i_hud);
    i_hud.draw(&w_hud);
    w_main.refresh();
    w_hud.refresh();
    ch = input();
    if (ch == KEY_LEFT  || ch == 'h')  player.posx--;
    if (ch == KEY_RIGHT || ch == 'l')  player.posx++;
    if (ch == KEY_UP    || ch == 'k')  player.posy--;
    if (ch == KEY_DOWN  || ch == 'j')  player.posy++;
  }

  end_display();
  return 0;
}
