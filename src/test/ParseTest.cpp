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

class MiniBuf {
public:
	MiniBuf() {}
	static void cb1(MiniBuf* mb, vector<string>& pv) {
		xParam = pv;
		result = 17;
	}
	static void cb2(MiniBuf* mb, vector<string>& pv) {
		result = 666;
	}
	static void cb3(MiniBuf* mb, vector<string>& pv) {
		result = 4711;
		xParam = pv;
	}
	static void cb4(MiniBuf* mb, vector<string>& pv) {
		result = -1;
		xParam = pv;
	}
	int getResult() {
		return result;
	}
	string getParam(int i) {
		return xParam[i];
	}
};



struct ptest : public ::testing::Test {
	int result;
	MiniBuf* buf = new MiniBuf();
	map<string, Parse<MiniBuf>::cbFun> ct {
		{"hej", MiniBuf::cb1},
		{"hopp", MiniBuf::cb2},
		{"show *", MiniBuf::cb3},
		{"apa * *", MiniBuf::cb4},
	};
};

TEST_F(ptest, call_function) {
	Parse<MiniBuf> p {ct};
	p.decode(buf, "hej");
	EXPECT_EQ(17, buf->getResult());
	p.decode(buf, "hopp");
	EXPECT_EQ(666, buf->getResult());
	p.decode(buf, "hej");
	EXPECT_EQ(17, buf->getResult());
}

TEST_F(ptest, func_params) {
	Parse<MiniBuf> p {ct};
	p.decode(buf, "show xyzzy");
	EXPECT_EQ(4711, buf->getResult());
	EXPECT_EQ("xyzzy", buf->getParam(0));
	p.decode(buf, "apa hej hopp");
	EXPECT_EQ(-1, buf->getResult());
	EXPECT_EQ("hej", buf->getParam(0));
	EXPECT_EQ("hopp", buf->getParam(1));

}


