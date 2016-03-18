/*
 * adamTest.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: joxmox
 */

#include <string>
#include <vector>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"
#include "gtest/gtest.h"

#include "Editor.hpp"

using namespace std;
using namespace log4cxx;

struct Adam : public ::testing::Test {
	LoggerPtr logger{Logger::getLogger("adamTest")};
	vector<string> resBuf1 {
		"Hello, world!",
		"This is a test",
		"012345678901234567890",
		"   ",
		"          The above line is blank",
		"the above line starts at pos 10",
		"this line contains a hash sign - #",
	};
	vector<string> cmdBuf1 {
		"version(1.1)",
		"text(hello world); sol; right; del",
		"text(H);right(4);text(,);eol;cr",
		"text(This is a test) ; cr ;up(2);eol #this is a comment",
		"text(!);",
		"down(3)",
		"text(The above line is blank);cr",
		"text(the above line starts at pos 12);sol;up(2)",
		"text(012345678901234567890);cr",
		"text(apa);kill;cr;down;eol;del",
		"text(0);cr;",
		"text(this line contains a hash sign - #);exit",
	};
	Adam() {
	    PropertyConfigurator::configure("conf/log4cxx.conf");
	}
	void createFile(const string& fn, const vector<string>& fBuf) {
		ofstream ofs {fn};
		for (auto b : fBuf) {
			ofs << b << endl;
		}
	}
};

TEST_F(Adam, edit1) {
	system("rm -f test.tmp");
	createFile("test.cmd", cmdBuf1);
	Editor ed {"test.tmp", "test.cmd", "", false};
	ed.edit();
	ifstream ifs {"test.tmp"};
	string line;
	vector<string> tstBuf1;
	while (getline(ifs, line)) {
		tstBuf1.push_back(line);
	}
	EXPECT_EQ(resBuf1.size() , tstBuf1.size());
	for (int i = 0; i < resBuf1.size(); i++) {
		EXPECT_EQ(resBuf1[i], tstBuf1[i]);
	}
}

//
//TEST_F(Adam, learn1) {
//	createFile("kaka.tmp", fBuf2);
//	Editor ed {fileName};
//	ed.edit(cBuf2, "");
//	ifstream ifs {"kaka.tmp"};
//	string line;
//	vector<string> tBuf2;
//	while (getline(ifs, line)) {
//		tBuf2.push_back(line);
//	}
//	EXPECT_EQ(rBuf2.size() , tBuf2.size());
//	for (int i = 0; i < rBuf2.size(); i++) {
//		EXPECT_EQ(rBuf2[i], tBuf2[i]);
//	}
//	EXPECT_EQ(rBuf2, tBuf2);
//}




