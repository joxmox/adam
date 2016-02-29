/*
 * adam.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#include <string>
#include <iostream>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

#include "Editor.hpp"

using namespace std;
using namespace log4cxx;


LoggerPtr logger{Logger::getLogger("main")};


int main(int argc, char* argv[]) {
    PropertyConfigurator::configure("conf/log4cxx.conf");
	LOG4CXX_DEBUG(logger, "adam starting");
	string usage = "usage: " + string(argv[0]) + " <file name> [--debug <debug level>]";

	string fileName;
	int dbgMask {0}; // just a dummy for time beeing
	if (argc > 1) {
		fileName = argv[1];
	}
	for (int k = 2; k < argc; ++k) {
		string arg = argv[k];
		if (arg == "--debug") {
			dbgMask = stoi(argv[++k]);
		} else {
			cerr << argv[0] << "Invalid option -- " << arg << endl << usage << endl;
			return 1;
		}
	}
	dbgMask++; // dummy op;

    if (fileName.empty()) {
    	cerr << argv[0] << "No filename specified" << endl << usage << endl;
    	return 1;
    }

    LOG4CXX_DEBUG(logger, "command line ok. filename is " << fileName);

    Editor ed {fileName};
    ed.edit();

}





