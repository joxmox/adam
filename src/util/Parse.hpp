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

#include "log4cxx/logger.h"


using namespace std;
using namespace log4cxx;

template<typename Object>
class Parse {
public:
	using cbFun = void (*)(Object*, const vector<string>&);
	using cbErr = void (*)(Object*, int);
private:
	Object* obj;
	cbErr errFunc = nullptr;
	LoggerPtr logger{Logger::getLogger("Parse")};
	vector<pair<regex, cbFun>> funcArr;
	map<string, int>parseMap;
public:
    Parse(Object* obj, const map<string, cbFun>& inTable, cbErr errFunc): obj(obj), errFunc(errFunc) {
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

    void decode(const string& cmd) const {
    	LOG4CXX_TRACE(logger, "enter");
    	cbFun res;
    	int cnt = 0;
    	if (regex_match(cmd, regex("^\\s*$"))) {
			LOG4CXX_DEBUG(logger, "no command given");
			errFunc(obj, 1);
    		return;
    	}
		vector<string> params;
    	smatch pMatch;
    	for (auto p : funcArr) {
    		if (regex_match(cmd, pMatch, p.first)) {
    			if (++cnt > 1) {
    				LOG4CXX_DEBUG(logger, "command '" << cmd << "' is ambiguous");
    				errFunc(obj, 2);
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
			LOG4CXX_DEBUG(logger, "command '" << cmd << "' could not be parsed");
			errFunc(obj, 3);
		} else {
			LOG4CXX_DEBUG(logger, "command '" << cmd << "' is valid - dispatching");
			res(obj, params);
		}
		LOG4CXX_TRACE(logger, "exit");
    }
};


#endif /* PARSE_HPP_ */

