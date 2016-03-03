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
	const string eobStr = "[End of buffer]";
	int topLine = 0;
	int maxLine = 0;
	int row = 0;
	int col = 0;
	string stsWrite = "Write";
	string stsInsert = "Insert";
	string stsDirection = "Forward";
	string fileName;
	string bufName;
	vector<string> data;
	bool selectActive = false;
	Screen* scr;
	static log4cxx::LoggerPtr logger;
//	Win* mainWin = nullptr;
//	Win* stsWin = nullptr;
//	Win* messWin = nullptr;
//	Win* cmdWin = nullptr;
	void updateStatus();
	void printMessage(const string& str);
	void adjustBuffer();

public:
	Buffer(const string& bufName, const string& fileName = "", Screen* scr = nullptr);
	~Buffer();
	int readFile(const string& fileName = "");
	bool fileExists();
	bool atTopRow();
	bool atBotRow();
	int getLines();
	void insertChar(int key);
	void dump();
	void setFocus();
	Screen* getMainWin();
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
};

