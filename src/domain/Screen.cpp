/*
 * Screen.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: joxmox
 */

#include <string>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Screen.hpp"
#include "Curse.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr Screen::logger{Logger::getLogger("Screen")};

Screen::Screen(Curse* cur, int startRow, int numScreens, int pos, Win* cmdWin, Win* messWin) :
		Win(cur, cur->getHeight() - 3, cur->getWidth(), 0, 0), cur(cur), cmdWin(cmdWin), messWin(messWin) {
	stsWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 3, 0);
}

Screen::~Screen() {
	LOG4CXX_DEBUG(logger, "destrouing screen");
	delete stsWin;
}

Win* Screen::createCmdWin() {
	LOG4CXX_DEBUG(logger, "Screen window id = " << id);
	cmdWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 2, 0);
	LOG4CXX_DEBUG(logger, "cmdWin = " << reinterpret_cast<long>(cmdWin));
	return cmdWin;
}

Win* Screen::createMessWin() {
	LOG4CXX_DEBUG(logger, "Screen window id = " << id);
	messWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 1, 0);
	LOG4CXX_DEBUG(logger, "messWin = " << reinterpret_cast<long>(messWin));
	return messWin;
}

void Screen::setStatus(const string& str1, const string& str2) {
	stsWin->pos(0, 0);
	stsWin->setAttr(attRev);
	stsWin->printStr(str1 + string(stsWin->getWidth() - str1.size() - str2.size(), ' ') + str2);
	stsWin->clearAttr(attRev);
	stsWin->refresh();
}

void Screen::printMessage(const string& str) {
	messWin->pos(0, 0);
	messWin->printStr(str);
	messWin->pos(0, str.size());
	messWin->clearEol();
	messWin->refresh();
}


void Screen::push() {
	posStack.push_back({row, col});
	LOG4CXX_DEBUG(logger, "saved position " << row << "," << col);
}

void Screen::push(int r, int c) {
	push();
	LOG4CXX_DEBUG(logger, "...and moving to " << r << "," << c);
	pos(r, c);
}

void Screen::pop() {
	if (posStack.empty()) throw logic_error("position stack is empty - cannot pop");
	pair<int, int> res {posStack.back()};
	posStack.pop_back();
	pos(res.first, res.second);
	LOG4CXX_DEBUG(logger, "restored position " << row << "," << col);
}

bool Screen::atBottom() {
	return row >= maxRow();
}

int Screen::maxRow() {
	return height - 1;
}

void Screen::repaint(const vector<string>& data, int topRow) {
	LOG4CXX_DEBUG(logger, "topRow: " << topRow);
	LOG4CXX_DEBUG(logger, "data size: " << data.size());
	push();
	for (auto i = 0; i < height; i++) {
		LOG4CXX_DEBUG(logger, i);
		pos(i, 0);
		LOG4CXX_DEBUG(logger, "back");
		if (topRow + i < data.size()) {
			string s = data[topRow + i];
			LOG4CXX_DEBUG(logger, "string: " << s);
			printStr(s);
			move(0, s.size());
		}
		clearEol();
	}
	pop();
}

/**
 * Save position. Move to start of row. Insert new line. Restore position.
 */
void Screen::insertLine(const string& line) {
	push();
	LOG4CXX_DEBUG(logger, "moving to col=0 and inserting blank line");
	setCol(0);
	Win::insertLine();
	LOG4CXX_DEBUG(logger, "printing supplied string (if any)");
	if (!line.empty()) printStr(line);
	pop();
}

void Screen::printBelow(const string& line) {
	push();
	pos(row + 1, 0);
	printStr(line);
	pop();
}

void Screen::insertChar(char c) {
		Win::insertChar(c);
		move(0, 1);
}


