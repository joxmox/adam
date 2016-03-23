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

TEST(argtest, normal) {
	const char* argv[] = {"test1", "--cow", "17", "-a", "-bhello", "foo",  "bar"};
	Argument a (7, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(2, 3);
	ASSERT_TRUE(a.validate());
	EXPECT_TRUE(a.isSet('a'));
	EXPECT_TRUE(a.isSet('b'));
	EXPECT_TRUE(a.isSet('c'));
	EXPECT_FALSE(a.isSet('d'));
	EXPECT_EQ(17, a.getInt("cow"));
	EXPECT_EQ(17, a.getInt('c'));
	EXPECT_EQ("hello", a.getStr("bat"));
	EXPECT_EQ(2, a.numParams());
	EXPECT_EQ("foo", a.getParam(0));
	EXPECT_EQ("bar", a.getParam(1));
	EXPECT_THROW(a.isSet('x'), logic_error);
	EXPECT_THROW(a.getParam(2), logic_error);
};

TEST(argtest, no_such_option) {
	const char* argv[] = {"error1", "--cow", "17", "-x", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	ASSERT_FALSE(a.validate());
	EXPECT_EQ("Failed to parse options: no such option: -x", a.getError());
}

TEST(argtest, no_argument) {
	const char* argv[] = {"error2", "--cow", "urk", "-a", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	ASSERT_FALSE(a.validate());
	EXPECT_EQ("Failed to parse options: argument for option --cow must be numeric", a.getError());
}

TEST(argtest, no_value) {
	const char* argv[] = {"error3", "--cow", "17", "-a", "-b"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	ASSERT_FALSE(a.validate());
	EXPECT_EQ("Failed to parse options: no value specified for option -b", a.getError());
}

TEST(argtest, no_option_specified) {
	const char* argv[] = {"error4", "--cow", "17", "-", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	ASSERT_FALSE(a.validate());
	EXPECT_EQ("Failed to parse options: no option specified", a.getError());
}

TEST(argtest, too_few_params) {
	const char* argv[] = {"error7", "--cow", "17", "-a", "-bhello"};
	Argument a (5, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	bool ok = a.validate();
	EXPECT_EQ(0, ok);
	EXPECT_EQ("Failed to parse options: too few parameters supplied", a.getError());
}

TEST(argtest, to_many_params) {
	const char* argv[] = {"error6", "--cow", "17", "-a", "-bhello", "foo", "bar"};
	Argument a (7, argv);
	a.boolOpt('a', "ape");
	a.strOpt('b', "bat");
	a.intOpt('c', "cow");
	a.boolOpt('d', "dog");
	a.setParams(1);
	ASSERT_FALSE(a.validate());
	EXPECT_EQ("Failed to parse options: too many parameters supplied", a.getError());
}

