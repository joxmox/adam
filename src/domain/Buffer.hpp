/*
 * Buffer.hpp
 *
 *  Created on: Oct 21, 2015
 *      Author: sebis
 */

#pragma once

#include <vector>
#include <fstream>
#include <unordered_map>

#include "logging.hpp"
#include "Curse.hpp"
#include "Screen.hpp"

using namespace std;

class Buffer {
public:
	const string eobStr = "[End of file]";
private:
	int row = 0;
	int col = 0;
	string bufName;
	string fileName;
	Screen* scr;
	unordered_map<string, pair<int, int>> markMap;
	bool readOnly = false;
	bool modified = false;
	vector<pair<int, int>> posStack;
	string stsWrite = "Write";
	string stsInsert = "Insert";
	string stsDirection = "Forward";
	vector<string> data = {{eobStr}};
	bool selectActive = false;
	vector<string> pasteBuf;
	bool firstKill = true;
#ifdef LOG4CXX
	static log4cxx::LoggerPtr logger;
#endif
//	Win* mainWin = nullptr;
//	Win* stsWin = nullptr;
//	Win* messWin = nullptr;
//	Win* cmdWin = nullptr;
	void updateStatus();
	void printMessage(const string& str);
	void adjustBuffer(int type);
	bool markModified();
	void gotoAbs(int line);
	void gotoRel(int offset);

public:
	Buffer(const string& bufName, const string& fileName = "", Screen* scr = nullptr , bool readOnly = false);
	~Buffer();
	int readFile(const string& fileName = "");
	bool fileExists();
	bool atTopRow();
	bool atBotRow();
	int getLines();
	string getFileName();
	void insertChar(int key);
	void dump();
	void setFocus();
	Screen* getScr();
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void insertBreak();
	void deleteChar();
	void gotoSol();
	void gotoEol();
	void pageUp();
	void pageDown();
	void push();
	void push(int r, int c);
	void pop();
	void killLine();
	void paste();
	void insertLine(const string& s);
	void saveToFile(const string& fileName);
	string readCommand();
	void commandError(const string& cmd, int errCode);
	bool isModified();
	void gotoLine(const string& line);
	void gotoMark(const string& mark);
	void setMark(const string& mark);
	void debug();
	void where();
	void gotoExtreme(int x);
	bool inView(int r = -1);
};

