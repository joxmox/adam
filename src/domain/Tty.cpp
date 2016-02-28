

#include <ncurses.h>
#include <string>
#include <map>
#include <cstdlib>
#include <cstring>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Tty.hpp"

using namespace std;
using namespace log4cxx;


Tty::Tty() {
	LoggerPtr logger{Logger::getLogger("Tty")};
	LOG4CXX_DEBUG(logger, "creating Tty");
	initscr();
	LOG4CXX_TRACE(logger, "curses initialized")
	getmaxyx(stdscr, height, width);
	LOG4CXX_DEBUG(logger, "geometry: " << height << "*" << width);
	cmdRow = height - 1;
	messRow = height - 2;
	raw();
	meta(NULL, true);
	nonl();
	keypad(stdscr, true);
	noecho();

}

Tty::~Tty() {
	LoggerPtr logger{Logger::getLogger("~Tty")};
	LOG4CXX_DEBUG(logger, "shutting down curses");
	endwin();
}

int Tty::getWidth() {
	return width;
}

int Tty::getHeight() {
	return height - 2;
}

int Tty::readKey() {
  return getch();
}

void Tty::pos(int row, int col) {
	move(row, col);
}

void Tty::print(char c) {
	winsch(stdscr, c);
}
