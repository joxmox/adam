/*
 * parse.hpp
 *
 *  Created on: Oct 18, 2015
 *      Author: joxmox
 */

#ifndef PARSE_HPP_
#define PARSE_HPP_

#include <vector>
#include <map>
#include <string>
#include <regex>
#include "stringUtils.hpp"

using namespace std;

template<typename Object>
class Parse {
public:
	using cbFun = void (*)(Object*, vector<string>&);
private:
	vector<pair<regex, cbFun>> funcArr;
	map<string, int>parseMap;
public:
    Parse(const map<string, cbFun>& inTable) {
    	for (auto i : inTable) {
    		vector<string> refCmds = str::split(i.first, "\\s+");
    		string regStr = "^\\s*";
    	    for (auto j=refCmds.begin(); j<refCmds.end(); j++) {
    	      if (j != refCmds.begin()) regStr += "\\s+";
    	      if (*j == "*") {
    	        regStr += "(\\S+)\\s*";
    	      } else {
    	        regStr += "(?:";
    	        string chars;
    	        for (auto c : *j) {
    	          if (chars.length() > 0) regStr += "|";
    	          chars += c;
    	          regStr += chars;
    	        }
    	        regStr += ")";
    	      }
    	    }
    	    funcArr.push_back(make_pair(regex(regStr + "$"), i.second));
    	}
    }

    void decode(Object* obj, const string& cmd) const {
    	cbFun res;
    	int cnt = 0;
    	if (regex_match(cmd, regex("^\\s*$"))) {
    		// callback no command;
    		return;
    	}
		vector<string> params;
    	smatch pMatch;
    	for (auto p : funcArr) {
    		if (regex_match(cmd, pMatch, p.first)) {
    			if (++cnt > 1) {
    				// callback ambigous;
    				return;
    			} else {
    				res = p.second;
    				for (auto i = 1; i < pMatch.size(); i++) {
    					params.push_back(pMatch[i].str());
    				}
    			}
    		}
    	}
		if (cnt == 0) {
			// callback no match
		} else {
			res(obj, params);
		}
    }
};


#endif /* PARSE_HPP_ */

