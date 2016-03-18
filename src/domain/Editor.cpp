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
#include "cbFuncs.hpp"

using namespace std;
using namespace log4cxx;

LoggerPtr Editor::logger{Logger::getLogger("Editor")};
Parse<Editor>* Editor::parse;


Editor::Editor(const string& fileName, const string& oInput, const string& oRecord, bool oReadOnly):
		fileName(fileName), oInput(oInput), oRecord(oRecord), oReadOnly(oReadOnly) {
	LoggerPtr logger{Logger::getLogger("Editor")};
	LOG4CXX_DEBUG(logger, "instance created. filename is " << fileName);
}

Editor::~Editor() {
	LOG4CXX_DEBUG(logger, "shutting down");
	if (recFlag) {
		LOG4CXX_DEBUG(logger, "saving record buffer");
		saveRecord();
	}
	if (buf) delete buf;
	if (cmdWin) delete cmdWin;
	if (messWin) delete messWin;
	if (tty != nullptr) delete tty;
}

void Editor::saveRecord() {
	ofstream ofs {recFile};
	if (!ofs) abort("could not open recording file" + recFile + " for output");
	ofs << "version(" << RECORD_VERSION << ")" << endl;
	vector<int>::iterator it = recBuf.begin();
	string txt;
	while (it < recBuf.end()) {
		if (*it >= 32 && *it <= 126) {
			if (*it == '(' || *it == ')' || *it == '\\') txt.push_back('\\');
			txt = "text(" + string(1, *it++);
			while (it != recBuf.end() && *it >= 32 && *it  <= 126) {
				if (*it == '(' || *it == '\\' || *it == ')') txt.push_back('\\');
				txt.push_back(*it++);
			}
			txt.push_back(')');
		} else {
			int key = *it++;
			txt = func2name[key];
			int count = 1;
			while (it != recBuf.end() && *it == key) {
				count++;
				it++;
			}
			if (count > 1) {
				txt += "(" + to_string(count) + ")";
			}
		}
		ofs << txt << endl;
	}
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
	bool ok = false;
	if (!buf->isModified()) {
		ok = true;
		loop = false;
	}
	while (!ok) {
		string ans = str::lowercase(buf->getScr()->readCommand("Buffer modifications will not be saved, continue quitting [Yes]? "));
		if (ans == "" || ans == "y" || ans == "ye" || ans == "yes") {
			loop = false;
			ok = true;
		} else if (ans == "n" || ans == "no") {
			ok = true;
		} else {
			buf->getScr()->printWarning("Don't understand " + ans + "  Please answer Yes or No.");
		}
	}
	buf->getScr()->printCommand("");
}

void Editor::edit() {
	LOG4CXX_DEBUG(logger, "starting");

	initDispatch();

	tty = new Curse();
	Screen* scr = new Screen(tty, 0, 1, 1);
	messWin = scr->createMessWin();
	cmdWin = scr->createCmdWin();

	bufName = getBufferName(fileName);
	buf = new Buffer(bufName, fileName, scr, oReadOnly);
	bufMap[bufName] = buf;

	int sts = buf->readFile();
	if (sts >= 0) {
		LOG4CXX_DEBUG(logger, sts << " lines read from file");
	} else {
		LOG4CXX_DEBUG(logger, "editing new file");
	}

	if (!oInput.empty()) {
		LOG4CXX_DEBUG(logger, "replay option specified");
		readReplay(oInput);
	}

	if (!oRecord.empty()) {
		recFile = oRecord;
		LOG4CXX_DEBUG(logger, "session input will be saved to file " << recFile);
		recFlag = true;
	}
	mainLoop();
}

void Editor::readReplay(const string& input) {
	LOG4CXX_DEBUG(logger, "reading simulated keys from file " << input);
	ifstream ifs {input};
	if (!ifs) abort("cannot open replay file " + input + " for input");
	string line;
	if (getline(ifs, line)) {
		vector<string> vec = str::match(line, R"(^(.*?)\(\"?(.*?)\"?\)$)");
		if (vec.size() != 3 || vec[1] != "version") abort("invalid header record in replay file - " + line);
		string version = vec[2];
		LOG4CXX_DEBUG(logger, "replay file version = " << version);
		vec = str::split(version, "\\.");
		LOG4CXX_DEBUG(logger, vec[0]);
		int major = stoi(vec[0]);
		LOG4CXX_DEBUG(logger, "stoi done");
//		int minor = 0;
//		if (vec.size() > 1) minor = stoi(vec[1]);
		switch (major) {
		case 1:
			readReplayVer1(ifs);
			break;
		default:
			abort("unsupported replay file version - " + version);
		}
	}
}

void Editor::readReplayVer1(istream& ifs) {
	string line;
	while (getline(ifs, line)) {
		line = str::decomment(line, '#', '(');
		vector<string> lines {str::split(line, ";")};
		for (auto line : lines) {
			line = str::trim(line);
			if (!line.empty()) {
				if (line.back() != ')') line.append("()");
				vector<string> vec = str::match(line, R"(^\s*(.*?)\((.*?)\)\s*$)");
				if (vec.size() != 3) abort("cannot parse replay line in replay file - " + line);
				LOG4CXX_DEBUG(logger, vec[0]);
				LOG4CXX_DEBUG(logger, vec[1]);
				LOG4CXX_DEBUG(logger, vec[2]);

				string func {vec[1]};
				if (func == "text") {
					string::iterator it {vec[2].begin()};
					while (it != vec[2].end()) {
						if (*it == '\\') it++;
						inKeys.push_back(*(it++));
					}
				} else {
					int cnt;
					if (name2func.find(func) == name2func.end()) {
						LOG4CXX_ERROR(logger, "no function matches " << func);
						abort("invalid function name in replay file encountered - " + func);
					}
					int key = name2func[func];
					if (vec[2].empty()) {
						cnt = 1;
					} else {
						LOG4CXX_DEBUG(logger, vec[2]);
						try {
						cnt = stoi(vec[2]);
						} catch (invalid_argument& e) {
							abort("function " + func + " expects and integer argument");
						}
					}
					for (int i = 0; i < cnt; i++) inKeys.push_back(key);
				}
			}
		}
	}
}


string Editor::getBufferName(const string& fileName) {
	return fileName;
}

void Editor::mainLoop() {
	while (loop) {
		buf->setFocus();
		if (!inKeys.empty()) {
//			this_thread::sleep_for(chrono::milliseconds(20)); //TODO: Just for cosmetic reasons right now ;)
			key = inKeys.front();
			inKeys.erase(inKeys.begin());
			LOG4CXX_DEBUG(logger, "read key " << key << " from file - dispatching...");
			if (inKeys.empty()) LOG4CXX_DEBUG(logger, "input file processed  reverting to keyboard");
		} else  {
			LOG4CXX_TRACE(logger, "waiting for key from terminal");
			buf->debug();
			buf->getScr()->debug();
			key = buf->getScr()->readKey();
			LOG4CXX_DEBUG(logger, "read key " << key << " from keyboard - dispatching...");
		}
		if (learnFlag) learnBuf.back().push_back(key);
		if (recFlag) recBuf.push_back(key);
		disMap[key](this, {});
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
	setDisp("learn", 12, cbStartLearn);
	setDisp("cr", 13, cbInsertBreak);
	setDisp("insert", 16, cbInsert);
	setDisp("remember", 18, cbRemember);
	setDisp("quit", 24, cbQuit);
	setDisp("exit", 26, cbExit);
	setDisp("char", 32, 126, cbNormalChar);
	setDisp("down", 258, cbMoveDown);
	setDisp("up", 259, cbMoveUp);
	setDisp("left", 260, cbMoveLeft);
	setDisp("right", 261, cbMoveRight);
	setDisp("del", 263, cbBackSpace);
	setDisp("do", 276, cbDo);
	setDisp("pgdown", 338, cbPageDown);
	setDisp("pgup", 339, cbPageUp);
	LOG4CXX_DEBUG(logger, "dispatch tables initialized");
	map<string, Parse<Editor>::cbFun> funcTab {
		{"learn", cbStartLearn},
		{"quit", cbQuit},
		{"write", cbWriteFile},
		{"write *", cbWriteFile},
		{"line *", cbGotoLine},
		{"goto *", cbGotoMark},
		{"mark *", cbSetMark},
		{"line", cbWhere},
		{"where", cbWhere},
		{"top", cbGotoTop},
		{"bottom", cbGotoBot},
	};
	parse = new Parse<Editor> {this, funcTab, cbParseError};
	LOG4CXX_DEBUG(logger, "dispatch tables initialized");
}

void Editor::startLearn() {
	buf->getScr()->printMessage("Press keystrokes to be learned.  Press CTRL/R to remember these keystrokes.");
	learnBuf.push_back(vector<int> {});
	learnFlag = true;
}

void Editor::remember() {
	if (learnFlag) {
		bool ok = false;
		while (!ok) {
			buf->getScr()->printCommand("Press the key you want to use to do what was just learned: ");
			if (inKeys.empty()) {
				key = buf->getScr()->readKey();
			} else {
				key = inKeys.front();
				inKeys.erase(inKeys.begin());
			}
			if (key == 18) {
				buf->getScr()->printWarning("You cannot define the REMEMBER key for a learn sequence.");
			} else if (key >= 32 && key <= 126) {
				buf->getScr()->printWarning("You cannot define a typing key.");
			} else {
				buf->getScr()->printCommand("");
				ok = true;
			}
		}
		if (recFlag) recBuf.push_back(key);
		if (key != 13) {
			learnBuf.back().pop_back(); // remove last char, which is ctrl-r
			learnMap[key] = learnBuf.size() - 1;
			setDisp("recall", key, cbDoLearned);
			LOG4CXX_DEBUG(logger, "learned, buffer entry: " << learnBuf.size() - 1);
			buf->getScr()->printMessage("Key sequence remembered.");
		} else {
			buf->getScr()->printMessage("Key sequence not remembered.");
			LOG4CXX_DEBUG(logger, "learn sequence cancelled");
		}
		learnFlag = false;
	} else {
		LOG4CXX_DEBUG(logger, "not learning - no action");
		buf->getScr()->printMessage("Nothing to remember.");
	}
}

void Editor::doLearned() {
	int bufNum = learnMap[key];
	LOG4CXX_DEBUG(logger, "key: " << key << ", bufNum:" << bufNum);
	vector<int> apa = learnBuf[bufNum];
	for (auto a : apa) {
		LOG4CXX_DEBUG(logger, "playing back: " << a);
		key = a;
		disMap[a](this, {});
	}
}

void Editor::debug() {
	LOG4CXX_DEBUG(logger, "dumping buffer:");
	buf->dump();
	LOG4CXX_DEBUG(logger, "window info:");
	buf->getScr()->debug();
}

string Editor::getCommand() {
	return command;
}

void Editor::setCommand(const string& cmd) {
	command = cmd;
}

void Editor::illegalChar() {
	LOG4CXX_DEBUG(logger, "received unexpected character: " << key);
}

void Editor::normalChar() {
	buf->insertChar(key);
}

void Editor::moveUp() {
	buf->moveUp();
}

void Editor::moveDown() {
	buf->moveDown();
}

void Editor::moveLeft() {
	buf->moveLeft();
}

void Editor::moveRight() {
	buf->moveRight();
}

void Editor::insertBreak() {
	buf->insertBreak();
}

void Editor::backSpace() {
	buf->deleteChar();
}

void Editor::gotoSol() {
	buf->gotoSol();
}

void Editor::gotoEol() {
	buf->gotoEol();
}

void Editor::pageUp() {
	buf->pageUp();
}

void Editor::pageDown() {
	buf->pageDown();
}

void Editor::killLine() {
	buf->killLine();
}

void Editor::insert() {
	buf->paste();
}

void Editor::parseError(int errCode) {
	buf->commandError(command, errCode);
}
void Editor::doCommand() {
	command = buf->readCommand();
	parse->decode(command);
}

void Editor::writeFile(const vector<string>& params) {
	string fn;
	if (params.empty()) {
		fn = fileName;
	} else {
		fn = params[0];
	}
	buf->saveToFile(fn);
}

void Editor::gotoLine(const string& line) {
	buf->gotoLine(line);
}

void Editor::gotoMark(const string& mark) {
	buf->gotoMark(mark);
}

void Editor::setMark(const string& mark) {
	buf->setMark(mark);}

void Editor::where() {
	buf->where();
}

void Editor::gotoTop() {
	buf->gotoExtreme(-1);
}

void Editor::gotoBot() {
	buf->gotoExtreme(1);
}

void Editor::abort(const string& str) {
	tty->delWinAll();
	delete tty;
	tty = nullptr;
	throw logic_error(str);
}
