#pragma once

#include <map>
#include <vector>
#include <fstream>

using namespace std;

class Win;

class Curse {
	int height;
	int width;
	vector<void*> winMap;
public:
	Curse();
	~Curse();
	Win* creWin(int height, int width, int row, int col);
	void delWin(int id);
	void pos(int id, int row, int col);
	void print(int id, char c);
	void print(int id, const string& s);
	int getHeight();
	int getWidth();
	int readKey(int id);
	void refresh(int id = -1);
};

class Win {
	int height;
	int width;
	int row;
	int col;
	int id;
	Curse* curse;
public:
	Win(int height, int width, int row, int col, int id, Curse* curse);
	~Win();
	int getId();
	void pos(int row, int col);
	void print(char c);
	void print(const string& s);
	void refresh();
	int readKey();
};
