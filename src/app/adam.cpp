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
	string usage = "usage: " + string(argv[0]) + " <file name> [--replay <filename>] [--record <filename>]";

	vector<string> params;
	string input;
	string record;
	for (int k = 1; k < argc; ++k) {
		string arg = argv[k];
		if (arg[0] == '-') {
			if (arg == "--replay" || arg == "-p") {
				LOG4CXX_DEBUG(logger, "option replay: " << argv[++k]);
				input = argv[k];
			} else if (arg == "--record" || arg == "-r") {
				LOG4CXX_DEBUG(logger, "option record: " << argv[++k]);
				record = argv[k];
			} else {
				cerr << argv[0] << "Invalid option -- " << arg << endl << usage << endl;
				return 1;
			}
		} else {
			params.push_back(arg);
		}

	}

	string fileName;
	if (params.size() > 0) fileName = params[0];

    if (fileName.empty()) {
    	cerr << argv[0] << "No filename specified" << endl << usage << endl;
    	return 1;
    }

    LOG4CXX_DEBUG(logger, "command line ok. filename is " << fileName);

    Editor ed {fileName};
    LOG4CXX_DEBUG(logger, "editor instance created - starting editing");
    ed.edit(input, record);
    LOG4CXX_DEBUG(logger, "editor session ended");
}





