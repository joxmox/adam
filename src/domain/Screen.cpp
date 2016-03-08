/*
 * Screen.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: joxmox
 */

#include <string>
#include <chrono>
#include <thread>

#include "log4cxx/logger.h"

#include "Screen.hpp"
#include "Curse.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr Screen::logger{Logger::getLogger("Screen")};

mutex Screen::gMessage;

Screen::Screen(Curse* cur, int startRow, int numScreens, int pos, Win* cmdWin, Win* messWin) :
		Win(cur, cur->getHeight() - 3, cur->getWidth(), 0, 0), cur(cur), cmdWin(cmdWin), messWin(messWin) {
	stsWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 3, 0);
}

Screen::~Screen() {
	LOG4CXX_DEBUG(logger, "destroying screen");
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

Win* Screen::getMessWin() {
	return messWin;
}

Win* Screen::getCmdWin() {
	return messWin;
}

void Screen::printMessage(const string& str) {
	LOG4CXX_DEBUG(logger, "pointer to messWin: " << messWin);
	LOG4CXX_DEBUG(logger, "starting async printMessage");
	thread th (asyncMessage, this, str);
	th.detach();
}

void Screen::asyncMessage(Screen* scr, const string& str) {
	LOG4CXX_TRACE(logger, "enter");
	lock_guard<mutex> guard(gMessage);
	LOG4CXX_DEBUG(logger, "lock taken");
	Win* messWin = scr->getMessWin();
	LOG4CXX_DEBUG(logger, "pointer to messWin: " << messWin);
	LOG4CXX_DEBUG(logger, "str: " << str);
	messWin->pos(0, 0);
	messWin->printStr(str);
	messWin->pos(0, str.size());
	messWin->clearEol();
	messWin->refresh();
}

void Screen::printWarning(const string& str) {
	thread th {asyncWarning, this, str};
	th.detach();
}

void Screen::asyncWarning(Screen* scr, const string& str) {
	lock_guard<mutex> guard(gMessage);
	Win* messWin = scr->getMessWin();
	messWin->pos(0, 0);
	messWin->setAttr(attRev);
	messWin->printStr(str);
	messWin->pos(0, str.size());
	messWin->clearEol();
	messWin->refresh();
	this_thread::sleep_for(chrono::milliseconds(100));
	messWin->pos(0, 0);
	messWin->clearAttr(attRev);
	messWin->printStr(str);
	messWin->pos(0, str.size());
	messWin->clearEol();
	messWin->refresh();
}

void Screen::printCommand(const string& str) {
	cmdWin->pos(0, 0);
	cmdWin->printStr(str);
	cmdWin->pos(0, str.size());
	cmdWin->clearEol();
	cmdWin->refresh();
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

void Screen::repaint(const vector<string>& data, int bufRow, int newRow) {
	if (newRow < 0) newRow = getRow();
	setRow(newRow);
	push();
	LOG4CXX_DEBUG(logger, "data size: " << data.size());
	for (auto r = 0; r < height; r++) {
		pos(r, 0);
		int i = bufRow - newRow + r;
		if (i < data.size()) {
			string s = data[i];
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


void Screen::moveUp(const string& s) {
	if (row == 0) {
		push(0, 0);
		insertLine();
		printStr(s);
		pop();
	} else {
		Win::moveUp();
	}
}

void Screen::moveDown(const string& s) {
	if (atBottom()) {
		push(0, 0);
		delLine();
		pop();
		push(row, 0);
		printStr(s);
		pop();
	} else {
		Win::moveDown();
	}
}
void Screen::delLine() {
	Win::delLine();
}

void Screen::delLine(const string& s) {
	Win::delLine();
	push(height - 1, 0);
	printStr(s);
	pop();
}

void Screen::delLine(int r, const string& s) {
	push();
	pos(r, 0);
	Win::delLine();
	if (!s.empty()) {
		pos(height - 1, 0);
		printStr(s);
	}
	pop();
}

void Screen::insertBreak(const string& s) {
	LOG4CXX_TRACE(logger, "enter");
	LOG4CXX_DEBUG(logger, "clearing to eol");
	clearEol();
	if (atBottom()) {
		LOG4CXX_DEBUG(logger, "at bottom");
		LOG4CXX_DEBUG(logger, "deleting line 0, supplying string:" << s);
		delLine(0, s);
		LOG4CXX_DEBUG(logger, "setting col=0");
		setCol(0);
	} else {
		LOG4CXX_DEBUG(logger, "not at bottom");
		LOG4CXX_DEBUG(logger, "increasing row and setting col=0");
		pos(++row, 0);
		LOG4CXX_DEBUG(logger, "insert string: " << s);
		insertLine(s);
	}
	LOG4CXX_TRACE(logger, "exit");

}

void Screen::delChar() {
	moveLeft();
	Win::delChar();
}

void Screen::moveUp(int r) {
	Win::moveUp(r);
}

void Screen::deleteBreak(const string& botRow, const string& delStr, const string& orgStr) {
	delLine(botRow);
	if (row > 0) {
		moveUp(1);
	} else {
		moveUp(orgStr);
	}
	setCol(orgStr.size());
	printStr(delStr);
}

