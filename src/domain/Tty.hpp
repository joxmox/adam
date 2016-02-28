#pragma once

#include <map>
#include <fstream>


class Tty {
	int height;
	int width;
	int cmdRow;
	int messRow;
public:
	Tty();
	~Tty();
	void pos(int row, int col);
	void print(char c);
	int getHeight();
	int getWidth();
	int readKey();

};
