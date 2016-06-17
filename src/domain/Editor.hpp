/*
 * Editor.hpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#ifndef DOMAIN_EDITOR_HPP_
#define DOMAIN_EDITOR_HPP_

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <queue>

#include "logging.hpp"
#include "Buffer.hpp"
#include "Curse.hpp"
#include "Parse.hpp"


using namespace std;

class Editor;

using funcFun = void (*)(Editor*, const vector<string>&);
using funcVec = vector<funcFun>;

class Editor {
	const string RECORD_VERSION = "1.1";
	Curse* tty = nullptr;
	Win* cmdWin = nullptr;
	Win* messWin = nullptr;
	Buffer* buf = nullptr;
	string bufName;
	map<string, Buffer*> bufMap;
	string fileName;
	string oInput;
	string oRecord;
	bool oReadOnly = false;
	int oWait = 0;
	int key = 0;
	bool loop = true;
	bool learnFlag = false;
	vector<vector<int>> learnBuf;
	map<int, int> learnMap;
	vector<int> inKeys;
	bool recFlag = false;
	string recFile;
	vector<int> recBuf;
	funcVec disMap;
	map<string, int> name2func;
	map<int, string> func2name;
#ifdef LOG4CXX
	static log4cxx::LoggerPtr logger;
#endif
	string command;
	static Parse<Editor>* parse;
	string getBufferName(const string& fileName);
	void mainLoop();
	void readReplay(const string& replay);
	void readReplayVer1(istream& ifs);
	void initDispatch();
	void saveRecord();
	void save2();
	void setDisp(const string& name, int key, funcFun f);
	void setDisp(const string& name, int key1, int key2, funcFun f);
	void abort(const string& str);

public:
	Editor(const string& fileName);
	~Editor();
	void setReplay(const string &input);
	void setRecord(const string &record);
	void setReadOnly();
	void setWait(int milli);
	void edit();
	int getKey();
	Buffer* getBuffer();
	string getCommand();
	void setCommand(const string& cmd);
	void illegalChar();
	void normalChar();
	void exit();
	void quit();
	void debug();
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void insertBreak();
	void backSpace();
	void gotoSol();
	void gotoEol();
	void startLearn();
	void remember();
	void doLearned();
	void pageUp();
	void pageDown();
	void killLine();
	void insert();
	void parseError(int errCode);
	void doCommand();
	void writeFile(const vector<string>& params);
	void gotoLine(const string& line);
	void gotoMark(const string& mark);
	void setMark(const string& mark);
	void where();
	void gotoTop();
	void gotoBot();

};




#endif /* DOMAIN_EDITOR_HPP_ */
