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

#include "Curse.hpp"

using namespace std;

class Buffer {
	const string eobStr = "[End of buffer]";
	int topLine;
	int maxLine;
	int curLine;
	int row = 0;
	int col = 0;
	string stsWrite = "Write";
	string stsInsert = "Insert";
	string stsDirection = "Forward";
	string fileName;
	string bufName;
	vector<string> data;
	bool selectActive = false;
	Win* mainWin = nullptr;
	Win* stsWin = nullptr;
	Win* messWin = nullptr;
	Win* cmdWin = nullptr;
	void updateStatus();
	void printMessage(const string& str);

public:
	Buffer(const string& bufName, const string& fileName = "", Win* mainWin = nullptr,
			Win* stsWin = nullptr, Win* messWin = nullptr, Win* cmdWin = nullptr);
	int readFile(const string& fileName = "");
	bool fileExists();
	int getLines();
	void insertChar(int key);
	void dump();
	void setFocus();
	Win* getMainWin();
};

