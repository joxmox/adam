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
#include "Tty.hpp"
#include "functions.hpp"
#include "Editor.hpp"
#include "Window.hpp"

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
	tty = new Tty();
	win = new Window(tty);
	bufName = getBufferName(fileName);
	buf = new Buffer(bufName, fileName, win);
	bufMap[bufName] = buf;
	if (buf->fileExists()) {
		int readLines = buf->getLines();
		LOG4CXX_DEBUG(logger, readLines << " lines read from file");
	} else {
		LOG4CXX_DEBUG(logger, "editing new file");
	}
	initDispatch();
	mainLoop();
}

string Editor::getBufferName(const string& fileName) {
	return fileName;
}

void Editor::mainLoop() {
	LoggerPtr logger{Logger::getLogger("Editor.mainLoop")};
	while (loop) {
		key = tty->readKey();
		LOG4CXX_DEBUG(logger, "read key " << key << " from keyboard");
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







