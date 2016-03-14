/*
 * ParseTest.cpp
 *
 *  Created on: Mar 11, 2016
 *      Author: joxmox
 */

#include <vector>
#include <string>
#include <map>

#include "gtest/gtest.h"


#include "Parse.hpp"

using namespace std;

int result = 0;
vector<string> xParam;

class TestClass {
public:
	TestClass() {}
	static void cb1(TestClass* mb, const vector<string>& pv) {
		xParam = pv;
		result = 17;
	}
	static void cb2(TestClass* mb, const vector<string>& pv) {
		result = 666;
	}
	static void cb3(TestClass* mb, const vector<string>& pv) {
		result = 4711;
		xParam = pv;
	}
	static void cb4(TestClass* mb, const vector<string>& pv) {
		result = -1;
		xParam = pv;
	}
	int getResult() {
		return result;
	}
	string getParam(int i) {
		return xParam[i];
	}
	static void errFunc(TestClass* mb, int errCode) {
		result = errCode;
	}
};



struct parse_test : public ::testing::Test {
	int result;
	TestClass* tst = new TestClass();
	map<string, Parse<TestClass>::cbFun> ct {
		{"hej", TestClass::cb1},
		{"hopp", TestClass::cb2},
		{"show *", TestClass::cb3},
		{"apa * *", TestClass::cb4},
	};
	Parse<TestClass> p {tst, ct, tst->errFunc};
};

TEST_F(parse_test, call_function) {
	p.decode("hej");
	EXPECT_EQ(17, tst->getResult());
	p.decode("hopp");
	EXPECT_EQ(666, tst->getResult());
	p.decode("hej");
	EXPECT_EQ(17, tst->getResult());
}

TEST_F(parse_test, func_params) {
	p.decode("show xyzzy");
	EXPECT_EQ(4711, tst->getResult());
	EXPECT_EQ("xyzzy", tst->getParam(0));
	p.decode("apa hej hopp");
	EXPECT_EQ(-1, tst->getResult());
	EXPECT_EQ("hej", tst->getParam(0));
	EXPECT_EQ("hopp", tst->getParam(1));
}

TEST_F(parse_test, errors) {
	p.decode("");
	EXPECT_EQ(1, tst->getResult());
	p.decode("h");
	EXPECT_EQ(2, tst->getResult());
	p.decode("x");
	EXPECT_EQ(3, tst->getResult());
}


