#include <vector>
#include <regex>
#include "stringUtils.hpp"

#include "Parse.hpp"

using namespace std;


Parse::Parse(vector<pair<int, string>> iTable) {
  for (auto i : iTable) {
    string refCmd = i.second;
    vector<string> refCmds = split(refCmd, "\\s+");
    string regStr = "^\\s*";
 //   auto k = refCmds.begin();
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
    regStr += "$";
    pTable.push_back(make_pair(i.first, regex(regStr)));
  }
}


int Parse::decode(string cmd) {
  int res = 0;
  int cnt = 0;
  if (regex_match(cmd, regex("^\\s*$"))) return -2;
  smatch pMatch;
  for (auto p : pTable) {
    if (regex_match(cmd, pMatch, p.second)) {
      cnt++;
      res = p.first;
      if (pMatch.size() > 1) {
        this->param = pMatch[1].str();
      } else {
        this->param = "";
      }
    }
  }
  if (cnt > 1) {
    res = -1;
    this->param = "";
  }
  return res;
}

string Parse::getParam() {
  return this->param;
}

/*
int main() {
  static vector<pair<int, string>> vec1 {
    {1, "show default"},
    {2, "show disk"},
    {3, "show config"},
    {4, "mark *"},
  };
  Parse par {vec1};
  string line;
  while (getline(cin, line)) {
    int res = par.decode(line);
    cout << line << " > " << par.decode(line) << endl;
    if (res > 0 && par.getParam().length() > 0) {
      cout << "  param = " << par.getParam() << endl;
    }
  }
}
*/
