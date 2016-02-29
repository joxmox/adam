#pragma once

#include <map>
#include <fstream>


class Curse {
	int height;
	int width;
	int cmdRow;
	int messRow;
public:
	Curse();
	~Curse();
	void pos(int row, int col);
	void print(char c);
	int getHeight();
	int getWidth();
	int readKey();

};
