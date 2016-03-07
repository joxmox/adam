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
#include "log4cxx/propertyconfigurator.h"

#include "Buffer.hpp"
#include "Screen.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr Buffer::logger{Logger::getLogger("Buffer")};

Buffer::Buffer(const string& bufName, const string& fileName, Screen* scr): fileName(fileName), bufName(bufName), scr(scr) {
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
	maxLine = -1;
	topLine = 0;
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
	ifstream inf{readFile};
	maxLine = 0;
	if (inf) {
		string line;
		push();
		while (getline(inf, line)) {
			data.insert(data.begin() + row++, line);
			maxLine++;
			col = 0;
		}
		printMessage(to_string(maxLine) + " lines read from file " + this->fileName);
		pop();
	} else {
		printMessage("Editing new file.  Could not find: " + this->fileName);
		data.push_back(string {});
	}
	scr->repaint(data, topLine);
	updateStatus();
	return maxLine;
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
	return maxLine;
}

void Buffer::adjustBuffer() {
	if (row > data.size()) {
		LOG4CXX_ERROR(logger, "cursor way too far down - should not happen");
		throw logic_error("error");
	}
//	if (row == data.size()) {
//		LOG4CXX_DEBUG(logger, "cursor one row down - adding line to buffer");
//		data.push_back(string { });
//	}
}

void Buffer::insertChar(int key) {
	LOG4CXX_DEBUG(logger, "inserting char: " << key);
	if (atBotRow()) {
		LOG4CXX_DEBUG(logger, "at bottom row - inserting empty line at row " << row);
		data.insert(data.begin() + row, string(col, ' '));
		scr->insertLine();
		LOG4CXX_DEBUG(logger, "row:" << row);
	}
	if (col > data[row].size()) {
		LOG4CXX_DEBUG(logger, "cursor right of data - padding with spaces");
		data[row].append(string(col - data[row].size(), ' '));
	}
	adjustBuffer();
	scr->insertChar(key);
	data[row].insert(col++, string(1, key));
}

void Buffer::moveUp() {
	LOG4CXX_TRACE(logger, "enter");
	if (!atTopRow()) {
		LOG4CXX_DEBUG(logger, "buffer not at top - moving");
		scr->moveUp(data[--row]);
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveDown() {
	LOG4CXX_TRACE(logger, "enter");
	if (!atBotRow()) {
		LOG4CXX_DEBUG(logger, "buffer not at bottom - ok to move");
		row++;
		if (!scr->atBottom()) {
			LOG4CXX_DEBUG(logger, "screen not at bottom - ok to move");
			scr->moveDown();
		} else {
			LOG4CXX_DEBUG(logger, "screen at bottom - need to scroll");
			topLine++;
			scr->push(0, 0);
			scr->delLine();
			scr->pos(scr->maxRow(), 0);
			LOG4CXX_TRACE(logger, "Hej1");
			if (!atBotRow()) scr->printStr(data[row]);
			LOG4CXX_TRACE(logger, "Hej2");
			scr->pop();
		}
	} else {
		LOG4CXX_DEBUG(logger, "buffer already at bootom row");
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveLeft() {
	LOG4CXX_TRACE(logger, "enter");
	if (col > 0) {
		col--;
		scr->move(0, -1);
	} else {
		printMessage("Already at start of line."); //FIXME: check text with eve
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveRight() {
	LOG4CXX_TRACE(logger, "enter");
	col++;
	int w = scr->getWidth();
	if (scr->getCol() + 1 < w) {
		scr->move(0, 1);
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::insertBreak() {
	adjustBuffer();
	string tmp = data[row].substr(col);
	data[row].erase(col, string::npos);
	scr->clearEol();
	scr->setCol(0);
	scr->move(1, 0);
	scr->insertLine();
	scr->printStr(tmp);
	data.insert(data.begin() + ++row, tmp);
	col = 0;
}


void Buffer::deleteChar() {
	LOG4CXX_TRACE(logger, "enter");
	adjustBuffer();
	if (col > 0) {
		LOG4CXX_TRACE(logger, "cursor in the wild - remove single char");
		data[row].erase(--col, 1);
		scr->move(0, -1);
		scr->delChar();
	} else {
		LOG4CXX_TRACE(logger, "cursor at leftmost position - join lines");
		if (row > 0) {
			scr->delLine();
			scr->pos(scr->getRow() - 1, data[row - 1].size());
			scr->debug();
			scr->printStr(data[row]);
			scr->debug();
			col = data[row - 1].size();
			data[row - 1] += data[row];
			data.erase(data.begin() + row--);
		}
		LOG4CXX_TRACE(logger, "exit");
	}

//	DBG << "enter" << endl;
//	adjust();
//	if (pos.getCol() > 0) {
//		tty->move(--pos);
//		tty->delChar();
//		buf->delChar();
//	} else {
//		int br = buf->getRow();
//		DBG << "Buffer row: " << br << endl;
//		if (br > 0) {
//			int nc = buf->joinLines();
//			DBG << "new column: " << nc << endl;
//			tty->delLine();
//			pos.moveUp();
//			tty->move(pos);
//			DBG << "new pos (after move up): " << pos << endl;
//			tty->print(buf->getCurLine());
//			tty->move(siz.getLowLeft());
//			tty->print(buf->getLine(siz.getEndRow() + br - pos.getRow() - 1));
//			pos.setCol(nc);
//			buf->recalcMarks(-1);
//		}
//	}
}

void Buffer::gotoSol() {
	if (col != 0) {
		col = 0;
		scr->setCol(0);
	} else {
		printMessage("You are already at the start of a line.");
	}
}

void Buffer::gotoEol() {
	int maxRow = data[row].size();
	if (col != maxRow) {
		col = maxRow;
		scr->setCol(maxRow);
	} else {
		printMessage("You are already at the end of a line.");
	}
}

void Buffer::pageUp() {
	int topRow = row - scr->getRow();
	LOG4CXX_DEBUG(logger, "Current toprow: " << topRow);
	int offset = -scr->getHeight() + 2;
	if (row + offset < 0) {
		offset = -row;
	}
	LOG4CXX_DEBUG(logger, "offset: " << offset);
	row += offset;
	scr->repaint(data, max(topRow + offset, 0));
}

void Buffer::pageDown() {
	int topRow = row - scr->getRow();
	LOG4CXX_DEBUG(logger, "Current toprow: " << topRow);
	int offset = scr->getHeight() - 2;
	if (row + offset > data.size()) {
		offset = data.size() - row;
	}
	LOG4CXX_DEBUG(logger, "offset: " << offset);
	row += offset;
	scr->repaint(data, topRow + offset);
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

Screen* Buffer::getMainWin() {
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

