

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

	raw();
	meta(NULL, true);
	nonl();
	keypad(stdscr, true);
	noecho();
	row = 0;
	col = 0;

}

Tty::~Tty() {
	LoggerPtr logger{Logger::getLogger("~Tty")};
	LOG4CXX_DEBUG(logger, "shutting down curses");
	endwin();
}

