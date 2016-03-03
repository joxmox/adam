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

#include "Buffer.hpp"
#include "Curse.hpp"



using namespace std;
using namespace log4cxx;

class Editor;

using funcFun = void (*)(Editor*);
using funcVec = vector<funcFun>;

class Editor {
	Curse* tty = nullptr;
	Win* cmdWin = nullptr;
	Win* messWin = nullptr;
	Buffer* buf = nullptr;
	string bufName;
	map<string, Buffer*> bufMap;
	string fileName;
	int key = 0;
	bool loop = true;
	bool learnFlag = false;
	vector<vector<int>> learnBuf;
	map<int, int> learnMap;
	funcVec disMap;
	string getBufferName(const string& fileName);
	void mainLoop();
	void initDispatch();
	void setDisp(int key, funcFun f);
	void setDisp(int key1, int key2, funcFun f);
	void startLearn();
	void remember();
	void doLearned();
	static void cbIllegalChar(Editor* ed);
	static void cbNormChar(Editor* ed);
	static void cbExit(Editor* ed);
	static void cbDebug(Editor* ed);
	static void cbMoveUp(Editor* ed);
	static void cbMoveDown(Editor* ed);
	static void cbMoveLeft(Editor* ed);
	static void cbMoveRight(Editor* ed);
	static void cbReturn(Editor* ed);
	static void cbBackSpace(Editor* ed);
	static void cbGotoSol(Editor* ed);
	static void cbGotoEol(Editor* ed);
	static void cbStartLearn(Editor* ed);
	static void cbRemember(Editor* ed);
	static void cbDoLearned(Editor* ed);
	static void cbPageUp(Editor* ed);
	static void cbPageDown(Editor* ed);

public:
	Editor(const string& fileName);
	~Editor();
	void edit();
	int getKey();
	Buffer* getBuffer();
	void quit();
	void insertChar();
	void debug();
};




#endif /* DOMAIN_EDITOR_HPP_ */
