/*
 * Screen.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: joxmox
 */

#include <string>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Screen.hpp"
#include "Curse.hpp"

using namespace std;
using namespace log4cxx;

Screen::Screen(Curse* cur, int startRow, int numScreens, int pos, Win* cmdWin, Win* messWin) :
		Win(cur, cur->getHeight() - 3, cur->getWidth(), 0, 0), cur(cur), cmdWin(cmdWin), messWin(messWin) {
	LoggerPtr logger{Logger::getLogger("Screen")};
	stsWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 3, 0);
}

Win* Screen::createCmdWin() {
	LoggerPtr logger{Logger::getLogger("Screen.createCmdWin")};
	LOG4CXX_DEBUG(logger, "Screen window id = " << id);
	cmdWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 2, 0);
	LOG4CXX_DEBUG(logger, "cmdWin = " << reinterpret_cast<long>(cmdWin));
	return cmdWin;
}

Win* Screen::createMessWin() {
	LoggerPtr logger{Logger::getLogger("Screen.createMessWin")};
	LOG4CXX_DEBUG(logger, "Screen window id = " << id);
	messWin = cur->creWin(1, cur->getWidth(), cur->getHeight() - 1, 0);
	LOG4CXX_DEBUG(logger, "messWin = " << reinterpret_cast<long>(messWin));
	return messWin;
}

void Screen::setStatus(const string& str1, const string& str2) {
	stsWin->pos(0, 0);
	stsWin->setAttr(attRev);
	stsWin->printStr(str1 + string(stsWin->getWidth() - str1.size() - str2.size(), ' ') + str2);
	stsWin->clearAttr(attRev);
	stsWin->refresh();
}

void Screen::printMessage(const string& str) {
	messWin->pos(0, 0);
	messWin->printStr(str);
	messWin->pos(0, str.size());
	messWin->clearEol();
	messWin->refresh();
}

