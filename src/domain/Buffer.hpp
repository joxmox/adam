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

#include "log4cxx/logger.h"

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
	static log4cxx::LoggerPtr logger;
//	Win* mainWin = nullptr;
//	Win* stsWin = nullptr;
//	Win* messWin = nullptr;
//	Win* cmdWin = nullptr;
	void updateStatus();
	void printMessage(const string& str);
	void adjustBuffer(int type);
	bool markModified();

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
};

