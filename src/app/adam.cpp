/*
 * adam.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: joxmox
 */

#include <string>
#include <iostream>

#include "logging.hpp"
#ifdef LOG4CXX
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/fileappender.h"
#include "log4cxx/simplelayout.h"
#include "log4cxx/helpers/pool.h"
#endif


#include "Editor.hpp"
#include "Argument.hpp"

using namespace std;
#ifdef LOG4CXX
using namespace log4cxx;
#endif

const string usage = " [file] [-p <file>] [-r <file>] [-w <ms>] [-o]";
const string opFail = "Failed to parse options: ";

GET_LOGGER("Adam");

void printUsage(char* argv[]) {
	cerr << "usage: " << argv[0] << usage << endl;
}

void printHelp(char* argv[]) {
	cerr << "usage: " << argv[0] << usage << endl;
	cerr << R"(
Short  Long option      Function
-----  -----------      --------
-p     --replay <file>  Reads simulated keystrokes from file
-r     --record <file>  Writes all activity to file for later replay
-w     --wait <ms>      When processing a replay file, sleep between keys
-o     --read-only      Opens file write protected  
)";
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

	Argument arg {argc, argv};
	arg.setParams(0, 1);
	arg.strOpt('p', "replay");
	arg.strOpt('r', "record");
	arg.boolOpt('o', "read-only");
	arg.intOpt('w', "wait");
	arg.strOpt('l', "logfile");

	if (!arg.validate()) {
		cerr << arg.getError() << endl;
		printUsage(argv);
		return 1;
	}

	if (arg.wantHelp()) {
		printHelp(argv);
		return 0;
	}

#ifdef LOG4CXX
	cout << "log - yes" << endl;
	if (ifstream configTest {"conf/log4cxx.conf"}) {
		cout << "conf - yes" << endl;
		PropertyConfigurator::configure("conf/log4cxx.conf");
	} else {
		cout << "conf - no" << endl;
		string logSpec {"/dev/null"};
		if (arg.isSet("logfile")) logSpec = arg.getStr("logfile");
		FileAppender* fileAppender = new FileAppender(LayoutPtr(new SimpleLayout()), logSpec, false);
		helpers::Pool p;
		fileAppender->activateOptions(p);
		log4cxx::BasicConfigurator::configure(log4cxx::AppenderPtr(fileAppender));
	}
#endif

	LOG4CXX_DEBUG(logger, "adam starting");

    Editor ed {arg.getParam(0, true)};
    if (arg.isSet("replay")) ed.setReplay(arg.getStr("replay"));
    if (arg.isSet("record")) ed.setRecord(arg.getStr("record"));
    if (arg.isSet("read-only")) ed.setReadOnly();
    if (arg.isSet("wait")) ed.setWait(arg.getInt("wait"));

    LOG4CXX_DEBUG(logger, "starting editor session");
    ed.edit();
    LOG4CXX_DEBUG(logger, "editor session ended");

    return 0;
}





