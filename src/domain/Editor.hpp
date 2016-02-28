/*
 * Editor.hpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#ifndef DOMAIN_EDITOR_HPP_
#define DOMAIN_EDITOR_HPP_

#include <string>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Buffer.hpp"
#include "Tty.hpp"

using namespace std;
using namespace log4cxx;

class Editor {
	Tty* tty = nullptr;
//	Window* win = nullptr;
	Buffer* buf = nullptr;
	string bufName;
	map<string, Buffer*> bufMap;
	string fileName;
	string getBufferName(const string& fileName);
public:
	Editor(const string& fileName);
	~Editor();
	void edit();
};

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

void Editor::edit() {
	LoggerPtr logger{Logger::getLogger("Editor.edit")};
	LOG4CXX_DEBUG(logger, "starting");
	tty = new Tty();
//	win = new Window(tty);
	buf = new Buffer(fileName);
	bufName = getBufferName(fileName);
	bufMap[bufName] = buf;


}

string Editor::getBufferName(const string& fileName) {
	return fileName;
}



#endif /* DOMAIN_EDITOR_HPP_ */
