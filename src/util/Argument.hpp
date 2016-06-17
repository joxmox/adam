/*
 * arguments.hpp
 *
 *  Created on: Mar 22, 2016
 *      Author: joxmox
 */

#ifndef SRC_UTIL_ARGUMENT_HPP_
#define SRC_UTIL_ARGUMENT_HPP_

#include <string>
#include <vector>
#include <set>

#include "logging.hpp"

using namespace std;

class Argument {
	GET_LOGGER("Argument");
//	LoggerPtr logger{Logger::getLogger("Argument")};
	const string opFail = "Failed to parse options: ";
	int minParams = 0;
	int maxParams = 0;
	int argc;
	char** argv;
	vector<string> params;
	set<char> allOpts;
	map<string, char> longToShort;
	map<char, int> hasValue; // 0=no value(bool), 1=string, 2=int
	map<char, string> strValue;
	map<char, int> intValue;
	set<char> optSet;
	string errorText;
	bool validated = false;
private:
	void construct();
	void check();
	void check(char opt);
	void check(const string& opt);
	char popFirst(string& str);
	void anyOpt(char shortName, const string& longName);
	vector<string> getData(const string& shortName, const string& longName);
public:
	Argument(int argc, char* argv[]);
	Argument(int argc, const char* argv[]);
	void strOpt(char shortName, const string& longName);
	void intOpt(char shortName, const string& longName);
	void boolOpt(char shortName, const string& longName);
	void setParams(int minParams = 0, int maxParams = 0);
	bool validate();
	bool isSet(char shortName);
	bool isSet(const string& longName);
	string getStr(char shortName);
	string getStr(const string& longName);
	int getInt(char shortName);
	int getInt(const string& longName);
	string getError();
	string getParam(int i);
	int numParams();
	bool wantHelp();
};

Argument::Argument(int argc, char* argv[]): argc(argc), argv(argv) {
	construct();
}

Argument::Argument(int argc, const char* argv[]): argc(argc), argv(const_cast<char**>(argv)) {
	construct();
}

void Argument::construct() {
	boolOpt('h', "help");

}

void Argument::check() {
	if (!validated) throw logic_error("Arguments not validated!");
}

void Argument::check(char opt) {
	if (allOpts.count(opt) == 0) throw logic_error("Undefined option referenced: -" + opt);
	check();
}

void Argument::check(const string& opt) {
	if (longToShort.count(opt) == 0) throw logic_error("Undefined option referenced: --" + opt);
	check();
}

void Argument::anyOpt(char shortName, const string& longName) {
	allOpts.insert(shortName);
	longToShort[longName] = shortName;
}

void Argument::boolOpt(char shortName, const string& longName) {
	anyOpt(shortName, longName);
	hasValue[shortName] = 0;
}

void Argument::strOpt(char shortName, const string& longName) {
	anyOpt(shortName, longName);
	hasValue[shortName] = 1;
}

void Argument::intOpt(char shortName, const string& longName) {
	anyOpt(shortName, longName);
	hasValue[shortName] = 2;
}

void Argument::setParams(int minParams, int maxParams) {
	this->minParams = minParams;
	this->maxParams = max(minParams, maxParams);
}

bool Argument::validate() {
	validated = true;
	try {
		for (int k = 1; k < argc; ++k) {
			string arg = argv[k];
			string org = arg;
			bool fLong = false;
			if (arg[0] != '-') {
				params.push_back(arg);
				continue;
			}
			popFirst(arg);
			if (arg[0] == '-') {
				popFirst(arg);
				fLong = true;
			}
			if (arg.size() == 0) throw logic_error(opFail + "no option specified");
			char opt = fLong? longToShort[arg] : popFirst(arg);
			if (allOpts.count(opt) == 0) throw logic_error(opFail + "no such option: " + org);
			if ((fLong || arg.size() == 0) && hasValue[opt] > 0) {
				if (++k >= argc) throw logic_error(opFail + "no value specified for option " + org);
				arg = argv[k];
			}
			optSet.insert(opt);
			if (hasValue[opt] == 1) {
				strValue[opt] = arg;
			} else if (hasValue[opt] == 2) {
				try {
					intValue[opt] = stoi(arg);
				} catch (invalid_argument& e) {
					throw logic_error(opFail + "argument for option " + org + " must be numeric");
				}
			}
		}
		if (optSet.count('h') > 0) return true;
		if (params.size() > maxParams) throw logic_error(opFail + "too many parameters supplied");
		if (params.size() < minParams) throw logic_error(opFail + "too few parameters supplied");
	} catch (logic_error& e) {
		errorText = e.what();
		return false;
	}
	return true;
}

bool Argument::isSet(char shortName) {
	check(shortName);
	return optSet.count(shortName) == 1;
}

bool Argument::isSet(const string& longName) {
	check(longName);
	return isSet(longToShort[longName]);
}

string Argument::getStr(char shortName) {
	check(shortName);
	return strValue[shortName];
}

string Argument::getStr(const string& longName) {
	check(longName);
	return getStr(longToShort[longName]);
}

int Argument::getInt(char shortName) {
	check(shortName);
	return intValue[shortName];
}

int Argument::getInt(const string& longName) {
	check(longName);
	return getInt(longToShort[longName]);
}

char Argument::popFirst(string& str) {
	char c = str[0];
	str.erase(str.begin());
	return c;
}

string Argument::getError() {
	check();
	return errorText;
}

string Argument::getParam(int i) {
	check();
	if (i < 0) throw logic_error("There is no such thing as a negative parameter number");
	if (i >= params.size()) {
		if (params.size() == 0) {
			throw logic_error("Cannot get parameter #" + to_string(i) + ", there are no parameters");
		} else {
			throw logic_error("Cannot get parameter #" + to_string(i) + ", maximum number is " + to_string(params.size()));
		}
	}
	return params[i];
}

bool Argument::wantHelp() {
	return isSet('h');
}

int Argument::numParams() {
	return params.size();
}


#endif /* SRC_UTIL_ARGUMENT_HPP_ */
