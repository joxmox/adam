/*
 * stringUtils.hpp
 *
 *  Created on: Feb 27, 2016
 *      Author: joxmox
 */

#ifndef STRINGUTILS_HPP_
#define STRINGUTILS_HPP_

#include <vector>
#include <regex>

namespace str {

using namespace std;

vector<string> split(const string& str, const string& patt);
vector<string> match(const string& str, const string& patt);
string lowercase(const string& str);

}


#endif /* STRINGUTILS_HPP_ */
