/*
 * Editor.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#include <string>
#include <map>
#include <stdexcept>
#include <functional>

#include "log4cxx/logger.h"

#include "Buffer.hpp"
#include "Curse.hpp"
#include "Editor.hpp"
#include "Screen.hpp"

using namespace std;

LoggerPtr Editor::logger{Logger::getLogger("Editor")};

Editor::Editor(const string& fileName) {
	this->fileName = fileName;
	LoggerPtr logger{Logger::getLogger("Editor")};
	LOG4CXX_DEBUG(logger, "instance created. filename is " << fileName);
}

Editor::~Editor() {
	LOG4CXX_DEBUG(logger, "shutting down");
	delete buf;
	delete cmdWin;
	delete messWin;
	if (tty != nullptr) delete tty;
}

int Editor::getKey() {
	return key;
}

Buffer* Editor::getBuffer() {
	return buf;
}

void Editor::exit() {
	loop = false;
	buf->saveToFile(fileName);
}

void Editor::quit() {
	loop = false;
}

void Editor::edit(const vector<int>& input, const string& record) {
	LOG4CXX_DEBUG(logger, "starting");

	initDispatch();

	tty = new Curse();
	Screen* scr = new Screen(tty, 0, 1, 1);
	messWin = scr->createMessWin();
	cmdWin = scr->createCmdWin();

	bufName = getBufferName(fileName);
	buf = new Buffer(bufName, fileName, scr);
	bufMap[bufName] = buf;

	int sts = buf->readFile();
	if (sts >= 0) {
		LOG4CXX_DEBUG(logger, sts << " lines read from file");
	} else {
		LOG4CXX_DEBUG(logger, "editing new file");
	}

	if (!input.empty()) {
		inKeys = input;
	}

	if (!record.empty()) {
		recFile.open(record);
		if (!recFile) throw runtime_error("could not open " + record + " for output");
		recFlag = true;
	}
	mainLoop();
}

string Editor::getBufferName(const string& fileName) {
	return fileName;
}

void Editor::mainLoop() {
	while (loop) {
		buf->setFocus();
		if (!inKeys.empty()) {
			key = inKeys.front();
			inKeys.erase(inKeys.begin());
			LOG4CXX_DEBUG(logger, "read key " << key << " from file - dispatching...");
			if (inKeys.empty()) LOG4CXX_DEBUG(logger, "input file processed  reverting to keyboard");
		} else  {
			LOG4CXX_TRACE(logger, "waiting for key from terminal");
			buf->getMainWin()->debug();
			key = buf->getMainWin()->readKey();
			LOG4CXX_DEBUG(logger, "read key " << key << " from keyboard - dispatching...");
		}
		if (learnFlag) learnBuf.back().push_back(key);
		if (recFlag) recFile << key << endl;
		disMap[key](this);
	}
}

void Editor::setDisp(int key, funcFun f) {
	disMap[key] = f;
}

void Editor::setDisp(int key1, int key2, funcFun f) {
	for (int i = key1; i <= key2; i++) {
	    setDisp(i, f);
	}
}


void Editor::initDispatch() {
	disMap = funcVec {1000, cbIllegalChar};
	setDisp(4, cbDebug);
	setDisp(5, cbGotoEol);
	setDisp(8, cbGotoSol);
	setDisp(11, cbKillLine);
	setDisp(12, cbStartLearn); //FIXME: Temporary, should (also?) be a do command
	setDisp(13, cbReturn);
	setDisp(16, cbPaste);
	setDisp(18, cbRemember);
	setDisp(24, cbQuit);
	setDisp(26, cbExit);
	setDisp(32, 126, cbNormChar);
	setDisp(258, cbMoveDown);
	setDisp(259, cbMoveUp);
	setDisp(260, cbMoveLeft);
	setDisp(261, cbMoveRight);
	setDisp(263, cbBackSpace);
	setDisp(338, cbPageDown);
	setDisp(339, cbPageUp);
	LOG4CXX_DEBUG(logger, "dispatch table initialized");
}

void Editor::startLearn() {
	buf->getMainWin()->printMessage("Press keystrokes to be learned.  Press CTRL/R to remember these keystrokes.");
	learnBuf.push_back(vector<int> {});
	learnFlag = true;
}

void Editor::remember() {
	if (learnFlag) {
		buf->getMainWin()->printMessage("Press the key you want to use to do what was just learned: ");
		int key;
		if (inKeys.empty()) {
			key = buf->getMainWin()->readKey();
		} else {
			key = inKeys.front();
			inKeys.erase(inKeys.begin());
		}
		if (recFlag) recFile << key << endl;
		if (key != 13) {
			learnBuf.back().pop_back(); // remove last char, which is ctrl-r
			learnMap[key] = learnBuf.size() - 1;
			setDisp(key, cbDoLearned);
			LOG4CXX_DEBUG(logger, "learned, buffer entry: " << learnBuf.size() - 1);
			buf->getMainWin()->printMessage("Key sequence remembered.");
		} else {
			buf->getMainWin()->printMessage("Key sequence not remembered.");
			LOG4CXX_DEBUG(logger, "learn sequence cancelled");
		}
		learnFlag = false;
	} else {
		LOG4CXX_DEBUG(logger, "not learning - no action");
		buf->getMainWin()->printMessage("Nothing to remember.");
	}
}

void Editor::doLearned() {
	int bufNum = learnMap[key];
	LOG4CXX_DEBUG(logger, "key: " << key << ", bufNum:" << bufNum);
	vector<int> apa = learnBuf[bufNum];
	for (auto a : apa) {
		LOG4CXX_DEBUG(logger, "playing back: " << a);
		key = a;
		disMap[a](this);
	}
}

void Editor::debug() {
	LOG4CXX_DEBUG(logger, "dumping buffer:");
	buf->dump();
	LOG4CXX_DEBUG(logger, "window info:");
	buf->getMainWin()->debug();
}

void Editor::cbIllegalChar(Editor* ed) {
	LOG4CXX_DEBUG(logger, "received unexpected character: " << ed->getKey());
}

void Editor::cbNormChar(Editor* ed) {ed->getBuffer()->insertChar(ed->getKey());}
void Editor::cbExit(Editor* ed) {ed->exit();}
void Editor::cbQuit(Editor* ed) {ed->quit();}
void Editor::cbDebug(Editor* ed) {ed->debug();}
void Editor::cbMoveUp(Editor* ed) {ed->getBuffer()->moveUp();}
void Editor::cbMoveDown(Editor* ed) {ed->getBuffer()->moveDown();}
void Editor::cbMoveLeft(Editor* ed) {ed->getBuffer()->moveLeft();}
void Editor::cbMoveRight(Editor* ed) {ed->getBuffer()->moveRight();}
void Editor::cbReturn(Editor* ed) {ed->getBuffer()->insertBreak();}
void Editor::cbBackSpace(Editor* ed) {ed->getBuffer()->deleteChar();}
void Editor::cbGotoSol(Editor* ed) {ed->getBuffer()->gotoSol();}
void Editor::cbGotoEol(Editor* ed) {ed->getBuffer()->gotoEol();}
void Editor::cbStartLearn(Editor* ed) {ed->startLearn();}
void Editor::cbRemember(Editor* ed) {ed->remember();}
void Editor::cbDoLearned(Editor* ed) {ed->doLearned();}
void Editor::cbPageUp(Editor* ed) {ed->getBuffer()->pageUp();}
void Editor::cbPageDown(Editor* ed) {ed->getBuffer()->pageDown();}
void Editor::cbKillLine(Editor* ed) {ed->getBuffer()->killLine();}
void Editor::cbPaste(Editor* ed) {ed->getBuffer()->paste();}



