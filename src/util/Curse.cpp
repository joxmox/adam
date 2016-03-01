


#include <string>
#include <map>
#include <cstdlib>
#include <cstring>

#include <ncurses.h>
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Curse.hpp"


using namespace std;
using namespace log4cxx;


Curse::Curse() {
	LoggerPtr logger{Logger::getLogger("Curse")};
	LOG4CXX_DEBUG(logger, "initializing ncurses");
	initscr();
	getmaxyx(stdscr, height, width);
	LOG4CXX_DEBUG(logger, "terminal geometry: " << height << "*" << width);
	raw();
	meta(NULL, true);
	nonl();
	keypad(stdscr, true);
	noecho();
	winMap.push_back(stdscr);
}

Curse::~Curse() {
	LoggerPtr logger{Logger::getLogger("~Curse")};
	LOG4CXX_DEBUG(logger, "deleting all windows other than stdscr");
	for (auto id = 1; id < winMap.size(); id++) {
		if (winMap[id]) delWin(id);
	}
	LOG4CXX_DEBUG(logger, "shutting down curses");
	endwin();
}

Win* Curse::creWin(int height, int width, int row, int col) {
	LoggerPtr logger{Logger::getLogger("Curse.creWin")};
	WINDOW* z = newwin(height, width, row, col);
	keypad(z, true);
	LOG4CXX_DEBUG(logger, "created window with id = " << winMap.size());
	winMap.push_back(static_cast<void*>(z));
	Win* w = new Win {height, width, row, col, int(winMap.size()) - 1, this};
	return w;
}

void Curse::delWin(int id) {
	LoggerPtr logger{Logger::getLogger("Curse.delWin")};
	LOG4CXX_DEBUG(logger, "deleting window. id=" << id);
	if (winMap[id]) {
	    delwin(static_cast<WINDOW*>(winMap[id]));
	    winMap[id] = nullptr;
	} else {
		LOG4CXX_WARN(logger, "window is NULL. no action taken");
	}
}

int Curse::getWidth() {
	return width;
}

int Curse::getHeight() {
	return height;
}

int Curse::readKey(int id) {
	LoggerPtr logger{Logger::getLogger("Curse.readKey")};
	LOG4CXX_DEBUG(logger, "reading key from window id=" << id);
    return wgetch(static_cast<WINDOW*>(winMap[id]));
}

void Curse::pos(int id, int row, int col) {
	LoggerPtr logger{Logger::getLogger("Curse.pos")};
	LOG4CXX_DEBUG(logger, "moving window id=" << id << " to " << row << "," << col);
	wmove(static_cast<WINDOW*>(winMap[id]), row, col);
}

void Curse::print(int id, char c) {
	LoggerPtr logger{Logger::getLogger("Curse.print")};
	LOG4CXX_DEBUG(logger, "printing to window id=" << id << ", chr=" << c);
	int sts = winsch(static_cast<WINDOW*>(winMap[id]), c);
	LOG4CXX_TRACE(logger, "sts: " << sts);
}

void Curse::refresh(int id) {
	LoggerPtr logger{Logger::getLogger("Curse.refresh")};
	if (id == -1) {
		for (auto i = 0; i < winMap.size(); i++) {
			WINDOW* w = static_cast<WINDOW*>(winMap[i]);
			LOG4CXX_DEBUG(logger, "refreshing window id=" << i);
			wrefresh(w);
		}
	} else {
		LOG4CXX_DEBUG(logger, "refreshing window id=" << id);
		wrefresh(static_cast<WINDOW*>(winMap[id]));
	}
}

void Curse::print(int id, const string& s) {
	LoggerPtr logger{Logger::getLogger("Curse.print")};
	LOG4CXX_DEBUG(logger, "printing to window id=" << id << ", str=" << s);
	int sts = waddstr(static_cast<WINDOW*>(winMap[id]), s.c_str());
	LOG4CXX_TRACE(logger, "sts: " << sts);
	if (sts != OK) LOG4CXX_ERROR(logger, "unexpected return code from waddstr: " << sts);
}

void Curse::setAttr(int id, curseAttrs attr) {
	LoggerPtr logger{Logger::getLogger("Curse.setAttr")};
	int a = 0;
	if (attr & attRev) {
		LOG4CXX_TRACE(logger, "setting REVERSE attribute");
		a |= A_REVERSE;
	}
	if (attr & attBold) {
		LOG4CXX_TRACE(logger, "setting BOLD attribute");
		a |= A_BOLD;
	}	wattron(static_cast<WINDOW*>(winMap[id]), a);
}

void Curse::clearAttr(int id, curseAttrs attr) {
	LoggerPtr logger{Logger::getLogger("Curse.clearAttr")};
	int a = 0;
	if (attr & attRev) {
		LOG4CXX_TRACE(logger, "clearing REVERSE attribute");
		a |= A_REVERSE;
	}
	if (attr & attBold) {
		LOG4CXX_TRACE(logger, "clearing BOLD attribute");
		a |= A_BOLD;
	}
	wattroff(static_cast<WINDOW*>(winMap[id]), a);
}

Win::Win(int height, int width, int row, int col, int id, Curse* curse): height(height), width(width), row(row), col(col), id(id), curse(curse) {
}

Win::~Win() {
	curse->delWin(id);
}

int Win::getId() {
	return id;
}

int Win::getHeight() {
	return height;
}

int Win::getWidth() {
	return width;
}

void Win::pos(int row, int col) {
	curse->pos(id, row, col);
}

void Win::print(char c) {
	curse->print(id, c);
}

void Win::print(const string& s) {
	curse->print(id, s);
}

void Win::refresh() {
	curse->refresh(id);
}

int Win::readKey() {
	return curse->readKey(id);
}

void Win::setAttr(curseAttrs attr) {
	curse->setAttr(id, attr);
}

void Win::clearAttr(curseAttrs attr) {
	curse->clearAttr(id, attr);
}



