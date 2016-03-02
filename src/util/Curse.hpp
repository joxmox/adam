#pragma once

#include <vector>

using namespace std;

class Win;
enum curseAttrs {attRev = 1, attBold = 2};


class Curse {
	int height;
	int width;
	vector<void*> winMap;
public:
	Curse();
	~Curse();
	Win* creWin(int height, int width, int row, int col);
	int addWin(int height, int width, int row, int col);
	void delWin(int id);
	void pos(int id, int row, int col);
	void insertChar(int id, char c);
	void printStr(int id, const string& s);
	int getHeight();
	int getWidth();
	int readKey(int id);
	void refresh(int id = -1);
	void setAttr(int id, curseAttrs attr);
	void clearAttr(int id, curseAttrs attr);
	void clearEol(int id);
	void insertLine(int id);
	void delChar(int id);
	void delLine(int id);
};

class Win {
protected:
	int height;
	int width;
	int row;
	int col;
	int id;
	Curse* curse;
public:
	Win(Curse* cur, int height, int width, int row, int col);
	Win(int height, int width, int row, int col, int id, Curse* curse);
	~Win();
	int getId();
	int getHeight();
	int getWidth();
	void pos(int row, int col);
	void move(int rows, int cols);
	void moveUp(const string& str = "");
	void moveDown(const string& str = "");
	void moveLeft(const string& str);
	void moveRight(const string& str);
	int getCol();
	int getRow();
	void setCol(int col);
	void setRow(int row);
	void insertChar(char c);
	void printStr(const string& s);
	void refresh();
	int readKey();
	void setAttr(curseAttrs attr);
	void clearAttr(curseAttrs attr);
	void clearEol();
	void insertLine();
	void debug();
	void delChar();
	void delLine();
};
