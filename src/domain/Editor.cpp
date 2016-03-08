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
#include <chrono>
#include <thread>

#include "log4cxx/logger.h"

#include "Buffer.hpp"
#include "Curse.hpp"
#include "Editor.hpp"
#include "Screen.hpp"
#include "stringUtils.hpp"

using namespace std;

LoggerPtr Editor::logger{Logger::getLogger("Editor")};

Editor::Editor(const string& fileName) {
	this->fileName = fileName;
	LoggerPtr logger{Logger::getLogger("Editor")};
	LOG4CXX_DEBUG(logger, "instance created. filename is " << fileName);
}

Editor::~Editor() {
	LOG4CXX_DEBUG(logger, "shutting down");
	if (recFlag) {
		LOG4CXX_DEBUG(logger, "saving record buffer");
		saveRecord();
	}
	delete buf;
	delete cmdWin;
	delete messWin;
	if (tty != nullptr) delete tty;
}

void Editor::saveRecord() {
	ofstream ofs {recFile};
	if (!ofs) throw runtime_error("could not open " + recFile + " for output");
	vector<int>::iterator it = recBuf.begin();
	while (it < recBuf.end()) {
		string txt = "string(\"";
		if (*it >= 32 && *it <= 126) {
			if (*it == '"' || *it == '\\') txt.append("\\");
			txt.append(1, *it);
			while (it + 1 != recBuf.end() && *(it + 1) >= 32 && *(it + 1) <= 126) {
				it++;
				if (*it == '"' || *it == '\\') txt.append("\\");
				txt.append(1, *it);
			}
			txt += "\")";
		} else {
			int key = *it;
			txt = func2name[key];
			int count = 0;
			while (it + 1 != recBuf.end() && *(it + 1) == key) {
				count++;
				it++;
			}
			if (count > 1) {
				txt += "(" + to_string(count) + ")";
			} else {
				txt += "()";
			}
		}
		ofs << txt << endl;
		it++;	}
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

void Editor::edit(const string& input, const string& record) {
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
		LOG4CXX_DEBUG(logger, "reading simulated keys from file " << input);
		ifstream ifs {input};
		if (!ifs) throw invalid_argument("cannot open input file " + input);
		string line;
		while (getline(ifs, line)) {
			if (!line.empty()) {
				if (line.back() != ')') line.append("()");
				vector<string> vec = str::match(line, R"(^(.*?)\(\"?(.*?)\"?\)$)");
				if (vec.size() != 3) throw logic_error("cannot parse input file - " + line);
				string func {vec[1]};
				if (func == "string") {
					string::iterator it {vec[2].begin()};
					while (it != vec[2].end()) {
						if (*it == '\\') it++;
						inKeys.push_back(*(it++));
					}
				} else {
					int cnt;
					int key = name2func[func];
					if (vec[2].empty()) {
						cnt = 1;
					} else {
					  cnt = stoi(vec[2]);
					}
					for (int i = 0; i < cnt; i++) inKeys.push_back(key);
				}
			}
		}
	}

	if (!record.empty()) {
		recFile = record;
		LOG4CXX_DEBUG(logger, "session input will be saved to file " << recFile);
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
			this_thread::sleep_for(chrono::milliseconds(25));
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
		if (recFlag) recBuf.push_back(key);
		disMap[key](this);
	}
}

void Editor::setDisp(const string& name, int key, funcFun f) {
	disMap[key] = f;
	func2name[key] = name;
	name2func[name] = key;
}

void Editor::setDisp(const string& name, int key1, int key2, funcFun f) {
	for (int i = key1; i <= key2; i++) {
	    setDisp(name, i, f);
	}
}


void Editor::initDispatch() {
	disMap = funcVec {1000, cbIllegalChar};
	setDisp("debug", 4, cbDebug);
	setDisp("eol", 5, cbGotoEol);
	setDisp("sol", 8, cbGotoSol);
	setDisp("kill", 11, cbKillLine);
	setDisp("learn", 12, cbStartLearn); //FIXME: Temporary, should (also?) be a do command
	setDisp("cr", 13, cbReturn);
	setDisp("pasteBuffer", 16, cbPaste);
	setDisp("remember", 18, cbRemember);
	setDisp("quit", 24, cbQuit);
	setDisp("exit", 26, cbExit);
	setDisp("char", 32, 126, cbNormChar);
	setDisp("down", 258, cbMoveDown);
	setDisp("up", 259, cbMoveUp);
	setDisp("left", 260, cbMoveLeft);
	setDisp("right", 261, cbMoveRight);
	setDisp("delete", 263, cbBackSpace);
	setDisp("pgdown", 338, cbPageDown);
	setDisp("pgup", 339, cbPageUp);
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
		if (recFlag) recBuf.push_back(key);
		if (key != 13) {
			learnBuf.back().pop_back(); // remove last char, which is ctrl-r
			learnMap[key] = learnBuf.size() - 1;
			setDisp("recall", key, cbDoLearned);
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



