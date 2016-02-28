/*
 * Screen.hpp
 *
 *  Created on: Oct 20, 2015
 *      Author: joxmox
 */

#pragma once

#include <iostream>

#include "Tty.hpp"

using namespace std;

class Window {
public:
	enum scrMode {full, upper, lower};
private:
	int row = 0;
	int col = 0;
	int width;
	int height;
	int anchorRow;
	int anchorCol;
	static int winCnt;
	int winId = 0;
	int stsWin;
	Tty* tty;
	scrMode mode;
	string stsBuf = "MAIN";
	string stsWrt = "Write";
	string stsIns = "Insert";
	string stsDir = "Forward";
	void moveTty(int row, int col);
public:
	Window(Tty* tty, scrMode mode = full);
	void initScreen();
	void printStatus();
	void printChar(int key);
	void position();
	void moveRight();
	void moveLeft();
	void moveUp();
	void moveDown();
	void breakLine();
	void deleteChar();
};

std::ostream& operator <<(std::ostream&, const Window);

