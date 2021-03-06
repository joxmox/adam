#include <string>
#include <map>
#include <cstdlib>
#include <cstring>
#include <mutex>

#include <ncurses.h>

#include "logging.hpp"
#include "Curse.hpp"


using namespace std;

#ifdef LOG4CXX
LoggerPtr Curse::logger{Logger::getLogger("Curse")};
LoggerPtr Win::logger{Logger::getLogger("Win")};
#endif

mutex Curse::gMaster;

Curse::Curse() {
#ifdef LOG4CXX
	LOG4CXX_DEBUG(logger, "initializing ncurses");
#endif
	lock_guard<mutex> guard(gMaster);
	initscr();
	getmaxyx(stdscr, height, width);
	LOG4CXX_DEBUG(logger, "terminal geometry: " << height << "*" << width);
	raw();
	meta(NULL, true);
	nonl();
	keypad(stdscr, true);
	noecho();
	winMap.push_back(stdscr);
	initEsc();
}

Curse::~Curse() {
	LOG4CXX_DEBUG(logger, "shutting down curses");
	for (auto id = 1; id < winMap.size(); id++) {
		if (winMap[id]) {
			LOG4CXX_WARN(logger, "found stray WINDOW " << id << ". deleting, but you should delete this explicitly!");
			delWin(id);
		}
	}
	lock_guard<mutex> guard(gMaster);
	endwin();
}

void Curse::addEsc(const string s, int a) {
	escVec* e = escMap;
	for (auto c : s) {
		if (e->ptr.find(c) == e->ptr.end()) {
			escVec* f = new escVec;
			e->ptr[c] = f;
			e = f;
		} else {
			e = e->ptr[c];
		}
	}
	e->final = true;
	e->value =  a;
}

void Curse::initEsc() {
	addEsc("[12~", 1);
	addEsc("[13~", 2);
	addEsc("[14~", 3);
	addEsc("[15~", 4);
	addEsc("Ok", 4);
	addEsc("Ol", 5);
	addEsc("Om", 276); //kp4 / kp-minus => do
}


Win* Curse::creWin(int height, int width, int row, int col) {
	lock_guard<mutex> guard(gMaster);
	WINDOW* z = newwin(height, width, row, col);
	keypad(z, true);
	LOG4CXX_DEBUG(logger, "created window " << height << "*" << width << " with id = " << winMap.size() << " at " << row << "," << col);
	LOG4CXX_DEBUG(logger, "z = " << reinterpret_cast<long>(z));
	winMap.push_back(static_cast<void*>(z));
	Win* w = new Win {height, width, row, col, int(winMap.size()) - 1, this};
	return w;
}

int Curse::addWin(int height, int width, int row, int col) {
	lock_guard<mutex> guard(gMaster);
	WINDOW* z = newwin(height, width, row, col);
	keypad(z, true);
	LOG4CXX_DEBUG(logger, "created WINDOW " << height << "*" << width << " with id = " << winMap.size() << " at " << row << "," << col);
	LOG4CXX_DEBUG(logger, "WINDOW* = " << reinterpret_cast<long>(z));
	winMap.push_back(static_cast<void*>(z));
	LOG4CXX_DEBUG(logger, "adding WINDOW with id = " << winMap.size() - 1 << "to winMap");
	return winMap.size() - 1;
}

void Curse::delWin(int id) {
	if (winMap[id]) {
		LOG4CXX_DEBUG(logger, "deleting ncurses WINDOW. id=" << id);
		lock_guard<mutex> guard(gMaster);
	    delwin(static_cast<WINDOW*>(winMap[id]));
	    winMap[id] = nullptr;
	} else {
		LOG4CXX_WARN(logger, "WINDOW is NULL. no action taken");
	}
}

void Curse::delWinAll() {
	for (auto& w : winMap) {
		if (w) {
		    delwin(static_cast<WINDOW*>(w));
		    w = nullptr;
		}
	}
}



int Curse::getWidth() {
	return width;
}

int Curse::getHeight() {
	return height;
}

int Curse::readKey(int id) {

	LOG4CXX_DEBUG(logger, "reading key from window id=" << id);
	int k = wgetch(static_cast<WINDOW*>(winMap[id]));
	if (k != 27) return k;
	LOG4CXX_DEBUG(logger, "escape key detected");
	escVec* e {escMap};
	while (!e->final) {
		k = wgetch(static_cast<WINDOW*>(winMap[id]));;
		if (e->ptr.find(k) != e->ptr.end()) {
			LOG4CXX_DEBUG(logger, "valid sequence: " << k);
			e = e->ptr[k];
		} else {
			LOG4CXX_DEBUG(logger, "invalid sequence: " << k << " - returning -1");
			return -1;
		}
	}
	return e->value;
}

string Curse::readString(int id) {
	LOG4CXX_TRACE(logger, "enter");
	char buff[128];
	noraw();
	echo();
	wgetnstr(static_cast<WINDOW*>(winMap[id]), buff, 127);
	raw();
	noecho();
	LOG4CXX_DEBUG(logger, "read string " << string(buff) << " from keyboard");
	return string(buff);
	LOG4CXX_TRACE(logger, "exit");
}

void Curse::pos(int id, int row, int col) {
	LOG4CXX_DEBUG(logger, "moving window id=" << id << " to " << row << "," << col);
	lock_guard<mutex> guard(gMaster);
	wmove(static_cast<WINDOW*>(winMap[id]), row, col);
}

void Curse::insertChar(int id, char c) {
	LOG4CXX_DEBUG(logger, "printing to window id=" << id << ", chr=" << c);
	lock_guard<mutex> guard(gMaster);
	int sts = winsch(static_cast<WINDOW*>(winMap[id]), c);
	LOG4CXX_TRACE(logger, "sts: " << sts);
}

void Curse::refresh(int id) {
	lock_guard<mutex> guard(gMaster);
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

void Curse::printStr(int id, const string& s) {
	LOG4CXX_DEBUG(logger, "printing to window id=" << id << ", str=" << s);
//	lock_guard<mutex> guard(gMaster);
	int sts = winsstr(static_cast<WINDOW*>(winMap[id]), s.c_str());
	LOG4CXX_TRACE(logger, "sts: " << sts);
	if (sts != OK) LOG4CXX_ERROR(logger, "unexpected return code from winsstr: " << sts);
}

void Curse::printStr(int id, char s) {
	printStr(id, string(1, s));
}

void Curse::setAttr(int id, curseAttrs attr) {
	lock_guard<mutex> guard(gMaster);
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
	lock_guard<mutex> guard(gMaster);
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

void Curse::clearEol(int id) {
	lock_guard<mutex> guard(gMaster);
	int sts = wclrtoeol(static_cast<WINDOW*>(winMap[id]));
	LOG4CXX_DEBUG(logger, "id = " << id << ", status = " << sts);
}

void Curse::insertLine(int id) {
	LOG4CXX_TRACE(logger, "enter");
	lock_guard<mutex> guard(gMaster);
	winsertln(static_cast<WINDOW*>(winMap[id]));
	LOG4CXX_TRACE(logger, "exit");
}

void Curse::delChar(int id) {
	LOG4CXX_TRACE(logger, "enter");
	lock_guard<mutex> guard(gMaster);
	wdelch(static_cast<WINDOW*>(winMap[id]));
	LOG4CXX_TRACE(logger, "exit");
}

void Curse::delLine(int id) {
	LOG4CXX_TRACE(logger, "enter");
	lock_guard<mutex> guard(gMaster);
	wdeleteln(static_cast<WINDOW*>(winMap[id]));
	LOG4CXX_TRACE(logger, "exit");
}

Win::Win(int height, int width, int row, int col, int id, Curse* curse): height(height), width(width), row(row), col(col), id(id), curse(curse) {
}

Win::Win(Curse* curse, int height, int width, int row, int col): height(height), width(width), row(row), col(col), curse(curse) {
	id = curse->addWin(height, width, row, col);
}


Win::~Win() {
	LOG4CXX_DEBUG(logger, "deleting instance. id=" << id);
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
	this->row = row;
	this->col = col;
	curse->pos(id, row, col);
}

void Win::move(int rows, int cols) {
	pos(row + rows, col + cols);
}

void Win::moveUp(int r) {
	pos(row - r, col);
}

void Win::moveDown(int r) {
	pos(row + r, col);
}

void Win::moveLeft(int c) {
	pos(row, col - c);
}

void Win::moveRight(int c) {
	pos(row, col + c);
}

void Win::moveLeft(const string& str) {

}

void Win::moveRight(const string& str) {

}

void Win::insertChar(char c) {
	curse->insertChar(id, c);
}

void Win::printStr(const string& s) {
	curse->printStr(id, s);
	pos(row, col);
}

void Win::printStr(char s) {
	printStr(string(1, s));
}

void Win::refresh() {
	curse->refresh(id);
}

int Win::readKey() {
	return curse->readKey(id);
}

string Win::readString() {
	return curse->readString(id);
}

void Win::setAttr(curseAttrs attr) {
	curse->setAttr(id, attr);
}

void Win::clearAttr(curseAttrs attr) {
	curse->clearAttr(id, attr);
}

void Win::clearEol() {
	curse->clearEol(id);
}

void Win::insertLine() {
	curse->insertLine(id);
}

void Win::setRow(int r) {
	pos(r, col);
}

void Win::setCol(int c) {
	pos(row, c);
}

int Win::getRow() {
	return row;
}

int Win::getCol() {
	return col;
}

void Win::delChar() {
	curse->delChar(id);
}

void Win::delLine() {
	curse->delLine(id);
}

void Win::debug() {
	LOG4CXX_DEBUG(logger, "window id: " << id);
	LOG4CXX_DEBUG(logger, "window position: " << row << "," << col);
}


