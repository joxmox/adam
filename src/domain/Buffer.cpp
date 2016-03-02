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

Buffer::Buffer(const string& bufName, const string& fileName, Screen* scr): fileName(fileName), bufName(bufName), scr(scr) {
	LoggerPtr logger{Logger::getLogger("Buffer")};
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
	maxLine = -1;
	topLine = 0;
	curLine = 0;
}

void Buffer::updateStatus() {
	scr->setStatus(" Buffer: " + bufName, "| " + stsWrite + " | " + stsInsert + " | " + stsDirection + " ");
}

void Buffer::printMessage(const string& str) {
	scr->printMessage(str);
}

int Buffer::readFile(const string& fileName) {
	LoggerPtr logger{Logger::getLogger("Buffer.readFile")};
	string readFile = fileName;
	if (readFile.empty()) readFile = this->fileName;
	LOG4CXX_DEBUG(logger, "loading file " << readFile << " into buffer");
	ifstream inf{readFile};
	maxLine = 0;
	if (inf) {
		string line;
		while (getline(inf, line)) {
			data.push_back(line);
			scr->printStr(line);
			scr->moveDown();
			maxLine++;
			col = 0;
			row++;
		}
		printMessage(to_string(maxLine) + " lines read from file " + this->fileName);
	} else {
		printMessage("Editing new file.  Could not find: " + this->fileName);
		data.push_back(string {});
	}
	updateStatus();
	return maxLine;
}

bool Buffer::fileExists() {
	ifstream inf {fileName};
	return bool(inf);
}

int Buffer::getLines() {
	return maxLine;
}

void Buffer::adjustBuffer() {
	LoggerPtr logger{Logger::getLogger("Buffer.adjustBuffer")};
	if (row > data.size()) {
		LOG4CXX_ERROR(logger, "cursor way too far down - should not happen");
		throw logic_error("error");
	}
	if (row == data.size()) {
		LOG4CXX_DEBUG(logger, "cursor one row down - adding line to buffer");
		data.push_back(string { });
	}
}

void Buffer::insertChar(int key) {
	LoggerPtr logger{Logger::getLogger("Buffer.insertChar")};
	LOG4CXX_DEBUG(logger, "inserting char: " << key);
	adjustBuffer();
	scr->insertChar(key);
	scr->move(0, 1);
	data[row].insert(col++, string(1, key));
}

void Buffer::moveUp() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveUp")};
	LOG4CXX_TRACE(logger, "enter");
	if (row > 0) {
		scr->moveUp(data[--row]);
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveDown() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveDown")};
	LOG4CXX_TRACE(logger, "enter");
	if (row < data.size()) {
		scr->moveDown(data[++row]);
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveLeft() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveLeft")};
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
	LoggerPtr logger{Logger::getLogger("Buffer.moveRight")};
	LOG4CXX_TRACE(logger, "enter");
	col++;
	scr->move(0, 1);
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
	LoggerPtr logger{Logger::getLogger("Buffer.deleteChar")};
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

void Buffer::dump() {
	LoggerPtr logger{Logger::getLogger("Buffer.dump")};
	LOG4CXX_DEBUG(logger, "row: " << row);
	LOG4CXX_DEBUG(logger, "col: " << col);
	int c {0};
	for (auto d : data) {
		LOG4CXX_DEBUG(logger, c++ << ": " << d);
	}
}

void Buffer::setFocus() {
	LoggerPtr logger{Logger::getLogger("Buffer.focus")};
	LOG4CXX_DEBUG(logger, "giving focus to screen main window, id=" << scr->getId());
	scr->refresh();
}

Win* Buffer::getMainWin() {
	return scr;
}



