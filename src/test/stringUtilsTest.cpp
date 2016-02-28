/*
 * stringUtilsTest.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: joxmox
 */

#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "stringUtils.hpp"

using namespace std;
using namespace str;

TEST(split, basic) {
	string str {"The quick brown fox"};
	vector<string> result {str::split(str, " ")};
	EXPECT_EQ(4, result.size());
	EXPECT_EQ("The", result[0]);
	EXPECT_EQ("quick", result[1]);
	EXPECT_EQ("brown", result[2]);
	EXPECT_EQ("fox", result[3]);
}

TEST(split, multi_char) {
	string str {"two  spaces"};
	vector<string> result {str::split(str, " ")};
	EXPECT_EQ(3, result.size());
	EXPECT_EQ("two", result[0]);
	EXPECT_EQ("", result[1]);
	EXPECT_EQ("spaces", result[2]);
	result = str::split("two  spaces", "\\s+");
	EXPECT_EQ(2, result.size());
	EXPECT_EQ("two", result[0]);
	EXPECT_EQ("spaces", result[1]);
}



