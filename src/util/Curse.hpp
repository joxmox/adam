#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "log4cxx/logger.h"


using namespace std;

class Win;
enum curseAttrs {attRev = 1, attBold = 2}; //TODO: this should go into class I guess...

/**
 * Curse is wrapper intended to hide the (to C++) ugly ncurses API. An application program should
 * create one instance of the Curse class. Calling the member function creWin() will return a Win object that
 * corresponds to a ncurses window. It is possible to use the Curse class without any sub windows, aka stdscr,
 * by calling getStdscr(), but mixing the two approaches is not encouraged.
 * In either case, no functions, other than one of the two mentioned, should be used explicitly
 * from the application program, but rather called implicitly by the corresponding Win function.
 */
class Curse {
	int height;
	int width;
	vector<void*> winMap;
	static log4cxx::LoggerPtr logger;
	static mutex gMaster;
public:
	Curse();
	~Curse();



	/**
	 * The normal way to construct Win object. In some cases you will create the window
	 * object yourself, such as in a inherited class. The the subclass should u
	 * @param height Height of Window
	 * @param width Width of Window
	 * @param row Row where to anchor the window to the screen
	 * @param col Column where to anchor the window to the screen
	 */
	Win* creWin(int height, int width, int row, int col);

	/** If the Win object is already created we still need to create a curses WINDOW object
	 * without creating an extra Win instance. This is the function to use then.
	 * @param height Height of Window
	 * @param width Width of Window
	 * @param row Row where to anchor the window to the screen
	 * @param col Column where to anchor the window to the screen
	 */
	int addWin(int height, int width, int row, int col);

	/** This function deletes a Win object and its associated curses Window.
	 * Right now only the curses Window is deleted...
	 * @param id Window identification
	 */
	void delWin(int id);
	void delWinAll();
	void pos(int id, int row, int col);
	void insertChar(int id, char c);
	void printStr(int id, const string& s);
	void printStr(int id, char s);
	int getHeight();
	int getWidth();
	int readKey(int id);
	string readString(int id);
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
	static log4cxx::LoggerPtr logger;
public:
	Win(Curse* cur, int height, int width, int row, int col);
	Win(int height, int width, int row, int col, int id, Curse* curse);
	~Win();
	int getId();
	int getHeight();
	int getWidth();
	void pos(int row, int col);
	void move(int rows, int cols);
	void moveUp(int r = 1);
	void moveDown(int r = 1);
	void moveRight(int c = 1);
	void moveLeft(int c = 1);
	void moveLeft(const string& str);
	void moveRight(const string& str);
	int getCol();
	int getRow();
	void setCol(int col);
	void setRow(int row);
	void insertChar(char c);
	void printStr(const string& s);
	void printStr(char s);
	void refresh();
	int readKey();
	string readString();
	void setAttr(curseAttrs attr);
	void clearAttr(curseAttrs attr);
	void clearEol();
	void insertLine();
	void debug();
	void delChar();
	void delLine();
};
