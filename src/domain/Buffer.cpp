/*
 * Buffer.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: sebis
 */

#include <logging.hpp>
#include <string>
#include <stdexcept>
#include <fstream>

#include "logging.hpp"
#include "Buffer.hpp"
#include "Screen.hpp"

using namespace std;

#ifdef LOG4CXX
LoggerPtr Buffer::logger{Logger::getLogger("Buffer")};
#endif

Buffer::Buffer(const string& bufName, const string& fileName, Screen* scr, bool readOnly):
		bufName(bufName), fileName(fileName), scr(scr), readOnly(readOnly) {
	LOG4CXX_DEBUG(logger, "new buffer created: " << bufName);
	if (readOnly) {
		stsWrite = "Read-only";
		stsInsert = "Unmodifiable";
	}
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
	if (markModified()) {
		LOG4CXX_DEBUG(logger, "inserting char: " << key);
		adjustBuffer(1);
		scr->insertChar(key);
		data[row].insert(col++, string(1, key));
	}
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
	if (markModified()) {
		adjustBuffer(2);
		string tmp = data[row].substr(col);
		data[row].erase(col, string::npos);
		scr->insertBreak(tmp);
		data.insert(data.begin() + ++row, tmp);
		col = 0;
	}
}


void Buffer::deleteChar() {
	LOG4CXX_TRACE(logger, "enter");
	if (markModified()) {
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
		}
	}
	LOG4CXX_TRACE(logger, "exit");
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
		if (markModified()) {
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
}

void Buffer::paste() {
	if (!pasteBuf.empty() && markModified()) {
		for (auto p : pasteBuf) {
			LOG4CXX_DEBUG(logger, "pasting: " << p);
			insertLine(p);
			firstKill = true;
		}
	}
}

void Buffer::insertLine(const string& s) {
	if (markModified()) {
		scr->insertLine(s);
		data.insert(data.begin() + row, s);
		moveDown();
	}
}

void Buffer::saveToFile(const string& fileName) {
	ofstream of {fileName};
	if (!of) {
		LOG4CXX_FATAL(logger, "could not open file " << " for saving");
		throw runtime_error("could not open file for saving: " + fileName);
	}
	string tmp = data.back();
	data.pop_back();
	int count = 0;
	for (auto d : data) {
		of << d << endl;
		count++;
	}
	data.push_back(tmp);
	string lines = "lines";
	if (count == 1) lines = "line";
	scr->printMessage(to_string(count) + " " + lines + " written to file " + fileName);
}

string Buffer::readCommand() {
	return scr->readCommand();
}

void Buffer::commandError(const string& cmd, int errCode) {
	switch(errCode) {
	case 1:
		scr->printWarning("No command given.");
		break;
	case 2:
		scr->printWarning("Ambiguous command name: " + cmd);
		break;
	case 3:
		scr->printWarning("Don't understand command: " + cmd);
		break;
	default:
		scr->printWarning("ERROR: unexpected error code received from Parse");

	}
}

bool Buffer::isModified() {
	return modified;
}


string Buffer::getFileName() {
	return fileName;
}

bool Buffer::markModified() {
	if (readOnly) {
		scr->printWarning("Attempt to change unmodifiable buffer " + bufName);
	} else {
		modified = true;
	}
	return (!readOnly);
}

void Buffer::gotoRel(int toMove) {
	LOG4CXX_TRACE(logger, "enter");
	firstKill = true;
	int screenRow = scr->getRow();
	LOG4CXX_DEBUG(logger, "rows to move: " << toMove);
	int maxRow = data.size() - 1;
	int maxScreen = scr->getHeight() - 1;
	int newRow;
	if (toMove < 0) {
		int topRow = row - screenRow + toMove;
		newRow = row + toMove;
		if (topRow < 0) {
			screenRow += topRow;
		}
		if (newRow < 0) {
			screenRow -= newRow;
			newRow = 0;
		}
		if (screenRow < 0) screenRow = 0;
	} else {
		int botRow = row + scr->getHeight() - screenRow + toMove;
		newRow = row + toMove;
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
	}
	LOG4CXX_DEBUG(logger, "new buffer row: " << newRow << " (" << row << ")");
	LOG4CXX_DEBUG(logger, "new screen row: " << screenRow << " (" << scr->getRow() << ")");
	row = newRow;
	scr->repaint(data, row, screenRow);
	LOG4CXX_TRACE(logger, "exit");

}

void Buffer::gotoAbs(int line) {
	int diff = line - row;
	if (diff == 0) return;
	if (inView(line)) {
		scr->moveUp(diff); //FIXME: moveVertical()
		scr->setCol(0);
		row = line;
		col = 0;
	} else {
		int maxScrRow = scr->getHeight() - 1;
		int maxBufRow = data.size() - 1;
		if (diff > 0) {
			if (maxBufRow - line <= maxScrRow) {
				scr->repaint(data, line, maxScrRow - maxBufRow - line - 1); //FIXME: why not the last line?
			}
		}
		//FIXME: fixa kod!
	}
	gotoRel(line - row);
}

void Buffer::gotoLine(const string& line) {
	try {
		int iLine = stoi(line) - 1;
		if (iLine > data.size() - 1) {
			scr->printMessage("Buffer has only " + to_string(data.size() -1) + " lines.  (Now going to End of Buffer).");
			iLine = data.size() - 1;
		}
		if (iLine < 0) {
			scr->printMessage("Line number must be positive.  (Now going to Start of Buffer).");
			iLine = 0;
		}
		gotoAbs(iLine);
	} catch (invalid_argument& e) {
		scr->printMessage("Line expects a number for argument number: 1");
	}
}

void Buffer::gotoMark(const string& mark) {
	pair<int, int> p = markMap[mark];
	LOG4CXX_DEBUG(logger, "mark" + mark + ", position " + to_string(p.first) + "," + to_string(p.second));
	row = p.first;
	gotoAbs(row);
	col = p.second;
	scr->setCol(col);
}


void Buffer::setMark(const string& mark) {
	markMap[mark] = {row, col};
	scr->printMessage("Current position marked as: " + mark);
}

void Buffer::debug() {
	LOG4CXX_DEBUG(logger, "position = " << row << "," << col);
}

void Buffer::where() {
	scr->printMessage("You are at line " + to_string(row + 1) + " in buffer " + bufName + ".");
}

void Buffer::gotoExtreme(int x) {
	if (x < 0) {
		gotoAbs(0);
	} else {
		gotoAbs(data.size() -1);
	}
	col= 0;
	scr->setCol(0);
}

bool Buffer::inView(int r) {
	if (r < 0) r = row;
	int s = scr->getRow();
	int t = row - s;
	int b = t + scr->getHeight() - 1;
	if (r >= t && r <= b) return true;
	return false;
}
