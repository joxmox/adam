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

LoggerPtr logger{Logger::getLogger("adam")};

int main(int argc, char* argv[]) {
    PropertyConfigurator::configure("conf/log4cxx.conf");
	LOG4CXX_DEBUG(logger, "adam starting");
	string usage = "usage: " + string(argv[0]) + " <file name> [--debug <debug level>]";

	string fileName;
	string input;
	vector<int>inKeys;
	string record;

	if (argc > 1) {
		fileName = argv[1];
	}

	for (int k = 2; k < argc; ++k) {
		string arg = argv[k];
		if (arg == "--input") {
			LOG4CXX_DEBUG(logger, "option input: " << argv[++k]);
			input = argv[k];
		} else if (arg == "--record") {
			LOG4CXX_DEBUG(logger, "record input: " << argv[++k]);
			record = argv[k];
		} else {
			cerr << argv[0] << "Invalid option -- " << arg << endl << usage << endl;
			return 1;
		}

	}

    if (fileName.empty()) {
    	cerr << argv[0] << "No filename specified" << endl << usage << endl;
    	return 1;
    }

    if (!input.empty()) {
    	ifstream ifs {input};
    	if (!ifs) throw runtime_error("could not open " + input + " for input");
    	string line;
    	while (getline(ifs, line)) {
    		inKeys.push_back(stoi(line));
    	}
    }

    LOG4CXX_DEBUG(logger, "command line ok. filename is " << fileName);

    Editor ed {fileName};
    LOG4CXX_DEBUG(logger, "editor instance created - starting editing");
    ed.edit(inKeys, record);
    LOG4CXX_DEBUG(logger, "editor session ended");
}





