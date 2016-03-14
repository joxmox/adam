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

#include "log4cxx/logger.h"


#include "Buffer.hpp"
#include "Curse.hpp"
#include "Parse.hpp"


using namespace std;
using namespace log4cxx;

class Editor;

using funcFun = void (*)(Editor*, const vector<string>&);
using funcVec = vector<funcFun>;

class Editor {
	const string RECORD_VERSION = "1,1";
	Curse* tty = nullptr;
	Win* cmdWin = nullptr;
	Win* messWin = nullptr;
	Buffer* buf = nullptr;
	string bufName;
	map<string, Buffer*> bufMap;
	string fileName;
	string oInput;
	string oRecord;
	bool oReadOnly;
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
	static log4cxx::LoggerPtr logger;
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
	void startLearn();
	void remember();
	void doLearned();
	static void cbIllegalChar(Editor* ed, const vector<string>& params);
	static void cbNormChar(Editor* ed, const vector<string>& params);
	static void cbExit(Editor* ed, const vector<string>& params);
	static void cbQuit(Editor* ed, const vector<string>& params);
	static void cbDebug(Editor* ed, const vector<string>& params);
	static void cbMoveUp(Editor* ed, const vector<string>& params);
	static void cbMoveDown(Editor* ed, const vector<string>& params);
	static void cbMoveLeft(Editor* ed, const vector<string>& params);
	static void cbMoveRight(Editor* ed, const vector<string>& params);
	static void cbReturn(Editor* ed, const vector<string>& params);
	static void cbBackSpace(Editor* ed, const vector<string>& params);
	static void cbGotoSol(Editor* ed, const vector<string>& params);
	static void cbGotoEol(Editor* ed, const vector<string>& params);
	static void cbStartLearn(Editor* ed, const vector<string>& params);
	static void cbRemember(Editor* ed, const vector<string>& params);
	static void cbDoLearned(Editor* ed, const vector<string>& params);
	static void cbPageUp(Editor* ed, const vector<string>& params);
	static void cbPageDown(Editor* ed, const vector<string>& params);
	static void cbKillLine(Editor* ed, const vector<string>& params);
	static void cbPaste(Editor* ed, const vector<string>& params);
	static void parseError(Editor* ed, int errCode);
	static void cbDo(Editor* ed, const vector<string>& params);
	static void cbWriteFile(Editor* ed, const vector<string>& params);
	static void cbGotoLine(Editor* ed, const vector<string>& params);
	static void cbGotoMark(Editor* ed, const vector<string>& params);
	static void cbSetMark(Editor* ed, const vector<string>& params);

public:
	Editor(const string& fileName, const string& input, const string& record, bool readOnly);
	~Editor();
	void edit();
	int getKey();
	Buffer* getBuffer();
	void exit();
	void quit();
	void insertChar();
	void debug();
	string getCommand();
	void setCommand(const string& cmd);
};




#endif /* DOMAIN_EDITOR_HPP_ */
