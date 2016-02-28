/*
 * Screen.cpp
 *
 *  Created on: Oct 20, 2015
 *      Author: joxmox
 */

#include "Window.hpp"

#include <string>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

using namespace std;
using namespace log4cxx;
int Window::winCnt = 0;


void Window::initScreen() {
	LoggerPtr logger{Logger::getLogger("Window.initScreen")};
	winId = ++winCnt;
	LOG4CXX_DEBUG(logger, "initializing window #" << winId);
	width = tty->getWidth();
	if (mode == full) {
		height = tty->getWidth();
		anchorRow = 0;
		anchorCol = 0;
	}
	LOG4CXX_DEBUG(logger, "done" << winId);
}


Window::Window(Tty* tty, scrMode mode) : tty(tty), mode(mode) {
	LoggerPtr logger{Logger::getLogger("Window")};
	LOG4CXX_DEBUG(logger, "creating window" << winId);
	initScreen();
}

void Window::moveTty(int row, int col) {
	tty->pos(anchorRow + row, anchorCol + col);
}

void Window::printChar(int key) {
	LoggerPtr logger{Logger::getLogger("Window::printChar")};
	LOG4CXX_TRACE(logger, "row:" << row << ", col:" << col);
	LOG4CXX_DEBUG(logger, "inserting key: " << key);
	moveTty(row, col++);
	LOG4CXX_TRACE(logger, "move done");
	tty->print(key);
	LOG4CXX_TRACE(logger, "print done");
	moveTty(row, col);
}


//ostream& operator <<(ostream& os, const Window scr) {
//	os << scr.toString();
//	return os;
//}
