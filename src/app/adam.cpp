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
#include "Argument.hpp"

using namespace std;
using namespace log4cxx;

const string usage = " <file name> [--replay <filename>] [--record <filename>]";
const string opFail = "Failed to parse options: ";

LoggerPtr logger{Logger::getLogger("adam")};

void printUsage(char* argv[]) {
	cerr << "usage: " << argv[0] << usage << endl;
}

string getArg(int argc, char* argv[], int& k, const string& opt) {
	LOG4CXX_DEBUG(logger, "getting value for option " << opt);
	if (++k >= argc) {
		string error = opFail + "Missing argument for " + string(argv[k - 1]);
		cerr << error << endl;
		printUsage(argv);
		throw invalid_argument(error);
	}
	return argv[k];
}

int main(int argc, char* argv[]) {
    PropertyConfigurator::configure("conf/log4cxx.conf");
	LOG4CXX_DEBUG(logger, "adam starting");

	Argument arg {argc, argv};
	arg.setParams(1);
	arg.strOpt('p', "replay");
	arg.strOpt('r', "record");
	arg.boolOpt('o', "read-only");
	arg.intOpt('w', "wait");

	if (!arg.validate()) {
		cerr << arg.getError() << endl;
		return 1;
	}

    Editor ed {arg.getParam(0)};
    if (arg.isSet("replay")) ed.setReplay(arg.getStr("replay"));
    if (arg.isSet("record")) ed.setRecord(arg.getStr("record"));
    if (arg.isSet("read-only")) ed.setReadOnly();
    if (arg.isSet("wait")) ed.setWait(arg.getInt("wait"));

    LOG4CXX_DEBUG(logger, "starting editor session");
    ed.edit();
    LOG4CXX_DEBUG(logger, "editor session ended");
}





