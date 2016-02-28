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

using namespace std;

class Buffer {
	int topLine;
	int maxLine;
	int curLine;
	string file;
	string name;
	vector<string> data;
	bool selectActive = false;
public:
	Buffer(const string& buffName);
};

