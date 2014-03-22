#ifndef _GLYPH_H_
#define _GLYPH_H_

#include "color.h"
#include <istream>

#define HILITE_COLOR c_blue

struct glyph
{
  long symbol;
  nc_color fg;
  nc_color bg;

  glyph() { symbol = 'x'; fg = c_red; bg = c_white; }
  glyph(long S, nc_color F, nc_color B) : symbol (S), fg (F), bg (B) { }

  glyph invert();
  glyph hilite(nc_color back = HILITE_COLOR);
  std::string text_formatted();

  bool operator==(const glyph &rhs);

  void make_line_drawing(bool north, bool east, bool south, bool west);

  std::string save_data();
  void load_data(std::istream &datastream);
  void load_data_text(std::istream &datastream, // For human-readable data
                      std::string owner_name = "Unknown");
};

#endif
