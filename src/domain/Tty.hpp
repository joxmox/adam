#pragma once

#include <ncurses.h>
#include <string>
#include <map>
#include <fstream>


class Tty {
  int height;
  int width;
  int row;
  int col;
public:
  Tty();
  ~Tty();
};
