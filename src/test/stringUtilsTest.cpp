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

TEST(trim, left) {
	EXPECT_EQ("kalle anka", str::trimLeft("kalle anka"));
	EXPECT_EQ("kalle anka", str::trimLeft(" kalle anka"));
	EXPECT_EQ("kalle anka", str::trimLeft("   kalle anka"));
	EXPECT_EQ("kalle anka  ", str::trimLeft("  kalle anka  "));
}

TEST(trim, right) {
	EXPECT_EQ("kalle anka", str::trimRight("kalle anka"));
	EXPECT_EQ("kalle anka", str::trimRight("kalle anka "));
	EXPECT_EQ("kalle anka", str::trimRight("kalle anka   "));
	EXPECT_EQ("  kalle anka", str::trimRight("  kalle anka  "));
}

TEST(trim, both) {
	EXPECT_EQ("a", "a");
	EXPECT_EQ("kalle anka", str::trim("kalle anka"));
	EXPECT_EQ("kalle anka", str::trim("   kalle anka"));
	EXPECT_EQ("kalle anka", str::trim("kalle anka   "));
	EXPECT_EQ("kalle anka", str::trim("  kalle anka  "));
	EXPECT_EQ("kalle   anka", str::trim("  kalle   anka  "));
}

TEST(match, basic) {
	string str {"sune=(17)"};
	string patt = R"((.*)=\((.*)\))";
	vector<string> result {str::match(str, patt)};
	EXPECT_EQ(3, result.size());
	EXPECT_EQ("sune", result[1]);
	EXPECT_EQ("17", result[2]);
}

TEST(match, complex) {
	string str1 {"down()"};
	string str2 {"delete(17)"};
	string str3 {"string(\"kalle anka\")"};
	string patt = R"(^(.*?)\(\"?(.*?)\"?\)$)";
	vector<string> result;
	result = str::match(str1, patt);
	EXPECT_EQ(3, result.size());
	EXPECT_EQ("down", result[1]);
	EXPECT_EQ("", result[2]);
	result = str::match(str2, patt);
	EXPECT_EQ(3, result.size());
	EXPECT_EQ("delete", result[1]);
	EXPECT_EQ("17", result[2]);
	result = str::match(str3, patt);
	EXPECT_EQ("string", result[1]);
	EXPECT_EQ("kalle anka", result[2]);
}




