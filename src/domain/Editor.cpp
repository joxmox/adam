/*
 * Editor.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#include <string>
#include <map>
#include <stdexcept>
#include <functional>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Buffer.hpp"
#include "Curse.hpp"
#include "functions.hpp"
#include "Editor.hpp"

using namespace std;

Editor::Editor(const string& fileName) {
	this->fileName = fileName;
	LoggerPtr logger{Logger::getLogger("Editor")};
	LOG4CXX_DEBUG(logger, "instance created. filename is " << fileName);
}

Editor::~Editor() {
	LoggerPtr logger{Logger::getLogger("~Editor")};
	LOG4CXX_DEBUG(logger, "shutting down");
	if (tty != nullptr) delete tty;
	LOG4CXX_DEBUG(logger, "tty deleted");
}

int Editor::getKey() {
	return key;
}

void Editor::quit() {
	loop = false;
}

void Editor::edit() {
	LoggerPtr logger{Logger::getLogger("Editor.edit")};
	LOG4CXX_DEBUG(logger, "starting");
	initDispatch();
	tty = new Curse();
	Win* mainWin = tty->creWin(tty->getHeight() - 3, tty->getWidth(), 0, 0);
	Win* stsWin = tty->creWin(1, tty->getWidth(), tty->getHeight() - 3, 0);
	cmdWin = tty->creWin(1, tty->getWidth(), tty->getHeight() - 2, 0);
	messWin = tty->creWin(1, tty->getWidth(), tty->getHeight() - 1, 0);
	bufName = getBufferName(fileName);
	buf = new Buffer(bufName, fileName, mainWin, stsWin, messWin, cmdWin);
	bufMap[bufName] = buf;
	if (buf->fileExists()) {
		int readLines = buf->getLines();
		LOG4CXX_DEBUG(logger, readLines << " lines read from file");
	} else {
		LOG4CXX_DEBUG(logger, "editing new file");
	}
	mainLoop();
}

string Editor::getBufferName(const string& fileName) {
	return fileName;
}

void Editor::mainLoop() {
	LoggerPtr logger{Logger::getLogger("Editor.mainLoop")};
	while (loop) {
		buf->setFocus();
		LOG4CXX_TRACE(logger, "waiting for key from terminal");
		key = buf->getMainWin()->readKey();
		LOG4CXX_DEBUG(logger, "read key " << key << " from keyboard - dispatching...");
		disMap[key](this);
	}
}

void Editor::setDisp(int key, funcFun f) {
	disMap[key] = f;
}

void Editor::setDisp(int key1, int key2, funcFun f) {
	for (int i = key1; i <= key2; i++) {
	    disMap[i] = f;
	}
}



void Editor::initDispatch() {
	LoggerPtr logger{Logger::getLogger("Editor.initDispatch")};
	disMap = funcVec {1000, fun::illegalChar};
	setDisp(4, fun::debug);
	setDisp(26, fun::exit);
	setDisp(32, 126, fun::character);
	LOG4CXX_DEBUG(logger, "dispatch table initialized");
}

void Editor::insertChar() {
	LoggerPtr logger{Logger::getLogger(string("Editor") + string(__func__))};
	LOG4CXX_DEBUG(logger, "inserting character");
	buf->insertChar(key);
}

void Editor::debug() {
	LoggerPtr logger{Logger::getLogger("Editor.initDispatch")};
	LOG4CXX_DEBUG(logger, "dumping buffer:");
	buf->dump();
}







