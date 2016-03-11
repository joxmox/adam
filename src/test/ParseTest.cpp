/*
 * ParseTest.cpp
 *
 *  Created on: Mar 11, 2016
 *      Author: joxmox
 */

#include <vector>
#include <string>

#include "Parse.hpp"

using namespace std;

using funcFun = void (*)(Buffer*, string*);
using funcVec = vector<funcFun>;

TEST(Parse, basic) {
}

Parse<funcFun> par;


static vector<pair<string, funcFun>> {
		{"mark *", cmdMark},
		{2, "line *"},
		{3, "goto *"},
		{4, "select"},
		{5, "delete"},
		{6, "insert"},
    };
	static Parse parse {ptab};
	string cmd = tty->readCmd();
	DBG << "parsing command: " << cmd << endl;
	int res = parse.decode(cmd);
	switch(res) {
	case -2 : tty->putMessage("No command given."); break;
	case -1 : tty->putMessage("Ambiguous command."); break;
	case  0 : tty->putMessage("Unrecognized command."); break;
	case  1: cmdMark(parse.getParam()); break;
	case  2: cmdLine(parse.getParam()); break;
	case  3: cmdGoto(parse.getParam()); break;
	default : tty->putMessage("Command #" + to_string(res) + " OK.");

	/*
	 *
	case  1 : cmdShowDefault(); break;
	case  2 : cmdShowScreen(); break;
	case  3 : cmdMark(parse.getParam()); break;
	case  4 : cmdLine(parse.getParam()); break;
	case  5 : cmdGoto(parse.getParam()); break;
	case  6 : cmdGoto(parse.getParam()); break;
	case  7 : cmdGoto(parse.getParam()); break;
	case  8 : cmdGoto(parse.getParam()); break;
	*/
	}
}


