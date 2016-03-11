/*
 * Buffer.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: sebis
 */

#include <string>
#include <stdexcept>
#include <fstream>

#include "log4cxx/logger.h"

#include "Buffer.hpp"
#include "Screen.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr Buffer::logger{Logger::getLogger("Buffer")};

Buffer::Buffer(const string& bufName, const string& fileName, Screen* scr): fileName(fileName), bufName(bufName), scr(scr) {
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
}

Buffer::~Buffer() {
	LOG4CXX_DEBUG(logger, "destroying buffer");
	delete scr;
}

void Buffer::updateStatus() {
	scr->setStatus(" Buffer: " + bufName, "| " + stsWrite + " | " + stsInsert + " | " + stsDirection + " ");
}

void Buffer::printMessage(const string& str) {
	scr->printMessage(str);
}

int Buffer::readFile(const string& fileName) {
	string readFile = fileName;
	if (readFile.empty()) readFile = this->fileName;
	LOG4CXX_DEBUG(logger, "loading file " << readFile << " into buffer");
	int count = -1;
	ifstream inf{readFile};
	if (inf) {
		push();
		string line;
		count = 0;
		while (getline(inf, line)) {
			data.insert(data.begin() + row++, line);
			count++;
		}
		printMessage(to_string(count) + " lines read from file " + this->fileName);
		pop();
	} else {
		printMessage("Editing new file.  Could not find: " + this->fileName);
	}
	scr->repaint(data, row);
	updateStatus();
	return count;
}

bool Buffer::fileExists() {
	ifstream inf {fileName};
	return bool(inf);
}

bool Buffer::atTopRow() {
	if (row < 0) throw logic_error("invalid buffer row detected: " +to_string(row));
	return row == 0;
}

bool Buffer::atBotRow() {
	if (row >= data.size()) throw logic_error("invalid buffer row detected: " + to_string(row) + " - " + to_string(data.size()));
	return row == data.size() - 1;
}

int Buffer::getLines() {
	return data.size() - 1;
}

void Buffer::adjustBuffer(int type) {
	LOG4CXX_TRACE(logger, "enter");
	if (atBotRow()) {
		LOG4CXX_DEBUG(logger, "at bottom row - inserting empty line at row " << row);
		data.insert(data.begin() + row, string(col, ' '));
		scr->insertLine();
		LOG4CXX_DEBUG(logger, "row:" << row);
	}
	switch (type) {
	case 1:
		if (col > data[row].size()) {
			LOG4CXX_DEBUG(logger, "cursor right of data - padding with spaces");
			data[row].append(string(col - data[row].size(), ' '));
		}
		break;
	case 2:
		if (col > data[row].size()) {
			LOG4CXX_DEBUG(logger, "cursor right of data - reset to end of line");
			gotoEol();
		}
		break;
	default:
		LOG4CXX_FATAL(logger, "invalid adjust type specified: " << type);
		throw invalid_argument("invalid buffer adjustment type specified: " + to_string(type));
	}
	if (!firstKill) {
		LOG4CXX_DEBUG(logger, "we have moved - reset firstKill");
		firstKill = true;
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::insertChar(int key) {
	LOG4CXX_DEBUG(logger, "inserting char: " << key);
	adjustBuffer(1);
	scr->insertChar(key);
	data[row].insert(col++, string(1, key));
}

void Buffer::moveUp() {
	firstKill = true;
	LOG4CXX_TRACE(logger, "enter");
	if (!atTopRow()) {
		LOG4CXX_DEBUG(logger, "buffer not at top - moving");
		scr->moveUp(data[--row]);
	} else {
		LOG4CXX_DEBUG(logger, "buffer  at top - no action");
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveDown() {
	firstKill = true;
	LOG4CXX_TRACE(logger, "enter");
	if (!atBotRow()) {
		LOG4CXX_DEBUG(logger, "buffer not at bottom - ok to move");
		scr->moveDown(data[++row]);
	} else {
		LOG4CXX_DEBUG(logger, "buffer already at bottom row");
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveLeft() {
	firstKill = true;
	LOG4CXX_TRACE(logger, "enter");
	if (col > 0) {
		col--;
		scr->move(0, -1);
	} else {
		LOG4CXX_DEBUG(logger, "already at start of line");
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveRight() {
	firstKill = true;
	LOG4CXX_TRACE(logger, "enter");
	col++;
	int w = scr->getWidth();
	if (scr->getCol() + 1 < w) {
		scr->move(0, 1);
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::insertBreak() {
	adjustBuffer(2);
	string tmp = data[row].substr(col);
	data[row].erase(col, string::npos);
	scr->insertBreak(tmp);
	data.insert(data.begin() + ++row, tmp);
	col = 0;
}


void Buffer::deleteChar() {
	LOG4CXX_TRACE(logger, "enter");
	adjustBuffer(1);
	if (col > 0) {
		LOG4CXX_DEBUG(logger, "cursor in the wild - remove single char");
		data[row].erase(--col, 1);
		scr->delChar();
	} else {
		LOG4CXX_DEBUG(logger, "cursor at leftmost position - join lines");
		if (row > 0) {
			string s;
			int p = row + scr->getHeight() - scr->getRow();
			if (p < data.size()) s = data[p];
			scr->deleteBreak(s, data[row], data[row - 1]);
			col = data[row - 1].size();
			data[row - 1] += data[row];
			data.erase(data.begin() + row--);
		}
		LOG4CXX_TRACE(logger, "exit");
	}
}

void Buffer::gotoSol() {
	firstKill = false;
	if (col != 0) {
		col = 0;
		scr->setCol(0);
	} else {
		printMessage("You are already at the start of a line.");
	}
}

void Buffer::gotoEol() {
	firstKill = true;
	int maxRow = data[row].size();
	if (col != maxRow) {
		col = maxRow;
		scr->setCol(maxRow);
	} else {
		printMessage("You are already at the end of a line.");
	}
}

void Buffer::pageUp() {
	LOG4CXX_TRACE(logger, "enter");
	firstKill = true;
	int toMove = scr->getHeight() - 2;
	int screenRow = scr->getRow();
	LOG4CXX_DEBUG(logger, "rows to move: " << toMove);
	int topRow = row - screenRow - toMove;
	int newRow = row - toMove;
	if (topRow < 0) {
		screenRow += topRow;
	}
	if (newRow < 0) {
		screenRow -= newRow;
		newRow = 0;
	}
	if (screenRow < 0) screenRow = 0;
	LOG4CXX_DEBUG(logger, "new buffer row: " << newRow);
	LOG4CXX_DEBUG(logger, "new screen row: " << screenRow);
	row = newRow;
	scr->repaint(data, row, screenRow);
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::pageDown() {
	LOG4CXX_TRACE(logger, "enter");
	firstKill = true;
	int maxRow = data.size() - 1;
	int toMove = scr->getHeight() - 2;
	int maxScreen = scr->getHeight() - 1;
	int screenRow = scr->getRow();
	LOG4CXX_DEBUG(logger, "rows to move: " << toMove);
	int botRow = row + scr->getHeight() - screenRow + toMove;
	int newRow = row + toMove;
	if (botRow > maxRow) {
		int diff = botRow - maxRow;
		screenRow += diff;
	}
	if (newRow > maxRow) {
		int diff = newRow - maxRow;
		screenRow += diff;
		newRow = maxRow;
	}
	if (screenRow > maxScreen) screenRow = maxScreen;
	LOG4CXX_DEBUG(logger, "new buffer row: " << newRow);
	LOG4CXX_DEBUG(logger, "new screen row: " << screenRow);
	row = newRow;
	scr->repaint(data, row, screenRow);
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::dump() {
	LOG4CXX_DEBUG(logger, "row: " << row);
	LOG4CXX_DEBUG(logger, "col: " << col);
	int c {0};
	for (auto d : data) {
		LOG4CXX_DEBUG(logger, c++ << ": " << d);
	}
}

void Buffer::setFocus() {
	LOG4CXX_DEBUG(logger, "giving focus to screen main window, id=" << scr->getId());
	scr->refresh();
}

Screen* Buffer::getScr() {
	return scr;
}

void Buffer::push() {
	posStack.push_back({row, col});
	LOG4CXX_DEBUG(logger, "saved position " << row << "," << col);
}

void Buffer::push(int r, int c) {
	push();
	LOG4CXX_DEBUG(logger, "...and moving to " << r << "," << c);
	row = r;
	col = c;
}

void Buffer::pop() {
	if (posStack.empty()) throw logic_error("position stack is empty - cannot pop");
	pair<int, int> res {posStack.back()};
	posStack.pop_back();
	row = res.first;
	col = res.second;
	LOG4CXX_DEBUG(logger, "restored position " << row << "," << col);
}

void Buffer::killLine() {
	if (!atBotRow()) {
		string s;
		int p = row + scr->getHeight() - scr->getRow();
		if (p < data.size()) s = data[p];
		scr->delLine(s);
		if (firstKill) {
			LOG4CXX_DEBUG(logger, "this is first kill - clearing paste buffer");
			pasteBuf.clear();
			firstKill = false;
		} else {
			LOG4CXX_DEBUG(logger, "this is not first kill - keeping paste buffer intact");
		}
		pasteBuf.push_back(data[row]);
		data.erase(data.begin() + row);
	}
}

void Buffer::paste() {
	for (auto p : pasteBuf) {
		LOG4CXX_DEBUG(logger, "pasting: " << p);
		insertLine(p);
		firstKill = true;
	}
}

void Buffer::insertLine(const string& s) {
	scr->insertLine(s);
	data.insert(data.begin() + row, s);
	moveDown();
}

void Buffer::saveToFile(const string& fileName) {
	ofstream of {fileName};
	if (!of) {
		LOG4CXX_FATAL(logger, "could not open file " << " for saving");
		throw runtime_error("could not open file for saving: " + fileName);
	}
	string tmp = data.back();
	data.pop_back();
	for (auto d : data) {
		of << d << endl;
	}
	data.push_back(tmp);
}
