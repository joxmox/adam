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

const string usage = " <file name> [--replay <filename>] [--record <filename>]";

LoggerPtr logger{Logger::getLogger("adam")};

void printUsage(char* argv[]) {
	cerr << "usage: " << argv[0] << usage << endl;
}

string getArg(int argc, char* argv[], int& k, const string& opt) {
	LOG4CXX_DEBUG(logger, "getting value for option " << opt);
	if (++k >= argc) {
		string error = "Failed to parse arguments: Missing argument for " + string(argv[k - 1]);
		cerr << error << endl;
		printUsage(argv);
		throw invalid_argument(error);
	}
	return argv[k];
}

int main(int argc, char* argv[]) {
    PropertyConfigurator::configure("conf/log4cxx.conf");
	LOG4CXX_DEBUG(logger, "adam starting");

	vector<string> params;
	string input;
	string record;
	bool readOnly = false;
	try {
	for (int k = 1; k < argc; ++k) {
		string arg = argv[k];
		if (arg[0] == '-') {
			if (arg == "--replay" || arg == "-p") {
				input = getArg(argc, argv, k, "-p");
			} else if (arg == "--record" || arg == "-r") {
				record = getArg(argc, argv, k, "-r");
			} else if (arg == "--readonly" || arg == "-o") {
				LOG4CXX_DEBUG(logger, "option readonly set: ");
				readOnly = true;
			} else {
				cerr << argv[0] << "Invalid option -- " << arg << endl << usage << endl;
				return 1;
			}
		} else {
			params.push_back(arg);
		}

	}
	} catch (invalid_argument& e) {
		return 1;
	}

	string fileName;
	if (params.size() > 0) fileName = params[0];

    if (fileName.empty()) {
    	cerr << argv[0] << "No filename specified" << endl << usage << endl;
    	return 1;
    }

    LOG4CXX_DEBUG(logger, "command line ok. filename is " << fileName);

    Editor ed {fileName, input, record, readOnly};
    LOG4CXX_DEBUG(logger, "editor instance created - starting editing");
    ed.edit();
    LOG4CXX_DEBUG(logger, "editor session ended");
}





