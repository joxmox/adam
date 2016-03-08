/*
 * Screen.hpp
 *
 *  Created on: Mar 2, 2016
 *      Author: joxmox
 */

#ifndef DOMAIN_SCREEN_HPP_
#define DOMAIN_SCREEN_HPP_

#include <vector>
#include <string>
#include <mutex>


#include "log4cxx/logger.h"

#include "Curse.hpp"

class Screen : public Win {
	Curse* cur;
	Win* cmdWin;
	Win* messWin;
	Win* stsWin = nullptr;
	vector<pair<int, int>> posStack;
	static log4cxx::LoggerPtr logger;
	static mutex gMessage;
	static void asyncWarning(Screen* scr, const string& str);
	static void asyncMessage(Screen* scr, const string& str);
	Win* getMessWin();
	Win* getCmdWin();
public:
	Screen(Curse* cur, int startRow, int numScreens, int pos, Win* cmdWin = nullptr, Win* messWin = nullptr);
	~Screen();
	Win* createCmdWin();
	Win* createMessWin();
	void setStatus(const string& str1, const string& str2);
	void printMessage(const string& str);
	void printWarning(const string& str);
	void printCommand(const string& str);

	void push();
	void push(int r, int c);
	void pop();
	bool atBottom();
	int maxRow();
	void insertChar(char c);
	void insertLine(const string& line = "");
	void printBelow(const string& line);
	void delLine();
	void delLine(const string& s);
	void delLine(int r, const string& s = "");
	void delChar();
	void moveUp(int r = 1);
	void moveUp(const string& s);
	void moveDown(const string& s);
	void insertBreak(const string& s);
	void deleteBreak(const string& botRow, const string& delStr, const string& orgStr);
	void repaint(const vector<string>& data, int bufRow, int newRow = -1);
};

#endif /* DOMAIN_SCREEN_HPP_ */
