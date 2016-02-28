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

Buffer::Buffer(const string& bufName, const string& fileName, Window* win): win(win) {
	LoggerPtr logger{Logger::getLogger("Buffer")};
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
	this->bufName = bufName;
	if (!fileName.empty()) {
		this->fileName = fileName;
		readFile();
	}
	maxLine = -1;
	topLine = 0;
	curLine = 0;
}

int Buffer::readFile(const string& fileName) {
	LoggerPtr logger{Logger::getLogger("Buffer.readFile")};
	string readFile = fileName;
	if (readFile.empty()) readFile = this->fileName;
	LOG4CXX_DEBUG(logger, "loading file " << readFile << " into buffer");
	ifstream inf{readFile};
	if (inf) {
		string line;
		while (getline(inf, line)) {
			data.push_back(line);
			maxLine++;
			col = line.size();
			row++;
		}
		--row;
		return maxLine + 1;
	}
	return 0;
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
	data[row].insert(col++, string(1, key));
	win->printChar(key);
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


