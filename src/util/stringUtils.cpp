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

string decomment(const string& str, char comChr, char startQuote) {
	string res;
	char endQuote = startQuote;
	switch (startQuote) {
		case '(': endQuote = ')'; break;
		case '[': endQuote = ']'; break;
		case '{': endQuote = '}'; break;
	}
	bool inQuote = false;
	for (auto c : str) {
		if (c == comChr && !inQuote) {
			break;
		} else {
			if (c == startQuote) {
				inQuote = true;
			} else if (c == endQuote) {
				inQuote = false;
			}
			res += c;
		}
	}
	return res;
}


string trimRight(const string& str) {
	string res {str};
	auto pos = res.size();
	while (res.size() > 0 && (res[pos - 1] == ' ' || res[pos - 1] == '\t')) pos--;
	if (pos != res.size()) res = str.substr(0, pos);
	return res;
}

string trimLeft(const string& str) {
	string res {str};
	auto pos = -1;
	while (res.size() > 0 && (res[pos + 1] == ' ' || res[pos - 1] == '\t')) pos++;
	if (pos != -1) res = str.substr(pos + 1, str.size() - pos - 1);
	return res;
}

string trim(const string& str) {
	return trimLeft(trimRight(str));
}

}


