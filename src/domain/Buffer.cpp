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

using namespace std;
using namespace log4cxx;

Buffer::Buffer(const string& bufName, const string& fileName, Win* mainWin, Win* stsWin, Win* messWin, Win* cmdWin):
		fileName(fileName), bufName(bufName), mainWin(mainWin), stsWin(stsWin), messWin(messWin), cmdWin(cmdWin) {
	LoggerPtr logger{Logger::getLogger("Buffer")};
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
	maxLine = -1;
	topLine = 0;
	curLine = 0;
}

void Buffer::updateStatus() {
	string preStr { " Buffer: " + bufName };
	string postStr { "| " + stsWrite + " | " + stsInsert + " | " + stsDirection + " " };
	stsWin->pos(0, 0);
	stsWin->setAttr(attRev);
	stsWin->print(preStr + string(stsWin->getWidth() - preStr.size() - postStr.size(), ' ') + postStr);
	stsWin->clearAttr(attRev);
	stsWin->refresh();
}

void Buffer::printMessage(const string& str) {
	messWin->pos(0, 0);
	messWin->print(str);
	messWin->refresh();
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
			mainWin->pos(row , col);
			mainWin->print(line);
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

void Buffer::insertChar(int key) {
	LoggerPtr logger{Logger::getLogger("Buffer.insertChar")};
	LOG4CXX_DEBUG(logger, "inserting char: " << key);
	if (row > data.size()) {
		LOG4CXX_ERROR(logger, "cursor way too far down - should not happen");
		throw logic_error("error");
	}
	if (row == data.size()) {
		LOG4CXX_DEBUG(logger, "cursor one row down - adding line to buffer");
		data.push_back(string {});
	}
	mainWin->pos(row, col);
	mainWin->print(key);
	data[row].insert(col++, string(1, key));
	mainWin->pos(row, col);
}

void Buffer::moveUp() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveUp")};
	LOG4CXX_TRACE(logger, "enter");
	if (row > 0) row--;
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveDown() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveDown")};
	LOG4CXX_TRACE(logger, "enter");
	row++;
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveLeft() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveLeft")};
	LOG4CXX_TRACE(logger, "enter");
	if (col > 0) {
		col--;
	} else {
		printMessage("Already at start of line."); //FIXME: check text with eve
	}
	LOG4CXX_TRACE(logger, "exit");
}

void Buffer::moveRight() {
	LoggerPtr logger{Logger::getLogger("Buffer.moveRight")};
	LOG4CXX_TRACE(logger, "enter");
	col++;
	LOG4CXX_TRACE(logger, "exit");
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
	LOG4CXX_DEBUG(logger, "giving focus to mainWin, id=" << mainWin->getId());
	mainWin->pos(row, col);
	mainWin->refresh();
}

Win* Buffer::getMainWin() {
	return mainWin;
}



