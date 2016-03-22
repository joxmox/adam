/*
 * ArgumentTest.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: joxmox
 */


#include <string>
#include <vector>

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"
#include "gtest/gtest.h"

#include "Argument.hpp"

using namespace std;
using namespace log4cxx;

TEST(argtest, test1) {
	const char* argv[] = {"ArgumentTest", "--cow", "17", "-a", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(1, ok);
	EXPECT_EQ(1, a.isSet('a'));
	EXPECT_EQ(1, a.isSet('b'));
	EXPECT_EQ(1, a.isSet('c'));
	EXPECT_EQ(0, a.isSet('d'));
	EXPECT_EQ(17, a.getInt("cow"));
	EXPECT_EQ(17, a.getInt('c'));
	EXPECT_EQ("hello", a.getStr("bat"));
};

TEST(argtest, error1) {
	const char* argv[] = {"ArgumentTest", "--cow", "17", "-x", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(0, ok);
	EXPECT_EQ("Failed to parse options: no such option: -x", a.getError());
}

TEST(argtest, error2) {
	const char* argv[] = {"ArgumentTest", "--cow", "urk", "-a", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(0, ok);
	EXPECT_EQ("Failed to parse options: argument for option --cow must be numeric", a.getError());
}

TEST(argtest, error3) {
	const char* argv[] = {"ArgumentTest", "--cow", "17", "-a", "-b"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(0, ok);
	EXPECT_EQ("Failed to parse options: no value specified for option -b", a.getError());
}

TEST(argtest, error4) {
	const char* argv[] = {"ArgumentTest", "--cow", "17", "-", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(0, ok);
	EXPECT_EQ("Failed to parse options: no option specified", a.getError());
}
