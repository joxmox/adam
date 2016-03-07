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
	string fileName = "kaka.tmp";
	vector<string> fBuf1 {
		"Hej hopp",
		"gummisnopp",
	};
	vector<string> fBuf2 {
		"123",
		"abc",
		"xyz",
		"",
		"previous line is blank",
		"this line is not blank",
		"12345!\"#$%@",
		"aa",
		"bb",
		"cc",
		"dd",
		"ee",
		"ff",
		"1",
		"22",
		"333",
		"4444",
		"55555",
		"666666",
		"7777777",
		"88888888",
		"999999999",
		"0000000000",
		"a",
		"ab",
		"abc",
		"abcd",
		"abcde",
		"abcdef",
		"abcdefg",
		"abcdefgi",
		"abcdefghij",
		"x",
		"xx x",
		"xxx xx x",
		"xxxx xxx xx x",
		"xxxxx xxxx xxx xx x",
		"xxxxxx xxxxx xxxx xxx xx x",
	};
//	std::queue<int> data{std::vector<int>{1,2,3,4}};
	vector<int> cBuf1 {
		5,
		260,
		260,
		13,
		258,
		261,
		261,
		261,
		261,
		261,
		263,
		121,
		8,
		88,
		32,
		259,
		260,
		260,
		263,
		260,
		260,
		260,
		13,
		26,
	};	vector<int> cBuf2 {
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		11,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		11,
		11,
		11,
		11,
		11,
		11,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		259,
		12,
		5,
		59,
		258,
		8,
		18,
		1,
		1,
		1,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		258,
		263,
		259,
		8,
		263,
		260,
		260,
		260,
		260,
		263,
		260,
		260,
		260,
		263,
		8,
		263,
		258,
		260,
		263,
		258,
		8,
		263,
		258,
		8,
		263,
		1,
		26,
	};
	vector<string> rBuf1 {
		"Hej",
		" hopp",
		"X gummysnopp",
	};	vector<string> rBuf2 {
		"123;",
		"abc;",
		"xyz;",
        "",
		"previous line is blank",
		"this line is not blank",
		"12345!\"#$%@",
		"aababcabcdabcdeabcdefabcdefgabcdefgiabcdefghij;",
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
	createFile("kaka.tmp", fBuf1);
	Editor ed {fileName};
	ed.edit(cBuf1, "");
	ifstream ifs {"kaka.tmp"};
	string line;
	vector<string> tBuf1;
	while (getline(ifs, line)) {
		tBuf1.push_back(line);
	}
	EXPECT_EQ(rBuf1.size() , tBuf1.size());
	for (int i = 0; i < rBuf1.size(); i++) {
		EXPECT_EQ(rBuf1[i], tBuf1[i]);
	}
	EXPECT_EQ(rBuf1, tBuf1);
}

TEST_F(Adam, learn1) {
	createFile("kaka.tmp", fBuf2);
	Editor ed {fileName};
	ed.edit(cBuf2, "");
	ifstream ifs {"kaka.tmp"};
	string line;
	vector<string> tBuf2;
	while (getline(ifs, line)) {
		tBuf2.push_back(line);
	}
	EXPECT_EQ(rBuf2.size() , tBuf2.size());
	for (int i = 0; i < rBuf2.size(); i++) {
		EXPECT_EQ(rBuf2[i], tBuf2[i]);
	}
	EXPECT_EQ(rBuf2, tBuf2);
}




