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

#include "Window.hpp"

using namespace std;

class Buffer {
	int topLine;
	int maxLine;
	int curLine;
	int row = 0;
	int col = 0;
	string fileName;
	string bufName;
	vector<string> data;
	bool selectActive = false;
	Window* win = nullptr;
public:
	Buffer(const string& bufName, const string& fileName = "", Window* = nullptr);
	int readFile(const string& fileName = "");
	bool fileExists();
	int getLines();
	void insertChar(int key);
	void dump();
};

