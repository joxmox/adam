/*
 * Screen.hpp
 *
 *  Created on: Mar 2, 2016
 *      Author: joxmox
 */

#ifndef DOMAIN_SCREEN_HPP_
#define DOMAIN_SCREEN_HPP_

#include "Curse.hpp"

class Screen : public Win {
	Curse* cur;
	Win* cmdWin;
	Win* messWin;
	Win* stsWin = nullptr;
	vector<pair<int, int>> posStack;
public:
	~Screen() = default;
	Screen(Curse* cur, int startRow, int numScreens, int pos, Win* cmdWin = nullptr, Win* messWin = nullptr);
	Win* createCmdWin();
	Win* createMessWin();
	void setStatus(const string& str1, const string& str2);
	void printMessage(const string& str);
	void push();
	void push(int r, int c);
	void pop();
	bool atBottom();
	int maxRow();
};

#endif /* DOMAIN_SCREEN_HPP_ */
