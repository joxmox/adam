/*
 * stringUtils.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: joxmox
 */


#include <vector>
#include <regex>
#include "stringUtils.hpp"

namespace str {

using namespace std;

vector<string> split(const string& str, const string& patt) {
	regex e(patt);
	vector<string> r;
	sregex_token_iterator x;
	sregex_token_iterator i (str.begin(), str.end(), e, -1);
	while (i != x) r.push_back(*(i++));
	return r;
}

vector<string> match(const string& str, const string& patt) {
	regex e(patt);
	smatch sm;
	vector<string> r;
	if (regex_match(str, sm, e)) {
		for (auto s : sm) {
			r.push_back(s.str());
		}
	}
	return r;
}

string lowercase(const string& str) {
        string res {str};
        for (auto i = 0; i < str.size(); i++) {
        	res[i] = tolower(res[i]);
        }
        return res;
}

}


