/*
 * stringUtilsTest.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: joxmox
 */

#include <string>
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



